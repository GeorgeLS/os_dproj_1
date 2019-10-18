#ifndef EXERCISE_I__COMMANDS_H_
#define EXERCISE_I__COMMANDS_H_

#include "bloom_filter.h"
#include "rb_tree.h"

void lbf_command(int out_fd, bloom_filter *bf, const char *restrict key)
__NON_NULL(2, 3);

void lrb_command(int out_fd, rb_tree *tree, const char *restrict key)
__NON_NULL(2, 3);

void find_command(int out_fd, bloom_filter *bf, rb_tree *tree,
                  const char *restrict key) __NON_NULL(2, 3, 4);

void delete_command(int out_fd, bloom_filter *bf, rb_tree *tree,
                    const char *restrict key) __NON_NULL(2, 3, 4);

void vote_command(int out_fd, bloom_filter *bf, rb_tree *tree,
                  const char *restrict key) __NON_NULL(2, 3, 4);

void voted_command(int out_fd, rb_tree *tree) __NON_NULL(2);

void voted_postcode_command(int out_fd, rb_tree *tree, i64 postal_code)
__NON_NULL(2);

#endif //EXERCISE_I__COMMANDS_H_
