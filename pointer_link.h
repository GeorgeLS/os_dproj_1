#ifndef EXERCISE_I__POINTER_LINK_H_
#define EXERCISE_I__POINTER_LINK_H_

#include <stdlib.h>
#include "generic_list.h"

typedef struct pointer_link {
  void *ptr;
  generic_list node;
} pointer_link;

global __ALWAYS_INLINE __NON_NULL(1)
pointer_link *pointer_link_create(void *ptr) {
  pointer_link *link = malloc(sizeof(pointer_link));
  link->ptr = ptr;
  INIT_LIST_HEAD(&link->node);
  return link;
}

#endif //EXERCISE_I__POINTER_LINK_H_
