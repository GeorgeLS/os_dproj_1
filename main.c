#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include "common.h"
#include "report.h"
#include "utils.h"
#include "dynamic_buffer.h"
#include "tokenizer.h"
#include "bloom_filter.h"
#include "rb_tree.h"
#include "voter.h"
#include "commands.h"

#define INPUT_FILE_OPTION "-i"
#define OUTPUT_FILE_OPTION "-o"
#define UPDATES_N_OPTION "-n"

#define LBF_COMMAND "lbf"
#define LRB_COMMAND "lrb"
#define INS_COMMAND "ins"
#define FIND_COMMAND "find"
#define DELETE_COMMAND "delete"
#define VOTE_COMMAND "vote"
#define LOAD_COMMAND "load"
#define VOTED_COMMAND "voted"
#define VOTED_PER_POSTCODE_COMMAND "votedperpc"
#define EXIT_COMMAND "exit"

global int output_fd;
global bool running = true;
global bloom_filter *bf;
global rb_tree tree;
dynamic_buffer *stdin_buffer;

typedef struct program_options {
  char *input_file;
  char *output_file;
  i64 nupdates;
} program_options;

global program_options default_options = {
    .input_file = NULL,
    .output_file = "election.log",
    .nupdates = 5U,
};

bool is_option(const char *restrict arg) {
  size_t arg_len = strlen(arg);
  return !strncmp(arg, "-i", arg_len) &&
      !strncmp(arg, "-o", arg_len) &&
      !strncmp(arg, "-n", arg_len);
}

__NO_RETURN internal void
report_invalid_option_and_exit(const char *arg, const char *invalid_arg) {
  report_error("Argument \"%s\" is invalid for option \"%s\"",
               invalid_arg, arg);
  exit(EXIT_FAILURE);
}

internal void
check_option_argument_and_exit_if_invalid(const char *option, const char *arg) {
  if (is_option(arg)) {
    report_invalid_option_and_exit(option, arg);
  }
}

program_options get_program_options(int argc, char *args[]) {
  for (size_t i = 1U; i != argc; ++i) {
    const char *arg = args[i];
    size_t arg_len = strlen(arg);
    const char *next_arg = args[i + 1U];
    if (!strncmp(arg, INPUT_FILE_OPTION, arg_len)) {
      check_option_argument_and_exit_if_invalid(arg, next_arg);
      default_options.input_file = next_arg;
      ++i;
    } else if (!strncmp(arg, OUTPUT_FILE_OPTION, arg_len)) {
      check_option_argument_and_exit_if_invalid(arg, next_arg);
      default_options.output_file = next_arg;
      ++i;
    } else if (!strncmp(arg, UPDATES_N_OPTION, arg_len)) {
      check_option_argument_and_exit_if_invalid(arg, next_arg);
      i64 updates_n;
      if (!string_to_i64(next_arg, &updates_n) && updates_n <= 0) {
        fatal("Invalid argument \"%s\" in option \"%s\"",
              next_arg, UPDATES_N_OPTION);
      }
    } else {
      fatal("Unknown option \"%s\"", arg);
    }
  }
  return default_options;
}

