#include <inttypes.h>
#include <stdio.h>
#include "commands.h"
#include "voter.h"
#include "utils.h"
#include "tokenizer.h"

void lbf_command(int out_fd, bloom_filter *bf, const char *restrict key) {
  bool maybe_exists = bloom_filter_contains(bf, key);
  dprintf(out_fd, "COMMAND: lbf %1$s\n"
                  "OUTPUT:\n"
                  "\t# KEY %1$s %2$s\n"
                  "ERROR INDICATION:\n"
                  "\t-none\n", key,
          maybe_exists ? "POSSIBLY IN REGISTRY" : "NOT-IN-LBF");
}

void lrb_command(int out_fd, rb_tree *tree, const char *restrict key) {
  voter *voter = rb_tree_search(tree, key);
  dprintf(out_fd, "COMMAND: lrb %1$s\n"
                  "OUTPUT:\n"
                  "\t# KEY %1$s %2$s\n"
                  "ERROR INDICATION:\n"
                  "\t-none\n",
          key, voter ? "FOUND-IN-RBT" : "NOT-IN-RBT");
}

void ins_command(int out_fd, bloom_filter *bf, rb_tree *tree,
                 char *voter_string) {
  voter *v = voter_create_from_string(voter_string);

  bool inserted = rb_tree_insert(tree, v);
  if (inserted) {
    bloom_filter_add(bf, v->id);
    dprintf(out_fd,
            "COMMAND ins: %1$s\n"
            "OUTPUT:\n"
            "\t# REC-WITH %1$s INSERTED-IN-BF-RBT\n"
            "ERROR INDICATION:\n"
            "\t-none\n", v->id);
  } else {
    dprintf(out_fd,
            "COMMAND ins: %1$s\n"
            "OUTPUT:\n"
            "\tERROR\n"
            "ERROR INDICATION:\n"
            "\t# REC-WITH %1$s EXISTS\n", v->id);
  }
}

void find_command(int out_fd, bloom_filter *bf, rb_tree *tree,
                  const char *restrict key) {
  if (bloom_filter_contains(bf, key)) {
    voter *voter = rb_tree_search(tree, key);
    if (voter) {
      dprintf(out_fd,
              "COMMAND find: %1$s\n"
              "OUTPUT:\n"
              "\t# REC-IS: %1$s %2$s %3$s %4$c %5$" PRIu8 " %6$" PRIu64 "\n"
              "ERROR INDICATION:\n"
              "\t-none\n",
              key, voter->name, voter->surname,
              voter->gender, voter->age, voter->postal_code);
      return;
    }
  }
  dprintf(out_fd, "COMMAND find: %1$s\n"
                  "OUTPUT:\n"
                  "\t# REC-WITH %1$s NOT-IN-STRUCTS\n"
                  "ERROR INDICATION:\n"
                  "\t-none\n", key);
}

void delete_command(int out_fd, bloom_filter *bf,
                    rb_tree *tree, const char *restrict key) {

  if (bloom_filter_contains(bf, key)) {
    bool deleted = rb_tree_delete(tree, key);
    if (deleted) {
      dprintf(out_fd, "COMMAND delete: %1$s\n"
                      "OUTPUT:\n"
                      "\t# DELETED %1$s FROM-STRUCTS\n"
                      "ERROR INDICATION:\n"
                      "\t-none\n", key);
      return;
    }
  }

  dprintf(out_fd, "COMMAND delete: %1$s\n"
                  "OUTPUT:\n"
                  "\tERROR\n"
                  "ERROR INDICATION:\n"
                  "\t# KEY %1$s NOT-IN-STRUCTS\n", key);
}

void vote_command(int out_fd, bloom_filter *bf, rb_tree *tree,
                  const char *restrict key) {
  voter_vote(out_fd, bf, tree, key);
}

void voted_command(int out_fd, rb_tree *tree) {
  size_t nvoters = rb_tree_nvoters(tree, -1);
  dprintf(out_fd, "COMMAND: voted\n"
                  "OUTPUT:\n"
                  "\t# NUMBER %zu\n"
                  "ERROR INDICATION:\n"
                  "\t-none\n", nvoters);
}

void voted_postcode_command(int out_fd, rb_tree *tree, i64 postal_code) {
  size_t nvoters = rb_tree_nvoters(tree, postal_code);
  dprintf(out_fd, "COMMAND: voted %1$" PRIi64 "\n"
                  "OUTPUT:\n"
                  "\t# IN %1$" PRIi64 "VOTERS-ARE %2$zu\n"
                  "ERROR INDICATION:\n"
                  "\t-none\n", postal_code, nvoters);
}

void load_command(int out_fd, bloom_filter *bf, rb_tree *tree,
                  const char *filename) {
  file *file = read_entire_file_into_memory(filename);
  if (!file) return;

  tokenizer line_tok = {
      .stream = file->contents,
      .length = file->bytes_n,
      .index = 0U,
      .delimiter = '\n'
  };

  dprintf(out_fd, "COMMAND: load %s\n", filename);
  while (tokenizer_has_next(&line_tok)) {
    char *key = tokenizer_next_token(&line_tok);
    voter_vote(out_fd, bf, tree, key);
  }
  free_file(file);
}