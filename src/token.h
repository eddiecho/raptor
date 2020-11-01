#define LEXEMES                                                                \
  X(String)                                                                    \
  X(Integer)                                                                   \
  X(Floating)                                                                  \
  X(Identifier)                                                                \
  X(Eof)                                                                       \
  X(LeftParens)                                                                \
  X(RightParens)                                                               \
  X(Assign)                                                                    \
  X(Equality)                                                                  \
  X(Increment)                                                                 \
  X(Comment)                                                                   \
  X(Add)                                                                       \
  X(Subtract)                                                                  \
  X(Negate)                                                                    \
  X(Divide)

#define KEYWORDS                                                               \
  X(Function)                                                                  \
  X(If)                                                                        \
  X(Else)                                                                      \
  X(For)                                                                       \
  X(While)                                                                     \
  X(Return)                                                                    \
  X(Var)                                                                       \
  X(True)                                                                      \
  X(False)

enum class Lexeme {
#define X(value) value,
  LEXEMES
#undef X
#define X(value) value,
      KEYWORDS
#undef X
};

struct Token {
  Lexeme type;
  u32 line;
  u32 column;
  union {
    u64 integer;
    f64 floating;
    const String *string;
    const String *identifier;
  };

  void print();
};

// TODO - strings of errors
// TODO - need to even define error structs for decent UI
// TODO - just make this a class, all these public methods are monkaS
struct Scanner {
  std::ifstream source;
  std::vector<Token> tokens;
  std::vector<Token> errors;
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
  String *matchString(char delimit);
  f64 matchDecimal();
  void matchIdentifier(Token *token, char start);

  void matchNumeric(Token *token, char start);
  u64 hex2bit();
  u64 bin2bit();
};

