#include "report.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "rb_tree.h"
#include "common.h"
#include "utils.h"
#include "tokenizer.h"

#define ERROR_TAG "ERROR"

internal void report(const char *tag, const char *fmt, va_list args) {
  if (__glibc_likely(tag != NULL)) {
    fprintf(stderr, "[%s]: ", tag);
  }
  vfprintf(stderr, fmt, args);
  fputc('\n', stderr);
  va_end(args);
}

void fatal(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  report(NULL, fmt, args);
  exit(EXIT_FAILURE);
}

void report_error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  report(ERROR_TAG, fmt, args);
}

void check_voters_in_file(int out_fd,
                          rb_tree *tree,
                          size_t filename_length,
                          char filename[static filename_length]) {
  char aux = filename[filename_length - 1U];
  filename[filename_length - 1U] = '\0';
  file *file = read_entire_file_into_memory(filename);
  filename[filename_length - 1U] = aux;
  if (!file) return;

  tokenizer line_tok = {
      .stream = file->contents,
      .length = file->bytes_n,
      .index = 0U,
      .delimiter = '\n'
  };

  while (tokenizer_has_next(&line_tok)) {
    token key = tokenizer_next_token(&line_tok);
    voter *voter = rb_tree_search(tree, key.length, key.string);
    if (voter) {
      dprintf(out_fd,
              "COMMAND: load %2$*1$s\n"
              "OUTPUT:\n"
              "\tREC-WITH %3$*4$s %s\n"
              "ERROR INDICATION:\n"
              "\t-none\n",
              filename_length, filename,
              key.length, key.string,
              voter->has_voted ? "ALREADY-VOTED" : "SET-VOTED");
      if (!voter->has_voted) {
        voter->has_voted = true;
      }
    } else {
      dprintf(out_fd,
              "COMMAND: load %2$*1$s\n"
              "OUTPUT:\n"
              "\tERROR\n"
              "ERROR INDICATION:\n"
              "\tREC-WITH %3$*4$s NOT-IN-STRUCTS\n",
              filename_length, filename,
              key.length, key.string);
    }
  }
  free_file(file);
}