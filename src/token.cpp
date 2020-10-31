#include <fstream>
#include <sstream>
#include <vector>

#include "token.h"

const char *toStringLexeme(Lexeme value) {
  switch (value) {
#define X(lexeme)                                                              \
  case Lexeme::lexeme:                                                         \
    return #lexeme;
    LEXEMES
#undef X
#define X(lexeme)                                                              \
  case Lexeme::lexeme:                                                         \
    return #lexeme;
    KEYWORDS
#undef X
  }
  return "Unknown";
}

// whatever, this is only for debug code....
// unless? we expose AST stuff?
void Token::print() {
  Lexeme lex = this->type;
  printf("%s ", toStringLexeme(lex));

  switch (lex) {
  case Lexeme::String: {
    printf("%s", this->string->value);
  } break;
  case Lexeme::Integer: {
    printf("%ld", this->integer);
  } break;
  case Lexeme::Floating: {
    printf("%f", this->floating);
  } break;
  default:
    break;
  }
}

bool32 Scanner::matchChar(char c) {
  bool32 result = (this->source.peek() == c);
  this->currCol += result;

  // get and throw away
  if (result)
    this->source.get();

  return result;
}

void Scanner::print() {
  for (Token token : this->tokens) {
    token.print();
    printf(" line: %d col: %d\n", token.line, token.column);
  }
}

enum WhitespaceOptions {
  General = 0x01,
  NewLine = 0x02,
};

internal inline u32 isWhitespace(const char c) {
  u32 result = 0;

  result |= (((c == '\n') << 1) & WhitespaceOptions::NewLine);

  result |= ((c == ' ') & WhitespaceOptions::General);
  result |= ((c == '\t') & WhitespaceOptions::General);
  result |= ((c == '\v') & WhitespaceOptions::General);
  result |= ((c == '\f') & WhitespaceOptions::General);
  result |= ((c == '\r') & WhitespaceOptions::General);

  return result;
}

internal inline bool32 isDigit(const char c) { return c >= '0' && c <= '9'; }

// i dont like kebab-case
internal inline bool32 isAlpha(const char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_');
}

internal inline bool32 isAlphanumeric(const char c) {
  return isAlpha(c) || isDigit(c);
}

char Scanner::skipWhitespace() {
  // no language is truly whitespace invariant... you can't put spaces in
  // the middle of identifiers in whatever language

  char byte = 0;
  u32 whitespace = 0;
  while (this->source.good()) {
    byte = this->source.get();
    whitespace = isWhitespace(byte);

    // this could be a very expensive branch...
    if (whitespace & WhitespaceOptions::NewLine) {
      ++this->currLine;
      this->currCol = 1;
    } else if (whitespace & WhitespaceOptions::General) {
      ++this->currCol;
    } else {
      break;
    }
  }

  return this->source.good() ? byte : 0;
}

void Scanner::findNext(char c) {
  // whatever, if the file is over then we done
  while (this->source.good()) {
    char byte = this->source.get();
    ++this->currCol;
    if (byte == '\n') {
      ++this->currLine;
      this->currCol = 0;
    }

    if (c == byte) {
      return;
    }
  }
}

// TODO - this vector usage is going to be hard to remove....
// TODO - escaping characters!
// TODO - no escape string concat?
String *Scanner::matchString(const char delimit) {
  std::vector<char> value;

  while (this->source.good()) {
    char byte = this->source.get();
    ++this->currCol;

    if (byte == delimit) {
      break;
    } else if (byte == '\n') {
      ++this->currLine;
      this->currCol = 0;
    } else if (byte == '\\') {
      char escape = this->source.get();

      // which control chars do we handle??
      // TODO - do i need carriage return?
      switch (escape) {
      case 'n': {
        ++this->currLine;
        this->currCol = 0;
        byte = '\n';
      } break;
      case 't': {
        byte = '\t';
      } break;
      default: {
        byte = escape;
      } break;
      }
    }

    value.push_back(byte);
  }

  value.push_back(0);

  // i guess we're doing this....
  // TODO - learn more cpp so i dont need constructors
  String *ret = new String(value.data(), value.size());

  return ret;
}

