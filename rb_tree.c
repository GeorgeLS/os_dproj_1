#include <string.h>
#include <stdlib.h>
#include "rb_tree.h"
#include "utils.h"
#include "voter.h"
#include "generic_list.h"
#include "pointer_link.h"

typedef enum color {
  BLACK, RED
} color;

typedef struct rb_node {
  struct rb_node *left;
  struct rb_node *right;
  struct rb_node *parent;
  voter *data;
  color color : 1;
} rb_node;

internal __ALWAYS_INLINE rb_node *rb_node_create(voter *data) {
  rb_node *node = malloc(sizeof(rb_node));
  if (__glibc_unlikely(!node)) return NULL;
  node->left = node->right = node->parent = NULL;
  node->color = RED;
  node->data = data;
  return node;
}

internal __ALWAYS_INLINE rb_node *grandparent(rb_node *node) {
  return node->parent ? node->parent->parent : NULL;
}

internal __ALWAYS_INLINE rb_node *sibling(rb_node *node) {
  rb_node *parent = node->parent;
  if (!parent) return NULL;
  return node == parent->left ? parent->right : parent->left;
}

internal __ALWAYS_INLINE rb_node *uncle(rb_node *node) {
  rb_node *parent = node->parent;
  if (!parent || !parent->parent) return NULL;
  return sibling(parent);
}

internal void rotate_left(rb_node *node) {
  rb_node *node_parent = node->parent;
  rb_node *new_node = node->right;
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

internal void rotate_right(rb_node *node) {
  rb_node *node_parent = node->parent;
  rb_node *new_node = node->left;
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

internal rb_node *rb_tree_insert_wrapper(rb_tree *tree, voter *data) {
  rb_node *node = rb_node_create(data);
  if (__glibc_unlikely(!node)) return NULL;
  if (tree->root != NULL) {
    rb_node *current = tree->root;
    rb_node *parent = tree->root;
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

internal void rb_tree_reorder(rb_node *node) {
  if (node->parent == NULL) {
    node->color = BLACK;
  } else if (node->parent->color == BLACK) {
    return;
  } else if (uncle(node) && uncle(node)->color == RED) {
    node->parent->color = uncle(node)->color = BLACK;
    grandparent(node)->color = RED;
    rb_tree_reorder(grandparent(node));
  } else {
    rb_node *parent = node->parent;
    rb_node *gparent = grandparent(node);
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

internal rb_node *__rb_tree_search(rb_tree *tree, const char *restrict key) {
  rb_node *current = tree->root;
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

rb_tree rb_tree_create(void) {
  return (rb_tree) {
      .root = NULL
  };
}

bool rb_tree_insert(rb_tree *tree, voter *data) {
  rb_node *new_node = rb_tree_insert_wrapper(tree, data);
  if (!new_node) return false;
  rb_tree_reorder(new_node);
  rb_node *root = new_node;
  while (root->parent != NULL) {
    root = root->parent;
  }
  tree->root = root;
  return true;
}

internal rb_node *get_replacement_node(rb_node *to_replace) {
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

internal void rb_tree_delete_case_1(rb_node *);
internal void rb_tree_delete_case_2(rb_node *);
internal void rb_tree_delete_case_3(rb_node *);
internal void rb_tree_delete_case_4(rb_node *);
internal void rb_tree_delete_case_5(rb_node *);
internal void rb_tree_delete_case_6(rb_node *);

internal void rb_tree_delete_case_1(rb_node *node) {
  if (node->parent) {
    rb_tree_delete_case_2(node);
  }
}

internal void rb_tree_delete_case_2(rb_node *node) {
  rb_node *sibling_node = sibling(node);
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

internal void rb_tree_delete_case_3(rb_node *node) {
  rb_node *sibling_node = sibling(node);
  if (node->parent->color == BLACK && sibling_node->color == BLACK &&
      sibling_node->left->color == BLACK
      && sibling_node->right->color == BLACK) {
    sibling_node->color = RED;
    rb_tree_delete_case_1(node->parent);
  } else {
    rb_tree_delete_case_4(node);
  }
}

internal void rb_tree_delete_case_4(rb_node *node) {
  rb_node *sibling_node = sibling(node);
  if (node->parent->color == RED && sibling_node->color == BLACK &&
      sibling_node->left->color == BLACK
      && sibling_node->right->color == BLACK) {
    sibling_node->color = RED;
    node->parent->color = BLACK;
  } else {
    rb_tree_delete_case_5(node);
  }
}

internal void rb_tree_delete_case_5(rb_node *node) {
  rb_node *sibling_node = sibling(node);
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

internal void rb_tree_delete_case_6(rb_node *node) {
  rb_node *sibling_node = sibling(node);
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

bool rb_tree_delete(rb_tree *tree, const char *restrict key) {
  rb_node *to_replace = __rb_tree_search(tree, key);
  if (__glibc_unlikely(!to_replace)) return false;
  rb_node *replacement = get_replacement_node(to_replace);

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

  free(to_replace);
  return true;
}

voter *rb_tree_search(rb_tree *tree, const char *restrict key) {
  rb_node *result = __rb_tree_search(tree, key);
  return result ? result->data : NULL;
}

size_t rb_tree_nvoters(rb_tree *tree, i64 postal_code) {
  size_t nvoters = 0U;
  LIST_HEAD(stack);
  pointer_link *link = pointer_link_create(tree->root);
  list_add(&stack, &link->node);
  while (!list_is_empty(&stack)) {
    link = list_first_entry(&stack, pointer_link, node);
    rb_node *node = link->ptr;
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

    if (postal_code != -1) {
      if (postal_code == node->data->postal_code) {
        nvoters += node->data->has_voted;
      }
    } else {
      nvoters += node->data->has_voted;
    }
  }

  return nvoters;
}