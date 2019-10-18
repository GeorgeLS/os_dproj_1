#ifndef EXERCISE_I__TOKENIZER_H_
#define EXERCISE_I__TOKENIZER_H_

#include <stdbool.h>
#include <stddef.h>
#include "attributes.h"

typedef struct tokenizer {
  char *stream;
  size_t length;
  size_t index;
  char delimiter;
} tokenizer;

bool tokenizer_has_next(tokenizer *tokenizer) __NON_NULL(1);

char *tokenizer_next_token(tokenizer *tokenizer) __NON_NULL(1);

size_t tokenizer_remaining_tokens(tokenizer *tokenizer) __NON_NULL(1);

#endif //EXERCISE_I__TOKENIZER_H_
