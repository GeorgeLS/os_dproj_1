#include "voter.h"
#include "bloom_filter.h"
#include "tokenizer.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void voter_initialize(Voter *v,
                      const char *id,
                      const char *name,
                      const char *surname,
                      u8 age,
                      char gender,
                      u64 postal_code) {
  v->has_voted = false;
  v->id = strdup(id);
  v->name = strdup(name);
  v->surname = strdup(surname);
  v->gender = gender;
  v->age = age;
  v->postal_code = postal_code;
}

void voter_free(Voter *v) {
  free(v->id);
  free(v->name);
  free(v->surname);
  free(v);
}

Voter *voter_create_from_string(char *voter_string) {
  Voter *v = malloc(sizeof(Voter));
  Tokenizer tok = {
      .stream = voter_string,
      .length = strlen(voter_string),
      .index = 0U,
      .delimiter = ' '
  };

  char *id = tokenizer_next_token(&tok);
  char *name = tokenizer_next_token(&tok);
  char *surname = tokenizer_next_token(&tok);
  char *age_tok = tokenizer_next_token(&tok);
  char *gender = tokenizer_next_token(&tok);
  char *postal_code_tok = tokenizer_next_token(&tok);

  i64 age;
  i64 postal_code;
  if (!string_to_i64(age_tok, &age)) goto __ERROR__;
  if (!string_to_i64(postal_code_tok, &postal_code)) goto __ERROR__;

  voter_initialize(v, id, name, surname, age, *gender, postal_code);

  return v;
  __ERROR__:
  free(v);
  return NULL;
}

void voter_vote(int out_fd, Bloom_Filter *bf, RB_Tree *tree,
                const char *restrict id) {
  if (bloom_filter_contains(bf, id)) {
    Voter *voter = rb_tree_search(tree, id);
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