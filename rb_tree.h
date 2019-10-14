#ifndef EXERCISE_I__RB_TREE_H_
#define EXERCISE_I__RB_TREE_H_

#include <stdbool.h>
#include "voter.h"
#include "attributes.h"

typedef struct rb_node rb_node;

typedef struct rb_tree {
  rb_node *root;
} rb_tree;

rb_tree rb_tree_create(void);

bool rb_tree_insert(rb_tree *tree, voter *data) __NON_NULL(1, 2);

void rb_tree_delete(rb_tree *tree, size_t key_length,
    const char key[static key_length]) __NON_NULL(1, 3);

voter *rb_tree_search(rb_tree *tree, size_t key_length,
    const char key[static key_length]) __NON_NULL(1, 3);

#endif //EXERCISE_I__RB_TREE_H_
