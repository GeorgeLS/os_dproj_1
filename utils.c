#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <zconf.h>
#include "utils.h"

bool string_to_i64(char *string, i64 *out) {
  char *valid;
  *out = strtoll(string, &valid, 10);
  return *valid;
}

void string_to_lowercase(char *string) {
  for (size_t i = 0U; i != strlen(string); ++i) {
    string[i] = tolower(string[i]);
  }
}

bool string_less(const char *restrict lhs, const char *restrict rhs) {
  return strcmp(lhs, rhs) < 0;
}

bool string_greater(const char *restrict lhs, const char *restrict rhs) {
  return strcmp(lhs, rhs) > 0;
}

bool string_equals(const char *restrict lhs, const char *restrict rhs) {
  return !strcmp(lhs, rhs);
}

file *read_entire_file_into_memory(const char *filename) {
  int fd = open(filename, O_RDONLY);
  if (fd == -1) return NULL;
  struct stat info;
  fstat(fd, &info);
  char *contents = malloc(info.st_size * sizeof(char));
  if (!contents) {
    close(fd);
    return NULL;
  }
  file *file = malloc(sizeof(file));
  file->contents = contents;
  file->bytes_n = info.st_size;
  return file;
}

void free_file(file *file) {
  free(file->contents);
  free(file);
}