#ifndef EXERCISE_I__HASH_TABLE_H_
#define EXERCISE_I__HASH_TABLE_H_

#include "attributes.h"
#include "generic_list.h"
#include "pointer_link.h"
#include "voter.h"

typedef struct bucket_node {
  u64 key;
  generic_list voters_list;
  generic_list chain;
} bucket_node;

typedef struct Hash_Table {
  generic_list *table;
  size_t n_buckets;
  size_t entries_per_bucket_node;
  hash_function hash_function;
} Hash_Table;

Hash_Table *hash_table_create(size_t n_buckets, hash_function hash_function)
__NON_NULL(2);

void hash_table_free(Hash_Table *hash_table) __NON_NULL(1);

void hash_table_insert(Hash_Table *hash_table, voter *voter_ptr)
__NON_NULL(1, 2);

void hash_table_remove(Hash_Table *hash_table, voter *voter_ptr)
__NON_NULL(1, 2);

bool hash_table_contains_key(Hash_Table *hash_table, u64 key) __NON_NULL(1);

size_t hash_table_nvoters(Hash_Table *hash_table) __NON_NULL(1);

size_t hash_table_nvoters_postal_code(Hash_Table *hash_table, u64 postal_code)
__NON_NULL(1);

#endif //EXERCISE_I__HASH_TABLE_H_
