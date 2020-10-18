#include <fstream>
#include <iostream>
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

  return result;
}

void Scanner::print() {
  for (Token token : this->tokens) {
    printf("%s\n", token.toString());
  }
}

enum WhitespaceOptions {
  General = 0x01,
  NewLine = 0x02,
};

static inline u32 isWhitespace(const char c) {
  u32 result = 0;

  result |= (((c == '\n') << 1) & WhitespaceOptions::NewLine);

  result |= ((c == ' ') & WhitespaceOptions::General);
  result |= ((c == '\t') & WhitespaceOptions::General);
  result |= ((c == '\v') & WhitespaceOptions::General);
  result |= ((c == '\f') & WhitespaceOptions::General);
  result |= ((c == '\r') & WhitespaceOptions::General);

  return result;
}

char Scanner::skipWhitespace(char byte) {
  // no language is truly whitespace invariant... you can't put spaces in
  // the middle of identifiers in whatever language

  // this could be a very expensive branch...
  while (this->source.good()) {
    u32 whitespace = isWhitespace(byte);
    if (whitespace & WhitespaceOptions::NewLine) {
      ++this->currLine;
      this->currCol = 1;
    } else if (whitespace & WhitespaceOptions::General) {
      ++this->currCol;
    } else {
      break;
    }

    byte = this->source.get();
    whitespace = isWhitespace(byte);
  }

  return this->source.good() ? byte : 0;
}

// FIXME - utf8 is supposed to guarantee that no char is a prefix of
// another... so that should mean that as long as control chars are ascii,
// this should just work?
// TODO - is this copy on return expensive?
Token Scanner::advance() {
  Token token = {};

  if (this->source.good()) {

    char byte = this->skipWhitespace(this->source.get());

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
  }

  return token;
}

void Scanner::scanTokens() {
  for (auto token = this->advance(); token.type != Lexeme::Eof;
       token = this->advance()) {
    this->tokens.push_back(token);
  }

  // TODO - do i actually need an EOF token in the stream?
}

