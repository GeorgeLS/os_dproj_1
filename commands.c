#include <inttypes.h>
#include <stdio.h>
#include "commands.h"
#include "voter.h"
#include "utils.h"
#include "tokenizer.h"

void lbf_command(int out_fd, Bloom_Filter *bf, const char *restrict key) {
  bool maybe_exists = bloom_filter_contains(bf, key);
  dprintf(out_fd, "COMMAND: lbf %1$s\n"
                  "OUTPUT:\n"
                  "\t# KEY %1$s %2$s\n"
                  "ERROR INDICATION:\n"
                  "\t-none\n", key,
          maybe_exists ? "POSSIBLY IN REGISTRY" : "NOT-IN-LBF");
}

void lrb_command(int out_fd, RB_Tree *tree, const char *restrict key) {
  Voter *voter = rb_tree_search(tree, key);
  dprintf(out_fd, "COMMAND: lrb %1$s\n"
                  "OUTPUT:\n"
                  "\t# KEY %1$s %2$s\n"
                  "ERROR INDICATION:\n"
                  "\t-none\n",
          key, voter ? "FOUND-IN-RBT" : "NOT-IN-RBT");
}

void ins_command(int out_fd,
                 Hash_Table *hash_table,
                 Bloom_Filter *bf,
                 RB_Tree *tree,
                 char *voter_string,
                 size_t update_threshold) {
  local size_t nupdates = 0U;
  Voter *v = voter_create_from_string(voter_string);
  if (v) {
    bool inserted = rb_tree_insert(tree, v);
    if (inserted) {
      hash_table_insert(hash_table, v);
      bloom_filter_add(bf, v->id);
      ++nupdates;
      if (nupdates == update_threshold) {
        restructure_bloom_filter(bf, tree);
        nupdates = 0U;
      }
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
}

void find_command(int out_fd, Bloom_Filter *bf, RB_Tree *tree,
                  const char *restrict key) {
  if (bloom_filter_contains(bf, key)) {
    Voter *voter = rb_tree_search(tree, key);
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

void delete_command(int out_fd, Bloom_Filter *bf, RB_Tree *tree,
                    const char *restrict key, size_t update_threshold) {
  local size_t nupdates = 0U;
  if (bloom_filter_contains(bf, key)) {
    bool deleted = rb_tree_remove(tree, key);
    if (deleted) {
      ++nupdates;
      if (nupdates == update_threshold) {
        restructure_bloom_filter(bf, tree);
        nupdates = 0U;
      }
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

void vote_command(int out_fd, Bloom_Filter *bf, RB_Tree *tree,
                  const char *restrict key) {
  voter_vote(out_fd, bf, tree, key);
}

void voted_command(int out_fd, Hash_Table *hash_table) {
  size_t nvoters = hash_table_nvoters(hash_table);
  dprintf(out_fd, "COMMAND: voted\n"
                  "OUTPUT:\n"
                  "\t# NUMBER %zu\n"
                  "ERROR INDICATION:\n"
                  "\t-none\n", nvoters);
}

void voted_postcode_command(int out_fd,
                            Hash_Table *hash_table,
                            i64 postal_code) {
  size_t nvoters = hash_table_nvoters_postal_code(hash_table, postal_code);
  dprintf(out_fd, "COMMAND: voted %1$" PRIi64 "\n"
                  "OUTPUT:\n"
                  "\t# IN %1$" PRIi64 " VOTERS-ARE %2$zu\n"
                  "ERROR INDICATION:\n"
                  "\t-none\n", postal_code, nvoters);
}

void votedperpc_command(int out_fd, Hash_Table *hash_table) {
  dprintf(out_fd, "COMMAND: votedperpc\nOUTPUT:\n");
  for (size_t i = 0U; i != hash_table->n_buckets; ++i) {
    Generic_List *bucket_list = &hash_table->table[i];
    bucket_node *bnode = NULL;
    list_for_each_entry(bucket_list, bnode, chain) {
      size_t nvoters = 0U;
      Pointer_Link *voter_link = NULL;
      list_for_each_entry(&bnode->voters_list, voter_link, node) {
        nvoters += ((Voter *) voter_link->ptr)->has_voted;
      }
      dprintf(out_fd, "\t# IN %1$" PRIi64 " VOTERS-ARE %2$zu\n",
              bnode->key, nvoters);
    }
  }
  dprintf(out_fd, "ERROR INDICATION:\n\t-none\n");
}

void load_command(int out_fd, Bloom_Filter *bf, RB_Tree *tree,
                  const char *filename) {
  File *file = read_entire_file_into_memory(filename);
  if (!file) return;

  Tokenizer line_tok = {
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

void exit_command(int out_fd) {
  dprintf(out_fd, "COMMAND: exit\n"
                  "OUTPUT:\n"
                  "\t-exit program\n"
                  "ERROR INDICATION:\n"
                  "\t-none\n");
}