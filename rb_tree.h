#ifndef EXERCISE_I__RB_TREE_H_
#define EXERCISE_I__RB_TREE_H_

#include <stdbool.h>
#include "attributes.h"
#include "common.h"

typedef struct rb_node rb_node;

struct voter;

typedef struct rb_tree {
  rb_node *root;
} rb_tree;

rb_tree rb_tree_create(void);

bool rb_tree_insert(rb_tree *tree, struct voter *data) __NON_NULL(1, 2);

bool rb_tree_delete(rb_tree *tree, const char *restrict key) __NON_NULL(1, 2);

struct voter *rb_tree_search(rb_tree *tree, const char *restrict key)
__NON_NULL(1, 2);

size_t rb_tree_nvoters(rb_tree *tree, i64 postal_code) __NON_NULL(1);

#endif //EXERCISE_I__RB_TREE_H_
