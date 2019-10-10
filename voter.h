#ifndef EXERCISE_I__VOTER_H_
#define EXERCISE_I__VOTER_H_

#include <stdbool.h>
#include "common.h"

typedef struct voter {
  char *id;
  u64 postal_code;
  char *name;
  char *surename;
  char *gender;
  u8 age;
  bool has_voted : 1;
} voter;

#endif //EXERCISE_I__VOTER_H_
