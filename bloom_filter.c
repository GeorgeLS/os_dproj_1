#include <memory.h>
#include <stdlib.h>
#include <immintrin.h>
#include "bloom_filter.h"
#include "common.h"
#include "attributes.h"


typedef struct bloom_filter {
  byte *set;
  size_t length;
  hash_function *hash_functions;
  size_t hash_functions_n;
} bloom_filter;

internal __ALWAYS_INLINE size_t byte_pos(size_t bit_number) {
  return (bit_number - 1U) >> 6U;
}

internal __ALWAYS_INLINE size_t bit_offset(size_t bit_number) {
  return (bit_number - 1U) & 63U;
}

internal __ALWAYS_INLINE byte get_bit(const byte *restrict set,
                                      size_t bit_number) {
  return (set[byte_pos(bit_number)] >> bit_offset(bit_number)) & 1U;
}

internal __ALWAYS_INLINE void set_bit(byte *restrict set, size_t bit_number) {
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
  byte *set = filter->set;
  for (size_t i = 0U; i != filter->hash_functions_n; ++i) {
    size_t bit_number = filter->hash_functions[i](data, bytes_n) %
        filter->length;

    set_bit(set, bit_number);
  }
}

u64 murmur_hash(const byte *restrict key, size_t len) {
  local const u64 seed = 104729U;
  const u64 m = 0xC6A4A7935BD1E995;
  const int r = 47;

  u64 hash = seed ^(len * m);

  const u64 *data = (const uint64_t *) key;
  const u64 *end = data + (len / 8);

  while (data != end) {
    u64 k = *data++;

    k *= m;
    k ^= k >> r;
    k *= m;

    hash ^= k;
    hash *= m;
  }

  const byte *data2 = (const byte *) data;

  switch (len & 7) {
    case 7: hash ^= ((u64 *) data2)[6] << 48;
    case 6: hash ^= ((u64 *) data2)[5] << 40;
    case 5: hash ^= ((u64 *) data2)[4] << 32;
    case 4: hash ^= ((u64 *) data2)[3] << 24;
    case 3: hash ^= ((u64 *) data2)[2] << 16;
    case 2: hash ^= ((u64 *) data2)[1] << 8;
    case 1: hash ^= ((u64 *) data2)[0];
      hash *= m;
  };

  hash ^= hash >> r;
  hash *= m;
  hash ^= hash >> r;

  return hash;
}

u64 simple_hash(const byte *restrict key, size_t len) {
  u64 hash = 0U;
  for (size_t i = 0U; i < len >> 6U; i += 64U, key += 64U, len -= 64U) {
    __m256i v = _mm256_add_epi64(
        _mm256_load_si256(key),
        _mm256_load_si256(key + 32U));
    hash += _mm256_extract_epi64(v, 0U);
    hash += _mm256_extract_epi64(v, 1U);
    hash += _mm256_extract_epi64(v, 2U);
    hash += _mm256_extract_epi64(v, 3U);
  }

  for (size_t i = 0U; i < len >> 5U; i += 32U, key += 32U, len -= 32U) {
    __m128i v = _mm_add_epi64(
        _mm_load_si128(key),
        _mm_load_si128(key + 16U));
    hash += _mm_extract_epi64(v, 0U);
    hash += _mm_extract_epi64(v, 1U);
  }

  for (size_t i = 0U; i < len >> 3U; i += 8U, key += 8U, len -= 8U) {
    hash += *(i64 *)key;
  }

  for (size_t i = 0U; i < len; ++i) {
    hash += *key;
  }

  return hash;
}
