#define bool32 u32

#define LEXEMES                                                                \
  X(Literal)                                                                   \
  X(Keyword)                                                                   \
  X(Eof)                                                                       \
  X(LeftParens)                                                                \
  X(RightParens)                                                               \
  X(Assign)                                                                    \
  X(Equality)                                                                  \
  X(Increment)                                                                 \
  X(Add)

enum class Lexeme {
#define X(value) value,
  LEXEMES
#undef X
};

struct Token {
  Lexeme type;
  u32 line;
  u32 column;
  union {
    s64 integer;
    f64 floating;
  };

  const char *toString();
};

// TODO - strings of errors
// TODO - need to even define error structs for decent UI
struct Scanner {
  std::ifstream source;
  std::vector<Token> tokens;
  u32 currLine = 1;
  u32 currCol = 1;

  Scanner(char *filename) { source = std::ifstream(filename); };

  void scanTokens();
  void print();
  Token advance();
  char skipWhitespace(char byte);
  bool32 matchChar(char c);
};

