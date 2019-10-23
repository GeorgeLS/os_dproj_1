#include <memory.h>
#include <stdlib.h>
#include "dynamic_buffer.h"

internal bool dynamic_buffer_reallocate(Dynamic_Buffer *buffer,
                                        size_t new_capacity) {
  byte *new_contents = malloc(new_capacity);
  if (!new_contents) return false;
  memcpy(new_contents, buffer->contents, buffer->length);
  buffer->capacity = new_capacity;
  free(buffer->contents);
  buffer->contents = new_contents;
  return true;
}

Dynamic_Buffer *dynamic_buffer_create(size_t capacity) {
  Dynamic_Buffer *buffer = malloc(sizeof(Dynamic_Buffer));
  if (__glibc_unlikely(!buffer)) return NULL;
  byte *contents = malloc(capacity);
  if (__glibc_unlikely(!contents)) {
    free(buffer);
    return NULL;
  }
  buffer->contents = contents;
  buffer->length = 0U;
  buffer->capacity = capacity;
  return buffer;
}

bool dynamic_buffer_append(Dynamic_Buffer *buffer,
                           size_t bytes_n, byte data[static bytes_n]) {
  if (buffer->length + bytes_n + 1U> buffer->capacity) {
    if (!dynamic_buffer_reallocate(buffer, (buffer->length + bytes_n) << 1U)) {
      return false;
    }
  }
  memcpy(buffer->contents + buffer->length, data, bytes_n);
  buffer->length += bytes_n;
  return true;
}

void dynamic_buffer_clear(Dynamic_Buffer *buffer) {
  buffer->length = 0U;
}