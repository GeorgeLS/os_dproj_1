#ifndef EXERCISE_I__VOTER_H_
#define EXERCISE_I__VOTER_H_

#include <stdbool.h>
#include "attributes.h"
#include "common.h"
#include "rb_tree.h"
#include "bloom_filter.h"

typedef struct voter {
  char *id;
  u64 postal_code;
  char *name;
  char *surname;
  char gender;
  u8 age;
  bool has_voted : 1;
} voter;

void voter_initialize(voter *v,
                      const char *id,
                      u64 postal_code,
                      const char *name,
                      const char *surname,
                      char gender,
                      u8 age) __NON_NULL(1);

void voter_vote(int out_fd, bloom_filter *bf, rb_tree *tree,
                const char *restrict id) __NON_NULL(2, 3, 4);

#endif //EXERCISE_I__VOTER_H_
