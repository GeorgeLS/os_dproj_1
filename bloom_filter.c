#include <memory.h>
#include <stdlib.h>
#include <immintrin.h>
#include "bloom_filter.h"
#include "common.h"
#include "attributes.h"
#include "utils.h"

typedef struct bloom_filter {
  u64 *set;
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

internal __ALWAYS_INLINE byte get_bit(const u64 *restrict set,
                                      size_t bit_number) {
  return (set[byte_pos(bit_number)] >> bit_offset(bit_number)) & 1U;
}

internal __ALWAYS_INLINE void set_bit(u64 *restrict set, size_t bit_number) {
  set[byte_pos(bit_number)] |= (1U << bit_offset(bit_number));
}

internal __ALWAYS_INLINE size_t compute_set_size(size_t nbits) {
  nbits = nbits + nbits + nbits;
  while (!is_prime(nbits)) ++nbits;
  return nbits;
}

bloom_filter *bloom_filter_create(size_t nbits, size_t hash_functions_n,
                                  hash_function hash_functions[static hash_functions_n]) {
  bloom_filter *filter = malloc(sizeof(bloom_filter));
  filter->length = compute_set_size(nbits);
  size_t bytes = (byte_pos(filter->length) + 1U) * sizeof(u64);
  filter->set = malloc(bytes);
  memset(filter->set, 0, bytes);
  filter->hash_functions = hash_functions;
  filter->hash_functions_n = hash_functions_n;
  return filter;
}

void bloom_filter_free(bloom_filter *bf) {
  free(bf->set);
  free(bf);
}

bool bloom_filter_contains(bloom_filter *filter, const char *restrict key) {
  u64 *set = filter->set;
  hash_function *hash_functions = filter->hash_functions;
  for (size_t i = 0U; i != filter->hash_functions_n; ++i) {
    hash_function hf = hash_functions[i];
    size_t bit_number = (hf(key, strlen(key)) % filter->length) + 1U;
    byte bit = get_bit(set, bit_number);
    if (!bit) return false;
  }
  return true;
}

void bloom_filter_add(bloom_filter *filter, const char *restrict key) {
  u64 *set = filter->set;
  for (size_t i = 0U; i != filter->hash_functions_n; ++i) {
    size_t bit_number = (filter->hash_functions[i](key, strlen(key)) %
        filter->length) + 1U;

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
    hash += *(i64 *) key;
  }

  for (size_t i = 0U; i < len; ++i, ++key) {
    hash += *key;
  }

  return hash;
}