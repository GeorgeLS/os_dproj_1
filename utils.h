#ifndef EXERCISE_I__UTILS_H_
#define EXERCISE_I__UTILS_H_

#include <stdbool.h>
#include <stdlib.h>
#include "attributes.h"
#include "common.h"

bool string_to_i64(char *string, i64 *out) __NON_NULL(1, 2);
void string_to_lowercase(char *string, size_t n) __NON_NULL(1);

#endif //EXERCISE_I__UTILS_H_