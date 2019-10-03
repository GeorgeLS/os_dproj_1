#ifndef EXERCISE_I__BLOOM_FILTER_H_
#define EXERCISE_I__BLOOM_FILTER_H_

#include <stdbool.h>
#include "attributes.h"
#include "common.h"

typedef struct bloom_filter bloom_filter;

// Some C99 magic <3
bloom_filter bloom_filter_create(size_t hash_functions_n,
  hash_function hash_functions[static hash_functions_n]) __NON_NULL(2);

bool bloom_filter_reallocate(bloom_filter *filter, size_t new_length)
__NON_NULL(1);

bool bloom_filter_contains(bloom_filter *filter, size_t bytes_n,
    byte data[static bytes_n]) __NON_NULL(1, 3);

void bloom_filter_add(bloom_filter *filter, size_t bytes_n,
    byte data[static bytes_n]) __NON_NULL(1, 3);

#endif //EXERCISE_I__BLOOM_FILTER_H_