u64 Scanner::hex2bit() {
  u64 result = 0;
  while (this->source.good()) {
    char next = this->source.peek();
    if (isDigit(next)) {
      result <<= 4;
      result |= (this->source.get() - '0');
      ++this->currCol;
    } else if (next >= 'a' && next <= 'f') {
      result <<= 4;
      result |= (this->source.get() - 'a' + 10);
      ++this->currCol;
    } else if (next >= 'A' && next <= 'F') {
      result <<= 4;
      result |= (this->source.get() - 'A' + 10);
      ++this->currCol;
    } else {
      break;
    }
  }

  return result;
}

u64 Scanner::bin2bit() {
  u64 result = 0;
  while (this->source.good()) {
    char next = this->source.peek();
    if (next == '0' || next == '1') {
      result <<= 1;
      result |= (this->source.get() - '0');
      ++this->currCol;
    } else {
      break;
    }
  }

  return result;
}

void Scanner::matchNumeric(Token *token, char start) {
  std::vector<char> value;
  char next = this->source.peek();

  // FIXME - hex or bin strings only return integers for now, fuck it
  if (start == '0') {
    if (next == 'x') {
      this->source.get();
      token->type = Lexeme::Integer;
      token->integer = this->hex2bit();
      return;
    } else if (next == 'b') {
      this->source.get();
      token->type = Lexeme::Integer;
      token->integer = this->bin2bit();
      return;
    }
  }

  value.push_back(start);

  bool32 isFloat = 0;
  while (this->source.good()) {
    if (isDigit(this->source.peek())) {
      value.push_back(this->source.get());
      ++this->currCol;
    } else if (this->source.peek() == '.') {
      isFloat = 1;
      value.push_back(this->source.get());
      ++this->currCol;
    } else if (this->source.peek() == '_') {
      ++this->currCol;
      this->source.get();
      continue;
    } else {
      break;
    }
  }

  if (this->source.peek() == 'f') {
    isFloat = 1;
    ++this->currCol;
    this->source.get();
  }

  value.push_back(0);

  if (isFloat) {
    token->type = Lexeme::Floating;
    token->floating = strtod(value.data(), 0);
  } else {
    token->type = Lexeme::Integer;
    token->integer = strtol(value.data(), 0, 10);
  }
}

// FIXME - utf8 is supposed to guarantee that no char is a prefix of
// another... so that should mean that as long as control chars are ascii,
// this should just work?
// TODO - is this copy on return expensive?
Token Scanner::advance() {
  Token token = {};

  // TODO - should i just inline this?
  char byte = this->skipWhitespace();

  // TODO - multichar tokens need to update currCol
  // probably inside the switch?
  token.line = this->currLine;
  token.column = this->currCol++;

  switch (byte) {
  case '(': {
    token.type = Lexeme::LeftParens;
  } break;
  case ')': {
    token.type = Lexeme::RightParens;
  } break;
  case '+': {
    token.type = (this->matchChar('+')) ? Lexeme::Increment : Lexeme::Add;
  } break;
  case '/': {
    // single line comment
    if (this->matchChar('/')) {
      token.type = Lexeme::Comment;
      this->findNext('\n');
    } else if (this->matchChar('*')) {
      // multi line comment
      token.type = Lexeme::Comment;
      do {
        this->findNext('*');
      } while (!this->matchChar('/'));
    } else {
      token.type = Lexeme::Divide;
    }
  } break;
  case '\"': {
    token.type = Lexeme::String;
    token.string = this->matchString('\"');
  } break;
  case '\'': {
    token.type = Lexeme::String;
    token.string = this->matchString('\'');
  } break;
  default: {
    // identifiers cannot start with a digit
    if (isDigit(byte)) {
      this->matchNumeric(&token, byte);
    } else if (isAlpha(byte)) {
      token.type = Lexeme::Identifier;
    } else {
      token.type = Lexeme::Eof;
    }
  } break;
  }

  return token;
}

void Scanner::scanTokens() {
  for (auto token = this->advance(); token.type != Lexeme::Eof;
       token = this->advance()) {
    this->tokens.push_back(token);
  }

  // TODO - do i actually need an EOF token in the stream?
  // TODO - imports!, probably need to do some other EOF shenanigans
}

