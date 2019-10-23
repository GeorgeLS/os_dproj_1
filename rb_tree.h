#ifndef EXERCISE_I__RB_TREE_H_
#define EXERCISE_I__RB_TREE_H_

#include <stdbool.h>
#include "attributes.h"
#include "common.h"
#include "bloom_filter.h"

struct RB_Node;
struct Voter;

typedef struct RB_Tree {
  struct RB_Node *root;
  size_t nelements;
} RB_Tree;

bool rb_tree_insert(RB_Tree *tree, struct Voter *data) __NON_NULL(1, 2);

struct Voter *rb_tree_remove(RB_Tree *tree, const char *restrict key)
__NON_NULL(1, 2);

struct Voter *rb_tree_search(RB_Tree *tree, const char *restrict key)
__NON_NULL(1, 2);

void rb_tree_free(RB_Tree *tree) __NON_NULL(1);

void restructure_bloom_filter(Bloom_Filter *bf, RB_Tree *tree) __NON_NULL(1, 2);

#endif //EXERCISE_I__RB_TREE_H_
