#ifndef EXERCISE_I__DYNAMIC_BUFFER_H_
#define EXERCISE_I__DYNAMIC_BUFFER_H_

#include <stdbool.h>
#include "attributes.h"
#include "common.h"

typedef struct Dynamic_Buffer {
  byte *contents;
  size_t length;
  size_t capacity;
} Dynamic_Buffer;

extern Dynamic_Buffer *stdin_buffer;

Dynamic_Buffer *dynamic_buffer_create(size_t capacity);

bool dynamic_buffer_append(Dynamic_Buffer *buffer,
                           size_t bytes_n, byte data[static bytes_n]) __NON_NULL(1, 3);

void dynamic_buffer_clear(Dynamic_Buffer *buffer) __NON_NULL(1);

#endif //EXERCISE_I__DYNAMIC_BUFFER_H_
