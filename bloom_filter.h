#ifndef EXERCISE_I__BLOOM_FILTER_H_
#define EXERCISE_I__BLOOM_FILTER_H_

#include <stdbool.h>
#include "attributes.h"
#include "common.h"

typedef struct Bloom_Filter {
  u64 *set;
  size_t length;
  Hash_Function *hash_functions;
  size_t hash_functions_n;
} Bloom_Filter;

Bloom_Filter *bloom_filter_create(size_t nbits, size_t hash_functions_n,
                                  Hash_Function hash_functions[static hash_functions_n])
__NON_NULL(3);

void bloom_filter_free(Bloom_Filter *bf) __NON_NULL(1);

bool bloom_filter_contains(Bloom_Filter *filter, const char *restrict key)
__NON_NULL(1, 2);

void bloom_filter_add(Bloom_Filter *filter, const char *restrict key)
__NON_NULL(1, 2);

void bloom_filter_reset(Bloom_Filter *filter, size_t nbits) __NON_NULL(1);

u64 murmur_hash(const byte *restrict key, size_t len) __NON_NULL(1);

u64 djb_hash(const byte *restrict key, size_t len) __NON_NULL(1);

u64 simple_hash(const byte *restrict key, size_t len) __NON_NULL(1);

#endif //EXERCISE_I__BLOOM_FILTER_H_