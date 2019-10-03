#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "report.h"
#include "utils.h"

#define INPUT_FILE_OPTION "-i"
#define OUTPUT_FILE_OPTION "-o"
#define UPDATES_N_OPTION "-n"

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

int main(int argc, char *args[]) {
  program_options options = get_program_options(argc, args);
  return 0;
}