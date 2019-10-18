#ifndef EXERCISE_I__UTILS_H_
#define EXERCISE_I__UTILS_H_

#include <stdbool.h>
#include <stdlib.h>
#include "attributes.h"
#include "common.h"

typedef struct file {
  char *contents;
  size_t bytes_n;
} file;

bool string_to_i64(char *string, i64 *out) __NON_NULL(1, 2);

void string_to_lowercase(char *string) __NON_NULL(1);

bool string_less(const char *restrict lhs, const char *restrict rhs)
__NON_NULL(1, 2);

bool string_greater(const char *restrict lhs, const char *restrict rhs)
__NON_NULL(1, 2);

bool string_equals(const char *restrict lhs, const char *restrict rhs)
__NON_NULL(1, 2);

file *read_entire_file_into_memory(const char *filename) __NON_NULL(1);

void free_file(file *file) __NON_NULL(1);

#endif //EXERCISE_I__UTILS_H_