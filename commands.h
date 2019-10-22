#ifndef EXERCISE_I__COMMANDS_H_
#define EXERCISE_I__COMMANDS_H_

#include "bloom_filter.h"
#include "rb_tree.h"
#include "hash_table.h"

void lbf_command(int out_fd, bloom_filter *bf, const char *restrict key)
__NON_NULL(2, 3);

void lrb_command(int out_fd, rb_tree *tree, const char *restrict key)
__NON_NULL(2, 3);

void ins_command(int out_fd,
                 Hash_Table *hash_table,
                 bloom_filter *bf,
                 rb_tree *tree,
                 char *voter_string) __NON_NULL(2, 3, 4, 5);

void find_command(int out_fd, bloom_filter *bf, rb_tree *tree,
                  const char *restrict key) __NON_NULL(2, 3, 4);

void delete_command(int out_fd, bloom_filter *bf, rb_tree *tree,
                    const char *restrict key) __NON_NULL(2, 3, 4);

void vote_command(int out_fd, bloom_filter *bf, rb_tree *tree,
                  const char *restrict key) __NON_NULL(2, 3, 4);

void voted_command(int out_fd, Hash_Table *hash_table) __NON_NULL(2);

void voted_postcode_command(int out_fd, Hash_Table *hash_table, i64 postal_code)
__NON_NULL(2);

void votedperpc_command(int out_fd, Hash_Table *hash_table) __NON_NULL(2);

void load_command(int out_fd, bloom_filter *bf, rb_tree *tree,
                  const char *filename) __NON_NULL(2, 3, 4);

void exit_command(int out_fd);

#endif //EXERCISE_I__COMMANDS_H_
