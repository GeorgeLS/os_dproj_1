#ifndef EXERCISE_I__POINTER_LINK_H_
#define EXERCISE_I__POINTER_LINK_H_

#include <stdlib.h>
#include "generic_list.h"

typedef struct Pointer_Link {
  void *ptr;
  Generic_List node;
} Pointer_Link;

global __ALWAYS_INLINE __NON_NULL(1)
Pointer_Link *pointer_link_create(void *ptr) {
  Pointer_Link *link = malloc(sizeof(Pointer_Link));
  link->ptr = ptr;
  INIT_LIST_HEAD(&link->node);
  return link;
}

#endif //EXERCISE_I__POINTER_LINK_H_
