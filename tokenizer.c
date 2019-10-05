#include "tokenizer.h"

bool tokenizer_has_next(tokenizer *tokenizer) {
  return tokenizer->index != tokenizer->length;
}

token tokenizer_next_token(tokenizer *tokenizer) {
  size_t old_pos = tokenizer->index;
  size_t current_index = tokenizer->index;
  while (current_index != tokenizer->length &&
      tokenizer->stream[current_index] != tokenizer->delimiter) {
    ++current_index;
  }

  tokenizer->index = current_index == tokenizer->length
      ? current_index
      : current_index + 1;
  
  return (token) {
      .string = &tokenizer->stream[old_pos],
      .length = current_index - old_pos,
  };
}

size_t tokenizer_remaining_tokens(tokenizer *tokenizer) {
  if (tokenizer->index == tokenizer->length) return 0U;
  size_t current_index = tokenizer->index;
  size_t remaining_tokens = 1U;
  while (current_index != tokenizer->length) {
    if (tokenizer->stream[current_index] == tokenizer->delimiter) {
      ++remaining_tokens;
    }
    ++current_index;
  }
  return remaining_tokens;
}