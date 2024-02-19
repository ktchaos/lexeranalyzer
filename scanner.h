#ifndef PASCC_LEX_SCANNER
#define PASCC_LEX_SCANNER

#include <stdio.h>
#include <queue>
#include <string>
#include "types.h"

struct Token {
  std::string lexeme;
  unsigned int line;
  Type type;

  Token() = default;
  Token(std::string lex, unsigned int lin, Type tp): lexeme(lex), line(lin), type(tp){}
};

class Scanner {
  public:
    Scanner(FILE* file);
    ~Scanner();
    void LexerError(std::string e) const;
    void PrintToken();
    bool BuildToken();
    Token GetNextToken();
  private:
    char GetNextChar();
    FILE *stream;
    std::queue<Token> queue_token;
};

#endif //PASCC_LEX_SCANNER
