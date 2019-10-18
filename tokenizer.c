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

  while (current_index != length &&
      tokenizer->stream[current_index] == delimiter) {
    ++current_index;
  }

  size_t offset = current_index != length ? 1U : 0U;
  tokenizer->stream[current_index - offset] = '\0';
  tokenizer->index = current_index;
  return &tokenizer->stream[old_pos];
}

size_t tokenizer_remaining_tokens(tokenizer *tokenizer) {
  size_t length = tokenizer->length;
  if (tokenizer->index == length) return 0U;
  size_t current_index = tokenizer->index;
  size_t remaining_tokens = 1U;
  char delimiter = tokenizer->delimiter;
  while (current_index != length) {
    if (tokenizer->stream[current_index] == delimiter) {
      ++remaining_tokens;
    }
    ++current_index;
  }
  return remaining_tokens;
}