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

void Token::print() {
  Lexeme lex = this->type;

  char *value = 0;
  switch (lex) {
  case Lexeme::String: {
    value = this->string->value;
  } break;
  default:
    break;
  }

  printf("%s %s", toStringLexeme(lex), value);
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

