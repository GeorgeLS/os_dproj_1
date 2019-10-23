#ifndef EXERCISE_I__HASH_TABLE_H_
#define EXERCISE_I__HASH_TABLE_H_

#include "attributes.h"
#include "generic_list.h"
#include "pointer_link.h"
#include "voter.h"

typedef struct bucket_node {
  u64 key;
  Generic_List voters_list;
  Generic_List chain;
} bucket_node;

typedef struct Hash_Table {
  Generic_List *table;
  size_t n_buckets;
  Hash_Function hash_function;
} Hash_Table;

Hash_Table *hash_table_create(size_t n_buckets, Hash_Function hash_function)
__NON_NULL(2);

void hash_table_free(Hash_Table *hash_table) __NON_NULL(1);

void hash_table_insert(Hash_Table *hash_table, Voter *voter_ptr)
__NON_NULL(1, 2);

void hash_table_remove(Hash_Table *hash_table, Voter *voter_ptr)
__NON_NULL(1, 2);

size_t hash_table_nvoters(Hash_Table *hash_table) __NON_NULL(1);

size_t hash_table_nvoters_postal_code(Hash_Table *hash_table, u64 postal_code)
__NON_NULL(1);

u64 hash_table_hf(const byte *restrict key, size_t nbytes) __NON_NULL(1);

#endif //EXERCISE_I__HASH_TABLE_H_
