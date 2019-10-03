#include <memory.h>
#include <stdlib.h>
#include "bloom_filter.h"
#include "common.h"
#include "attributes.h"

typedef struct bloom_filter {
  byte *set;
  size_t length;
  hash_function *hash_functions;
  size_t hash_functions_n;
} bloom_filter;


internal __ALWAYS_INLINE size_t
byte_pos(size_t bit_number) {
  return (bit_number - 1U) >> 6U;
}

internal __ALWAYS_INLINE size_t
bit_offset(size_t bit_number) {
  return (bit_number - 1U) & 63U;
}

internal __ALWAYS_INLINE byte
get_bit(const byte *restrict set, size_t bit_number) {
  return (set[byte_pos(bit_number)] >> bit_offset(bit_number)) & 1U;
}

internal __ALWAYS_INLINE void
set_bit(byte *restrict set, size_t bit_number) {
  set[byte_pos(bit_number)] |= (1U << bit_offset(bit_number));
}

bloom_filter bloom_filter_create(size_t hash_functions_n,
    hash_function hash_functions[static hash_functions_n]) {
  return (bloom_filter) {
    .set = NULL,
    .length = 0U,
    hash_functions = hash_functions,
    hash_functions_n = hash_functions_n,
  };
}

bool bloom_filter_reallocate(bloom_filter *filter, size_t new_length) {
  if (__glibc_unlikely(filter->length > new_length)) return true;
  byte *new_set = malloc(new_length);
  if (!new_set) return false;
  byte *old_set = filter->set;
  memcpy(new_set, old_set, filter->length);
  filter->set = new_set;
  filter->length = new_length;
  free(old_set);
  return true;
}

bool bloom_filter_contains(bloom_filter *filter, size_t bytes_n,
    byte data[static bytes_n]) {
  byte *set = filter->set;
  for (size_t i = 0U; i != filter->hash_functions_n; ++i) {
    size_t bit_number = filter->hash_functions[i](data, bytes_n) %
        filter->length;

    byte bit = get_bit(set, bit_number);
    if (!bit) return false;
  }
  return true;
}

void bloom_filter_add(bloom_filter *filter, size_t bytes_n,
    byte data[static bytes_n]) {
  byte* set = filter->set;
  for (size_t i = 0U; i != filter->hash_functions_n; ++i) {
    size_t bit_number = filter->hash_functions[i](data, bytes_n) %
        filter->length;

    set_bit(set, bit_number);
  }
}
