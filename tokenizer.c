#include "tokenizer.h"

bool tokenizer_has_next(tokenizer *tokenizer) {
  return tokenizer->index != tokenizer->length;
}

char *tokenizer_next_token(tokenizer *tokenizer) {
  size_t length = tokenizer->length;
  char delimiter = tokenizer->delimiter;
  size_t old_pos = tokenizer->index;
  size_t current_index = tokenizer->index;
  while (current_index != length &&
      tokenizer->stream[current_index] != delimiter) {
    ++current_index;
  }

  size_t delimiter_offset = 0U;
  while (current_index != length &&
      tokenizer->stream[current_index] == delimiter) {
    ++current_index;
    ++delimiter_offset;
  }

  tokenizer->stream[current_index - delimiter_offset] = '\0';
  tokenizer->index = current_index;
  return &tokenizer->stream[old_pos];
}

size_t tokenizer_remaining_tokens(tokenizer *tokenizer) {
  size_t length = tokenizer->length;
  if (tokenizer->index == length) return 0U;
  size_t current_index = tokenizer->index;
  char delimiter = tokenizer->delimiter;
  size_t remaining_tokens = tokenizer->stream[length - 1U] == delimiter
      ? 0U
      : 1U;
  while (current_index != length) {
    size_t delimiter_offset = 0U;
    if (tokenizer->stream[current_index] == delimiter) {
      ++remaining_tokens;

      while (current_index != length &&
          tokenizer->stream[current_index] == delimiter) {
        ++current_index;
        ++delimiter_offset;
      }
    }

    ++current_index;
    current_index -= delimiter_offset;
  }
  return remaining_tokens;
}