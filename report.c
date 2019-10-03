#include "report.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

#define ERROR_TAG "[ERROR]"

internal void report(const char *tag, const char *fmt, va_list args) {
  if (__glibc_likely(tag != NULL)) {
    fprintf(stderr, "%s: ", tag);
  }
  vfprintf(stderr, fmt, args);
  fputc('\n', stderr);
  va_end(args);
}

void fatal(const char *fmt, ...) {
  va_list  args;
  va_start(args, fmt);
  report(NULL, fmt, args);
  exit(EXIT_FAILURE);
}

void report_error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  report(ERROR_TAG, fmt, args);
}