#ifndef EXERCISE_I__VOTER_H_
#define EXERCISE_I__VOTER_H_

#include <stdbool.h>
#include "attributes.h"
#include "common.h"
#include "rb_tree.h"
#include "bloom_filter.h"

typedef struct Voter {
  char *id;
  u64 postal_code;
  char *name;
  char *surname;
  char gender;
  u8 age;
  bool has_voted : 1;
} Voter;

void voter_initialize(Voter *v,
                      const char *id,
                      const char *name,
                      const char *surname,
                      u8 age,
                      char gender,
                      u64 postal_code) __NON_NULL(1);

void voter_free(Voter *v) __NON_NULL(1);

Voter *voter_create_from_string(char *voter_string) __NON_NULL(1);

void voter_vote(int out_fd, Bloom_Filter *bf, RB_Tree *tree,
                const char *restrict id) __NON_NULL(2, 3, 4);

#endif //EXERCISE_I__VOTER_H_
