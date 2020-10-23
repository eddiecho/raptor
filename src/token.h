#define LEXEMES                                                                \
  X(String)                                                                    \
  X(Keyword)                                                                   \
  X(Eof)                                                                       \
  X(LeftParens)                                                                \
  X(RightParens)                                                               \
  X(Assign)                                                                    \
  X(Equality)                                                                  \
  X(Increment)                                                                 \
  X(Comment)                                                                   \
  X(Add)                                                                       \
  X(Divide)

enum class Lexeme {
#define X(value) value,
  LEXEMES
#undef X
};

struct String {
  char *value;
  u32 length;
};

struct Token {
  Lexeme type;
  u32 line;
  u32 column;
  union {
    s64 integer;
    f64 floating;
    String string;
  };

  void print();
  const char *toString();
  const char *valueToString();
};

// TODO - strings of errors
// TODO - need to even define error structs for decent UI
struct Scanner {
  std::ifstream source;
  std::vector<Token> tokens;
  u32 currLine = 1;
  u32 currCol = 1;

  // TODO - does this alloc?
  Scanner(char *filename) { source = std::ifstream(filename); };

  void scanTokens();
  void print();
  Token advance();
  char skipWhitespace();
  bool32 matchChar(char c);
  void findNext(char c);
  String matchString();
};

