#ifndef EXERCISE_I__BLOOM_FILTER_H_
#define EXERCISE_I__BLOOM_FILTER_H_

#include <stdbool.h>
#include "attributes.h"
#include "common.h"

typedef struct bloom_filter bloom_filter;

// Some C99 magic <3
bloom_filter *bloom_filter_create(size_t nbits, size_t hash_functions_n,
                                  hash_function hash_functions[static hash_functions_n])
__NON_NULL(3);

void bloom_filter_free(bloom_filter *bf) __NON_NULL(1);

bool bloom_filter_contains(bloom_filter *filter, const char *restrict key)
__NON_NULL(1, 2);

void bloom_filter_add(bloom_filter *filter, const char *restrict key)
__NON_NULL(1, 2);

u64 murmur_hash(const byte *restrict key, size_t len) __NON_NULL(1);

u64 simple_hash(const byte *restrict key, size_t len) __NON_NULL(1);

#endif //EXERCISE_I__BLOOM_FILTER_H_