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
  }
  return "Unknown";
}

const char *Token::toString() {
  Lexeme lex = this->type;
  return toStringLexeme(lex);
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
    printf("%s line: %d col: %d\n", token.toString(), token.line, token.column);
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

// FIXME - utf8 is supposed to guarantee that no char is a prefix of
// another... so that should mean that as long as control chars are ascii,
// this should just work?
// TODO - is this copy on return expensive?
Token Scanner::advance() {
  Token token = {};

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
  default: { token.type = Lexeme::Eof; } break;
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