void process_command(const char *restrict command) {
  tokenizer tokenizer = {
      .stream = command,
      .length = strlen(command),
      .index = 0U,
      .delimiter = ' '
  };

  while (tokenizer_has_next(&tokenizer)) {
    char *command_token = tokenizer_next_token(&tokenizer);
    size_t command_len = strlen(command_token);
    string_to_lowercase(command_token);

    size_t remaining_tokens = tokenizer_remaining_tokens(&tokenizer);

    if (!strncmp(command_token, LBF_COMMAND, command_len)) {
      if (remaining_tokens != 1U) {
        report_error("lbf command requires exactly one argument");
        break;
      }
      char *key = tokenizer_next_token(&tokenizer);
      lbf_command(output_fd, bf, key);
    } else if (!strncmp(command_token, LRB_COMMAND, command_len)) {
      if (remaining_tokens != 1U) {
        report_error("lrb command requires exactly one argument");
        break;
      }
      char *key = tokenizer_next_token(&tokenizer);
      lrb_command(output_fd, &tree, key);
    } else if (!strncmp(command_token, INS_COMMAND, command_len)) {
      if (remaining_tokens != 6) {
        report_error("ins record requires exactly six arguments");
        break;
      }
      ins_command(output_fd, bf, &tree, &tokenizer.stream[tokenizer.index]);
      // TODO(Gliontos): check for number of updates and restructure bloom
      //  filter
    } else if (!strncmp(command_token, FIND_COMMAND, command_len)) {
      if (remaining_tokens != 1U) {
        report_error("find command requires exactly one argument");
        break;
      }
      char *key = tokenizer_next_token(&tokenizer);
      find_command(output_fd, bf, &tree, key);
    } else if (!strncmp(command_token, DELETE_COMMAND, command_len)) {
      if (remaining_tokens != 1U) {
        report_error("delete command requires exactly one argument");
        break;
      }
      char *key = tokenizer_next_token(&tokenizer);
      // TODO(gliontos): We must take into consideration the number of updates
      delete_command(output_fd, bf, &tree, key);
    } else if (!strncmp(command_token, VOTE_COMMAND, command_len)) {
      if (remaining_tokens != 1U) {
        report_error("vote command requires exactly one argument");
        break;
      }
      char *key = tokenizer_next_token(&tokenizer);
      vote_command(output_fd, bf, &tree, key);
    } else if (!strncmp(command_token, LOAD_COMMAND, command_len)) {
      if (remaining_tokens != 1U) {
        report_error("load command requires exactly one argument");
        break;
      }
      char *filename = tokenizer_next_token(&tokenizer);
      load_command(output_fd, bf, &tree, filename);
    } else if (!strncmp(command_token, VOTED_COMMAND, command_len)) {
      if (remaining_tokens > 1U) {
        report_error("voted command requires at most one argument");
        break;
      }
      if (!remaining_tokens) {
        voted_command(output_fd, &tree);
      } else {
        char *postcode_token = tokenizer_next_token(&tokenizer);
        i64 postcode;
        if (!string_to_i64(postcode_token, &postcode)) {
          report_error("postcode %s is not a valid postcode", postcode_token);
        }
        // TODO(gliontos): This must be done through the hashtable
        voted_postcode_command(output_fd, &tree, postcode);
      }
    } else if (!strncmp(command_token, VOTED_PER_POSTCODE_COMMAND,
                        command_len)) {
      if (remaining_tokens != 0U) {
        report_error("votedperpc command requires no arguments");
        break;
      }
      // TODO(Gliontos): Handle votedperpc command
    } else if (!strncmp(command_token, EXIT_COMMAND, command_len)) {
      if (remaining_tokens != 0U) {
        report_error("exit command requires no arguments");
        break;
      }
      running = false;
      dprintf(output_fd,
              "COMMAND: " EXIT_COMMAND "\n"
              "OUTPUT:\n"
              "\t-exit program\n"
              "ERROR INDICATION:\n"
              "\t-none\n");
    } else {
      report_error("Unknown command \"%s\"", command_token);
      break;
    }
  }
}

void load_initial_data(const char *input_filename) {
  file *input_file = read_entire_file_into_memory(input_filename);
  tokenizer line_tok = {
      .stream = input_file->contents,
      .length = input_file->bytes_n,
      .index = 0U,
      .delimiter = '\n'
  };

  size_t nlines = tokenizer_remaining_tokens(&line_tok);
  bf = bloom_filter_create(nlines,
                           2,
                           (hash_function[]) {murmur_hash, simple_hash});
  while (tokenizer_has_next(&line_tok)) {
    char *line = tokenizer_next_token(&line_tok);
    voter *v = voter_create_from_string(line);
    rb_tree_insert(&tree, v);
    bloom_filter_add(bf, v->id);
  }
  free_file(input_file);
}

void read_line_from_stdin(void) {
  ssize_t bytes_read;
  byte character;
  do {
    bytes_read = read(STDIN_FILENO, &character, sizeof(byte));
    if (bytes_read > 0 && character != '\n') {
      dynamic_buffer_append(stdin_buffer, sizeof(byte), &character);
    }
  } while (bytes_read > 0 && character != '\n');
  stdin_buffer->contents[stdin_buffer->length] = 0;
}

int main(int argc, char *args[]) {
  program_options options = get_program_options(argc, args);
  load_initial_data(options.input_file);
  stdin_buffer = dynamic_buffer_create(20);
  output_fd = open(options.output_file,
                   O_WRONLY | O_CREAT,
                   S_IRWXU | S_IRGRP | S_IROTH);

  while (running) {
    read_line_from_stdin();
    process_command(stdin_buffer->contents);
    dynamic_buffer_clear(stdin_buffer);
  }

  close(output_fd);
  bloom_filter_delete(bf);
  // TODO(Gliontos): Cleanup the mess
  return EXIT_SUCCESS;
}