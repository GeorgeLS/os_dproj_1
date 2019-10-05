#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

bool string_to_i64(char *string, i64 *out) {
  char *valid;
  *out = strtoll(string, &valid, 10);
  return *valid;
}

void string_to_lowercase(char *string, size_t n) {
  for (size_t i = 0U; i != n; ++i) {
    string[i] = tolower(string[i]);
  }
}