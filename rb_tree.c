#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "voter.h"
#include "generic_list.h"
#include "pointer_link.h"

typedef enum color {
  BLACK, RED
} color;

typedef struct RB_Node {
  struct RB_Node *left;
  struct RB_Node *right;
  struct RB_Node *parent;
  Voter *data;
  color color : 1;
} RB_Node;

internal __ALWAYS_INLINE RB_Node *rb_node_create(Voter *data) {
  RB_Node *node = malloc(sizeof(RB_Node));
  if (__glibc_unlikely(!node)) return NULL;
  node->left = node->right = node->parent = NULL;
  node->color = RED;
  node->data = data;
  return node;
}

internal __ALWAYS_INLINE RB_Node *grandparent(RB_Node *node) {
  return node->parent ? node->parent->parent : NULL;
}

internal __ALWAYS_INLINE RB_Node *sibling(RB_Node *node) {
  RB_Node *parent = node->parent;
  if (!parent) return NULL;
  return node == parent->left ? parent->right : parent->left;
}

internal __ALWAYS_INLINE RB_Node *uncle(RB_Node *node) {
  RB_Node *parent = node->parent;
  if (!parent || !parent->parent) return NULL;
  return sibling(parent);
}

internal void rotate_left(RB_Node *node) {
  RB_Node *node_parent = node->parent;
  RB_Node *new_node = node->right;
  node->right = new_node->left;
  new_node->left = node;
  node->parent = new_node;
  if (node->right) {
    node->right->parent = node;
  }

  if (node_parent) {
    if (node == node_parent->left) {
      node_parent->left = new_node;
    } else if (node == node_parent->right) {
      node_parent->right = new_node;
    }
  }
  new_node->parent = node_parent;
}

internal void rotate_right(RB_Node *node) {
  RB_Node *node_parent = node->parent;
  RB_Node *new_node = node->left;
  node->left = new_node->right;
  new_node->right = node;
  node->parent = new_node;
  if (node->left) {
    node->left->parent = node;
  }

  if (node_parent) {
    if (node == node_parent->left) {
      node_parent->left = new_node;
    } else if (node == node_parent->right) {
      node_parent->right = new_node;
    }
  }
  new_node->parent = node_parent;
}

internal RB_Node *rb_tree_insert_wrapper(RB_Tree *tree, Voter *data) {
  RB_Node *node = rb_node_create(data);
  if (__glibc_unlikely(!node)) return NULL;
  if (tree->root != NULL) {
    RB_Node *current = tree->root;
    RB_Node *parent = tree->root;
    while (current != NULL) {
      parent = current;
      if (string_less(data->id, current->data->id)) {
        current = current->left;
      } else if (string_greater(data->id, current->data->id)) {
        current = current->right;
      } else {
        free(node);
        return NULL;
      }
    }
    node->parent = parent;
    if (string_less(data->id, parent->data->id)) {
      parent->left = node;
    } else {
      parent->right = node;
    }
  }
  return node;
}

internal void rb_tree_reorder(RB_Node *node) {
  if (node->parent == NULL) {
    node->color = BLACK;
  } else if (node->parent->color == BLACK) {
    return;
  } else if (uncle(node) && uncle(node)->color == RED) {
    node->parent->color = uncle(node)->color = BLACK;
    grandparent(node)->color = RED;
    rb_tree_reorder(grandparent(node));
  } else {
    RB_Node *parent = node->parent;
    RB_Node *gparent = grandparent(node);
    if (node == parent->right && parent == gparent->left) {
      rotate_left(parent);
      node = node->left;
    } else if (node == parent->left && parent == gparent->right) {
      rotate_right(parent);
      node = node->right;
    }

    parent = node->parent;
    gparent = grandparent(node);

    if (node == parent->left) {
      rotate_right(gparent);
    } else {
      rotate_left(gparent);
    }

    parent->color = BLACK;
    gparent->color = RED;
  }
}

internal RB_Node *__rb_tree_search(RB_Tree *tree, const char *restrict key) {
  RB_Node *current = tree->root;
  while (current != NULL) {
    int order = strncmp(key, current->data->id, strlen(key));
    if (order < 0) {
      current = current->left;
    } else if (order > 0) {
      current = current->right;
    } else {
      return current;
    }
  }
  return NULL;
}

bool rb_tree_insert(RB_Tree *tree, Voter *data) {
  RB_Node *new_node = rb_tree_insert_wrapper(tree, data);
  if (!new_node) return false;
  rb_tree_reorder(new_node);
  RB_Node *root = new_node;
  while (root->parent != NULL) {
    root = root->parent;
  }
  tree->root = root;
  ++tree->nelements;
  return true;
}

internal RB_Node *get_replacement_node(RB_Node *to_replace) {
  if (to_replace->left && to_replace->right) {
    to_replace = to_replace->right;
    while (to_replace->left) {
      to_replace = to_replace->left;
    }
    return to_replace;
  }
  if (!to_replace->left && !to_replace->right) return NULL;
  return to_replace->left ? to_replace->left : to_replace->right;
}

