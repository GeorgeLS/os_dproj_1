#include <stdlib.h>
#include "rb_tree.h"
#include "utils.h"

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

void rb_tree_delete(rb_tree *tree, const char *restrict key) {

}

voter *rb_tree_search(rb_tree *tree, const char *restrict key) {

}
