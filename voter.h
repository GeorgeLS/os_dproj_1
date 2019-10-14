#ifndef EXERCISE_I__VOTER_H_
#define EXERCISE_I__VOTER_H_

#include <stdbool.h>
#include "attributes.h"
#include "common.h"

typedef struct voter {
  char *id;
  u64 postal_code;
  char *name;
  char *surname;
  char gender;
  u8 age;
  bool has_voted : 1;
} voter;

void voter_initialize(voter *v, const char *id, u64 postal_code,
    const char *name, const char *surname, char gender, u8 age) __NON_NULL(1);

#endif //EXERCISE_I__VOTER_H_
