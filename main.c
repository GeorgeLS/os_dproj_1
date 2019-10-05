#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "common.h"
#include "report.h"
#include "utils.h"
#include "dynamic_buffer.h"
#include "tokenizer.h"
#include "bloom_filter.h"

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

global bool running = true;
global bloom_filter *bf;
dynamic_buffer *stdin_buffer;

typedef struct program_options {
  char *input_file;
  char *output_file;
  i64 updates_n;
} program_options;

global program_options default_options = {
    .input_file = NULL,
    .output_file = "election.log",
    .updates_n = 5U,
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

void process_command(const char *restrict command, size_t command_length) {
  tokenizer tokenizer = {
      .stream = command,
      .length = command_length,
      .index = 0U,
      .delimiter = ' '
  };

  while (tokenizer_has_next(&tokenizer)) {
    token command_token = tokenizer_next_token(&tokenizer);
    string_to_lowercase(command_token.string, command_token.length);
    size_t remaining_tokens = tokenizer_remaining_tokens(&tokenizer);
    if (!strncmp(command_token.string, LBF_COMMAND, command_token.length)) {
      if (remaining_tokens != 1U) {
        report_error("lbf command requires exactly one argument");
        break;
      }
      token key = tokenizer_next_token(&tokenizer);
      if (bloom_filter_contains(bf, key.length, key.string)) {
        printf("Maybe\n");
      } else {
        printf("No\n");
      }
    } else if (!strncmp(command_token.string, LRB_COMMAND, command_token.length)) {
      if (remaining_tokens != 1U) {
        report_error("lrb command requires exactly one argument");
        break;
      }
      // TODO(Gliontos): Handle lrb command
    } else if (!strncmp(command_token.string, INS_COMMAND, command_token.length)) {
      if (remaining_tokens != 1) {
        report_error("ins record requires exactly one argument");
        break;
        }
      // TODO(Gliontos): Handle ins command
    } else if (!strncmp(command_token.string, FIND_COMMAND, command_token.length)) {
      if (remaining_tokens != 1U) {
        report_error("find command requires exactly one argument");
        break;
      }
      // TODO(Gliontos): Handle find command
    } else if (!strncmp(command_token.string, DELETE_COMMAND, command_token.length)) {
      if (remaining_tokens != 1U) {
        report_error("delete command requires exactly one argument");
        break;
      }
      // TODO(Gliontos): Handle delete command
    } else if (!strncmp(command_token.string, VOTE_COMMAND, command_token.length)) {
      if (remaining_tokens != 1U) {
        report_error("vote command requires exactly one argument");
        break;
      }
      // TODO(Gliontos): Handle vote command
    } else if (!strncmp(command_token.string, LOAD_COMMAND, command_token.length)) {
      if (remaining_tokens != 1U) {
        report_error("load command requires exactly one argument");
        break;
      }
      // TODO(Gliontos): Handle load command
    } else if (!strncmp(command_token.string, VOTED_COMMAND, command_token.length)) {
      if (remaining_tokens > 1U) {
        report_error("voted command requires at most one argument");
        break;
      }
      // TODO(Gliontos): Handle voted command
    } else if (!strncmp(command_token.string, VOTED_PER_POSTCODE_COMMAND, command_token.length)) {
      if (remaining_tokens != 0U) {
        report_error("votedperpc command requires no arguments");
        break;
      }
      // TODO(Gliontos): Handle votedperpc command
    } else if (!strncmp(command_token.string, EXIT_COMMAND, command_token.length)) {
      if (remaining_tokens != 0U) {
        report_error("exit command requires no arguments");
        break;
      }
      running = false;
    } else {
      report_error("Unknown command \"%.*s\"", command_token.length, command_token.string);
      break;
    }
  }
}

int main(int argc, char *args[]) {
  tokenizer tokenizer = {.delimiter = ' '};
  // start off with 20 characters capacity
  stdin_buffer = dynamic_buffer_create(20);
  program_options options = get_program_options(argc, args);
  while (running) {
    ssize_t bytes_read;
    byte character;
    do {
      bytes_read = read(STDIN_FILENO, &character, sizeof(byte));
      if (bytes_read > 0 && character != '\n') {
        dynamic_buffer_append(stdin_buffer, sizeof(byte), &character);
      }
    } while (bytes_read > 0 && character != '\n');
    if (stdin_buffer->length) {
      printf("Read %.*s\n", stdin_buffer->length, stdin_buffer->contents);
      tokenizer.stream = stdin_buffer->contents;
      tokenizer.index = 0U;
      tokenizer.length = stdin_buffer->length;
      while (tokenizer_has_next(&tokenizer)) {
        token token = tokenizer_next_token(&tokenizer);
        printf("Token: %.*s\n", token.length, token.string);
      }
      process_command(stdin_buffer->contents, stdin_buffer->length);
      dynamic_buffer_clear(stdin_buffer);
    }
  }
  // TODO(Gliontos): Cleanup the mess
  return 0;
}