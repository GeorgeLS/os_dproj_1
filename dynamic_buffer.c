#include <memory.h>
#include <stdlib.h>
#include "dynamic_buffer.h"

internal bool dynamic_buffer_reallocate(dynamic_buffer *buffer,
                                        size_t new_capacity) {
  byte *new_contents = malloc(new_capacity);
  if (!new_contents) return false;
  memcpy(new_contents, buffer->contents, buffer->length);
  buffer->capacity = new_capacity;
  free(buffer->contents);
  buffer->contents = new_contents;
  return true;
}

dynamic_buffer *dynamic_buffer_create(size_t capacity) {
  dynamic_buffer *buffer = malloc(sizeof(dynamic_buffer));
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

bool dynamic_buffer_write(dynamic_buffer *buffer,
                          size_t bytes_n, byte data[static bytes_n]) {
  if (bytes_n > buffer->capacity) {
    if (!dynamic_buffer_reallocate(buffer, bytes_n << 1U)) {
      return false;
    }
  }
  memcpy(buffer->contents, data, bytes_n);
  buffer->length = bytes_n;
  return true;
}

bool dynamic_buffer_append(dynamic_buffer *buffer,
                           size_t bytes_n, byte data[static bytes_n]) {
  if (buffer->length + bytes_n > buffer->capacity) {
    if (!dynamic_buffer_reallocate(buffer, (buffer->length + bytes_n) << 1U)) {
      return false;
    }
  }
  memcpy(buffer->contents + buffer->length, data, bytes_n);
  buffer->length += bytes_n;
  return true;
}

void dynamic_buffer_clear(dynamic_buffer *buffer) {
  buffer->length = 0U;
}