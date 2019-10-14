#include "voter.h"
#include <stdlib.h>
#include <string.h>

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