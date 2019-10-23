#ifndef EXERCISE_I__TOKENIZER_H_
#define EXERCISE_I__TOKENIZER_H_

#include <stdbool.h>
#include <stddef.h>
#include "attributes.h"

typedef struct Tokenizer {
  char *stream;
  size_t length;
  size_t index;
  char delimiter;
} Tokenizer;

bool tokenizer_has_next(Tokenizer *tokenizer) __NON_NULL(1);

char *tokenizer_next_token(Tokenizer *tokenizer) __NON_NULL(1);

size_t tokenizer_remaining_tokens(Tokenizer *tokenizer) __NON_NULL(1);

#endif //EXERCISE_I__TOKENIZER_H_
