#ifndef EXERCISE_I__DYNAMIC_BUFFER_H_
#define EXERCISE_I__DYNAMIC_BUFFER_H_

#include <stdbool.h>
#include "attributes.h"
#include "common.h"

typedef struct dynamic_buffer {
  byte *contents;
  size_t length;
  size_t capacity;
} dynamic_buffer;

extern dynamic_buffer *stdin_buffer;

dynamic_buffer *dynamic_buffer_create(size_t capacity);

bool dynamic_buffer_write(dynamic_buffer *buffer,
    size_t bytes_n, byte data[static bytes_n]) __NON_NULL(1, 3);

bool dynamic_buffer_append(dynamic_buffer *buffer,
    size_t bytes_n, byte data[static bytes_n]) __NON_NULL(1, 3);

void dynamic_buffer_clear(dynamic_buffer *buffer) __NON_NULL(1);

#endif //EXERCISE_I__DYNAMIC_BUFFER_H_
