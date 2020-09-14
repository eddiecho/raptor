#include "token.h"

enum Lexeme {
#define X(value) value,
  LEXEMES
#undef X
};

const char *toString(enum Lexeme value) {
  switch (value) {
#define X(lexeme)                                                              \
  case lexeme:                                                                 \
    return #lexeme;
    LEXEMES
#undef X
  }
}

struct Token {
  Lexeme type;
  void *value;
};
