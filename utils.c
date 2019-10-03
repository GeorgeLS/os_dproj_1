#include <stdlib.h>
#include "utils.h"

bool string_to_i64(char *string, i64 *out) {
  char *valid;
  *out = strtoll(string, &valid, 10);
  return *valid;
}
