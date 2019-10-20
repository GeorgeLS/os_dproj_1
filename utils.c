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
  return *valid == '\0';
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
  char *contents = malloc(info.st_size);
  if (!contents) {
    close(fd);
    return NULL;
  }
  read(fd, contents, info.st_size);
  file *file = malloc(sizeof(file));
  file->contents = contents;
  file->bytes_n = info.st_size;
  close(fd);
  return file;
}

void free_file(file *file) {
  free(file->contents);
  free(file);
}

bool is_prime(i64 number) {
  if (number < 2) return false;
  if (number == 2) return true;
  if (number & 1) return false;
  if (!(number % 3)) return number == 3;
  i64 j;
  for (i64 i = 5; (j = i * i), j <= number && j > i; i += 6) {
    if (!(number % i)) return false;
    if (!(number % (i + 2))) return false;
  }
  return true;
}