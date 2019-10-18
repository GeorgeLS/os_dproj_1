#include "report.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "rb_tree.h"
#include "common.h"
#include "utils.h"
#include "tokenizer.h"
#include "voter.h"

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

void check_voters_in_file(int out_fd, bloom_filter *bf, rb_tree *tree,
                          const char *filename) {
  file *file = read_entire_file_into_memory(filename);
  if (!file) return;

  tokenizer line_tok = {
      .stream = file->contents,
      .length = file->bytes_n,
      .index = 0U,
      .delimiter = '\n'
  };

  dprintf(out_fd, "COMMAND: load %s\n", filename);
  while (tokenizer_has_next(&line_tok)) {
    char *key = tokenizer_next_token(&line_tok);
    voter_vote(out_fd, bf, tree, key);
  }
  free_file(file);
}