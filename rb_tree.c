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
  color color : 1;
  voter data;
} rb_node;

internal __ALWAYS_INLINE rb_node *rb_node_create(voter *data) {
  rb_node *node = malloc(sizeof(rb_node));
  if (__glibc_unlikely(!node)) return NULL;
  node->left = node->right = node->parent = NULL;
  node->color = RED;
  node->data = *data;
  return node;
}

internal rb_node *rb_tree_insert_wrapper(rb_tree *tree, voter *data) {
  rb_node *node = rb_node_create(data);
  if (__glibc_unlikely(!node)) return NULL;
  if (tree->root != NULL) {
    rb_node *current = tree->root;
    rb_node *parent = tree->root;
    while (current != NULL) {
      parent = current;
      if (string_less(data->id, current->data.id)) {
        current = current->left;
      } else if (string_greater(data->id, current->data.id)) {
        current = current->right;
      } else {
        free(node);
        return NULL;
      }
    }
    if (string_less(data->id, parent->data.id)) {
      parent->left = node;
    } else {
      parent->right = node;
    }
  } else {
    tree->root = node;
  }
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
  rb_node *new_node = node->right;
  node->right = new_node->left;
  new_node->left = node;
  node->parent = new_node;
  if (node->right) {
    node->right->parent = node;
  }

  rb_node *node_parent = node->parent;
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
  rb_node *new_node = node->left;
  node->left = new_node->right;
  new_node->right = node;
  node->parent = new_node;
  if (node->left) {
    node->left->parent = node;
  }

  rb_node *node_parent = node->parent;
  if (node_parent) {
    if (node == node_parent->left) {
      node_parent->left = new_node;
    } else if (node == node_parent->right) {
      node_parent->right = new_node;
    }
  }
  new_node->parent = node_parent;
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
    int order = strncmp(key, current->data.id, strlen(key));
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
  return true;
}

internal rb_node *get_replacement_node(rb_node *to_replace) {
  if (to_replace->left && to_replace->right) {
    to_replace = to_replace->right;
    while (to_replace) {
      to_replace = to_replace->left;
    }
    return to_replace;
  }
  if (!to_replace->left && !to_replace->right) return NULL;
  return to_replace->left ? to_replace->left : to_replace->right;
}

internal void rb_tree_delete_case_1(rb_node *);
internal void rb_tree_delete_case_2(rb_node *, rb_node *);
internal void rb_tree_delete_case_3(rb_node *, rb_node *);
internal void rb_tree_delete_case_4(rb_node *, rb_node *);
internal void rb_tree_delete_case_5(rb_node *, rb_node *);
internal void rb_tree_delete_case_6(rb_node *, rb_node *);

internal void rb_tree_delete_case_1(rb_node *node) {
  if (node->parent) {
    rb_tree_delete_case_2(node, sibling(node));
  }
}

internal void rb_tree_delete_case_2(rb_node *node, rb_node *sibling) {
  if (sibling->color == RED) {
    node->parent->color = RED;
    sibling->color = BLACK;
    if (node == node->parent->left) {
      rotate_left(node->parent);
    } else {
      rotate_right(node->parent);
    }
  }
  rb_tree_delete_case_3(node, sibling);
}

internal void rb_tree_delete_case_3(rb_node *node, rb_node *sibling) {
  if (node->parent->color == BLACK && sibling->color == BLACK &&
      sibling->left->color == BLACK && sibling->right->color == BLACK) {
    sibling->color = RED;
    rb_tree_delete_case_1(node->parent);
  } else {
    rb_tree_delete_case_4(node, sibling);
  }
}

internal void rb_tree_delete_case_4(rb_node *node, rb_node *sibling) {
  if (node->parent->color == RED && sibling->color == BLACK &&
      sibling->left->color == BLACK && sibling->right->color == BLACK) {
    sibling->color = RED;
    node->parent->color = BLACK;
  } else {
    rb_tree_delete_case_5(node, sibling);
  }
}

internal void rb_tree_delete_case_5(rb_node *node, rb_node *sibling) {
  if (sibling->color == BLACK) {
    if (node == node->parent->left && sibling->right->color == BLACK &&
        sibling->left->color == RED) {
      sibling->color = RED;
      sibling->left->color = BLACK;
      rotate_right(sibling);
    } else if (node == node->parent->right && sibling->right->color == RED &&
        sibling->left->color == BLACK) {
      sibling->color = RED;
      sibling->right->color = BLACK;
      rotate_left(sibling);
    }
  }
  rb_tree_delete_case_6(node, sibling);
}

internal void rb_tree_delete_case_6(rb_node *node, rb_node *sibling) {
  sibling->color = node->parent->color;
  node->parent->color = BLACK;
  if (node == node->parent->left) {
    sibling->right->color = BLACK;
    rotate_left(node->parent);
  } else {
    sibling->left->color = BLACK;
    rotate_right(node->parent);
  }
}

bool rb_tree_delete(rb_tree *tree, const char *restrict key) {
  rb_node *to_replace = __rb_tree_search(tree, key);
  if (__glibc_unlikely(!to_replace)) return false;
  rb_node *replacement = get_replacement_node(to_replace);

  replacement->parent = to_replace->parent;
  if (to_replace == to_replace->parent->left) {
    to_replace->parent->left = replacement;
  } else {
    to_replace->parent->right = replacement;
  }

  if (to_replace->color == BLACK) {
    if (replacement && replacement->color == RED) {
      replacement->color = BLACK;
    } else {
      rb_tree_delete_case_1(replacement);
    }
  }

  free(to_replace);
  return true;
}

voter *rb_tree_search(rb_tree *tree, const char *restrict key) {
  rb_node *result = __rb_tree_search(tree, key);
  return result ? &result->data : NULL;
}

size_t rb_tree_nvoters(rb_tree *tree, i64 postal_code) {
  size_t nvoters = 0U;
  LIST_HEAD(stack);
  pointer_link *link = pointer_link_create(tree->root);
  list_add(&stack, &link->node);
  while (!list_is_empty(&stack)) {
    pointer_link *_link = list_first_entry(&stack, pointer_link, node);
    rb_node *node = _link->ptr;
    list_delete_entry(&stack);
    free(_link);

    if (node->left) {
      link = pointer_link_create(node->left);
      list_add(&stack, &link->node);
    }

    if (node->right) {
      link = pointer_link_create(node->right);
      list_add(&stack, &link->node);
    }

    if (postal_code != -1) {
      if (postal_code == node->data.postal_code) {
        nvoters += node->data.has_voted;
      }
    } else {
      nvoters += node->data.has_voted;
    }
  }

  return nvoters;
}