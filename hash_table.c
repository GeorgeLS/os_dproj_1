#include "hash_table.h"
#include <stdlib.h>

internal bucket_node *create_new_bucket_node(u64 key) {
  bucket_node *node = malloc(sizeof(bucket_node));
  node->key = key;
  INIT_LIST_HEAD(&node->voters_list);
  INIT_LIST_HEAD(&node->chain);
  return node;
}

u64 hash_table_hf(const byte *restrict key, size_t nbytes) {
  return *(u64 *)key;
}

Hash_Table *hash_table_create(size_t n_buckets, Hash_Function hash_function) {
  n_buckets = n_buckets / 0.7;
  Hash_Table *hash_table = malloc(sizeof(Hash_Table));
  hash_table->n_buckets = n_buckets;
  hash_table->hash_function = hash_function;
  Generic_List *table = malloc(n_buckets * sizeof(Generic_List));
  for (size_t i = 0U; i != n_buckets; ++i) {
    INIT_LIST_HEAD(&table[i]);
  }
  hash_table->table = table;
  return hash_table;
}

void hash_table_free(Hash_Table *hash_table) {
  for (size_t i = 0U; i != hash_table->n_buckets; ++i) {
    Generic_List *list = &hash_table->table[i];
    bucket_node *outer_it;
    bucket_node *outer_tmp;
    list_for_each_entry_safe(list, outer_it, outer_tmp, chain) {
      list_free(&outer_it->voters_list, Pointer_Link, node, free);
      list_delete_entry(&outer_it->chain);
      free(outer_it);
    }
  }
  free(hash_table->table);
  free(hash_table);
}

internal bucket_node *hash_table_find(Hash_Table *hash_table, u64 key) {
  size_t bucket =
      hash_table->hash_function(&key, sizeof(u64)) % hash_table->n_buckets;
  Generic_List *bucket_list = &hash_table->table[bucket];
  if (list_is_empty(bucket_list)) return NULL;
  bucket_node *node = NULL;
  list_for_each_entry(bucket_list, node, chain) {
    if (node->key == key) return node;
  }
  return NULL;
}

void hash_table_insert(Hash_Table *hash_table, Voter *voter_ptr) {
  size_t key = voter_ptr->postal_code;
  bucket_node *node = hash_table_find(hash_table, key);
  if (!node) {
    node = create_new_bucket_node(key);
    size_t bucket = hash_table->hash_function(&key, sizeof(u64)) %
        hash_table->n_buckets;
    list_add_tail(&hash_table->table[bucket], &node->chain);
  }
  Pointer_Link *link = pointer_link_create(voter_ptr);
  list_add_tail(&node->voters_list, &link->node);
}

void hash_table_remove(Hash_Table *hash_table, Voter *voter_ptr) {
  size_t key = voter_ptr->postal_code;
  bucket_node *bnode = hash_table_find(hash_table, key);
  if (!bnode) return;
  Pointer_Link *voter_link = NULL;
  list_for_each_entry(&bnode->voters_list, voter_link, node) {
    if (voter_link->ptr == voter_ptr) {
      list_delete_entry(&voter_link->node);
      free(voter_link);
      break;
    }
  }
}

size_t hash_table_nvoters(Hash_Table *hash_table) {
  size_t nvoters = 0U;
  for (size_t i = 0U; i != hash_table->n_buckets; ++i) {
    Generic_List *list = &hash_table->table[i];
    if (list_is_empty(list)) continue;
    bucket_node *bnode = NULL;
    list_for_each_entry(list, bnode, chain) {
      Pointer_Link *voter_link = NULL;
      list_for_each_entry(&bnode->voters_list, voter_link, node) {
        nvoters += ((Voter *) voter_link->ptr)->has_voted ? 1U : 0U;
      }
    }
  }
  return nvoters;
}

size_t hash_table_nvoters_postal_code(Hash_Table *hash_table, u64 postal_code) {
  bucket_node *bnode = hash_table_find(hash_table, postal_code);
  if (!bnode) return 0U;
  size_t nvoters = 0U;
  Pointer_Link *voter_link = NULL;
  list_for_each_entry(&bnode->voters_list, voter_link, node) {
    nvoters += ((Voter *) voter_link->ptr)->has_voted;
  }
  return nvoters;
}