internal void rb_tree_delete_case_1(RB_Node *);
internal void rb_tree_delete_case_2(RB_Node *);
internal void rb_tree_delete_case_3(RB_Node *);
internal void rb_tree_delete_case_4(RB_Node *);
internal void rb_tree_delete_case_5(RB_Node *);
internal void rb_tree_delete_case_6(RB_Node *);

internal void rb_tree_delete_case_1(RB_Node *node) {
  if (node->parent) {
    rb_tree_delete_case_2(node);
  }
}

internal void rb_tree_delete_case_2(RB_Node *node) {
  RB_Node *sibling_node = sibling(node);
  if (sibling_node->color == RED) {
    node->parent->color = RED;
    sibling_node->color = BLACK;
    if (node == node->parent->left) {
      rotate_left(node->parent);
    } else {
      rotate_right(node->parent);
    }
  }
  rb_tree_delete_case_3(node);
}

internal void rb_tree_delete_case_3(RB_Node *node) {
  RB_Node *sibling_node = sibling(node);
  if (node->parent->color == BLACK && sibling_node->color == BLACK &&
      sibling_node->left->color == BLACK
      && sibling_node->right->color == BLACK) {
    sibling_node->color = RED;
    rb_tree_delete_case_1(node->parent);
  } else {
    rb_tree_delete_case_4(node);
  }
}

internal void rb_tree_delete_case_4(RB_Node *node) {
  RB_Node *sibling_node = sibling(node);
  if (node->parent->color == RED && sibling_node->color == BLACK &&
      sibling_node->left->color == BLACK
      && sibling_node->right->color == BLACK) {
    sibling_node->color = RED;
    node->parent->color = BLACK;
  } else {
    rb_tree_delete_case_5(node);
  }
}

internal void rb_tree_delete_case_5(RB_Node *node) {
  RB_Node *sibling_node = sibling(node);
  if (sibling_node->color == BLACK) {
    if (node == node->parent->left && sibling_node->right->color == BLACK &&
        sibling_node->left->color == RED) {
      sibling_node->color = RED;
      sibling_node->left->color = BLACK;
      rotate_right(sibling_node);
    } else if (node == node->parent->right && sibling_node->right->color == RED
        &&
            sibling_node->left->color == BLACK) {
      sibling_node->color = RED;
      sibling_node->right->color = BLACK;
      rotate_left(sibling_node);
    }
  }
  rb_tree_delete_case_6(node);
}

internal void rb_tree_delete_case_6(RB_Node *node) {
  RB_Node *sibling_node = sibling(node);
  sibling_node->color = node->parent->color;
  node->parent->color = BLACK;
  if (node == node->parent->left) {
    sibling_node->right->color = BLACK;
    rotate_left(node->parent);
  } else {
    sibling_node->left->color = BLACK;
    rotate_right(node->parent);
  }
}

Voter *rb_tree_remove(RB_Tree *tree, const char *restrict key) {
  RB_Node *to_replace = __rb_tree_search(tree, key);
  if (__glibc_unlikely(!to_replace)) return false;
  RB_Node *replacement = get_replacement_node(to_replace);

  if (replacement) {
    replacement->parent = to_replace->parent;
    if (to_replace == to_replace->parent->left) {
      to_replace->parent->left = replacement;
    } else {
      to_replace->parent->right = replacement;
    }

    if (to_replace->color == BLACK) {
      if (replacement->color == RED) {
        replacement->color = BLACK;
      } else {
        rb_tree_delete_case_1(replacement);
      }
    }
  }

  Voter *v = to_replace->data;
  free(to_replace);
  --tree->nelements;
  return v;
}

Voter *rb_tree_search(RB_Tree *tree, const char *restrict key) {
  RB_Node *result = __rb_tree_search(tree, key);
  return result ? result->data : NULL;
}

void restructure_bloom_filter(Bloom_Filter *bf, RB_Tree *tree) {
  bloom_filter_reset(bf, tree->nelements);
  LIST_HEAD(stack);
  Pointer_Link *link = pointer_link_create(tree->root);
  list_add(&stack, &link->node);
  while (!list_is_empty(&stack)) {
    link = list_first_entry(&stack, Pointer_Link, node);
    RB_Node *node = link->ptr;
    list_delete_entry(&link->node);
    free(link);
    if (node->left) {
      link = pointer_link_create(node->left);
      list_add(&stack, &link->node);
    }
    if (node->right) {
      link = pointer_link_create(node->right);
      list_add(&stack, &link->node);
    }
    bloom_filter_add(bf, node->data->id);
  }
}

void rb_tree_free(RB_Tree *tree) {
  LIST_HEAD(stack);
  Pointer_Link *link = pointer_link_create(tree->root);
  list_add(&stack, &link->node);
  while (!list_is_empty(&stack)) {
    link = list_first_entry(&stack, Pointer_Link, node);
    RB_Node *node = link->ptr;
    list_delete_entry(&link->node);
    free(link);
    if (node->left) {
      link = pointer_link_create(node->left);
      list_add(&stack, &link->node);
    }
    if (node->right) {
      link = pointer_link_create(node->right);
      list_add(&stack, &link->node);
    }
    voter_free(node->data);
    free(node);
  }
}