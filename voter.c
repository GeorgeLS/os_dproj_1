#include "voter.h"
#include "bloom_filter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void voter_initialize(voter *v, const char *id, u64 postal_code,
                      const char *name, const char *surname, char gender,
                      u8 age) {
  v->has_voted = false;
  v->id = strdup(id);
  v->name = strdup(name);
  v->surname = strdup(surname);
  v->gender = gender;
  v->age = age;
}

void voter_vote(int out_fd, bloom_filter *bf, rb_tree *tree,
                const char *restrict id) {
  if (bloom_filter_contains(bf, id)) {
    voter *voter = rb_tree_search(tree, id);
    if (voter) {
      dprintf(out_fd,
              "COMMAND: vote %1$s\n"
              "OUTPUT:\n"
              "\t# REC-WITH %1$s %2$s\n"
              "ERROR INDICATION:\n"
              "\t-none\n",
              id, voter->has_voted ? "ALREADY-VOTED" : "SET-VOTED");
      if (!voter->has_voted) {
        voter->has_voted = true;
      }
      return;
    }
  }
  dprintf(out_fd,
          "COMMAND: vote %1$s\n"
          "OUTPUT:\n"
          "\tERROR\n"
          "ERROR INDICATION:\n"
          "\t# REC-WITH %1$s NOT-IN-STRUCTS\n", id);
}