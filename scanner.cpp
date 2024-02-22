#include "scanner.h"

unsigned int line = 1;
char buffer;
bool is_buffer = false;

bool IsKeyWord(const std::string& word) {
  if(word==kProgram or word==kVar or word==kInteger or word==kReal or word==kBoolean or
     word==kProcedure or word==kBegin or word==kEnd or word==kIf or word==kThen or
     word==kElse or word==kWhile or word==kDo or word==kNot)
     return true;
  else
    return false;
}

inline void SetBuffer(char ch){
  buffer = ch;
  is_buffer = true;
}

Scanner::Scanner(FILE* file): stream(file) {}

Scanner::~Scanner(){ fclose(stream); }

char Scanner::GetNextChar() {
  if(is_buffer){
    is_buffer = false;
    return buffer;
  } else {
    return getc(this->stream);
  }
}

Token Scanner::GetNextToken() {
  Token t = this->queue_token.front();
  this->queue_token.pop();
  return t;
}

Token Scanner::PeekNextToken() {
    if (queue_token.empty()) {
        BuildToken(); // Garante que haja pelo menos um token na fila
    }
    return queue_token.front(); // Retorna o próximo token sem removê-lo da fila
}

void Scanner::LexerError(std::string e) const {
  fprintf(stderr, "\n@@ Lexer error message: %s\n", e.c_str());
}

void Scanner::PrintToken() {
  Token t;

  while(!this->queue_token.empty()){
    t = GetNextToken();
    printf("Token: %s \tClassificação: %s \tLinha: %d\n", (t.lexeme).c_str(), kGetStrType.at(t.type), t.line);
  }
}

bool Scanner::BuildToken() {
  bool done = false;

  while(not done){
    char ch = GetNextChar();
    while(std::isspace(ch)){
      if(ch=='\n') ++line;
      ch = GetNextChar();
    }
    switch (ch) {
      case '{': {
        while(ch!='}'){
          ch = GetNextChar();
          if(ch==EOF){
            SetBuffer(ch);
            LexerError("unterminated comment miss '}'");
            break;
          }
        }
        break;
      }
      case ',': {
        this->queue_token.push(Token(std::string(1, ch), line, Type::kCommaChar));
        break;
      }
      case '+': case '-': {
        this->queue_token.push(Token(std::string(1, ch), line, Type::kAddOperator));
        break;
      }
      case '*': case '/': {
        this->queue_token.push(Token(std::string(1, ch), line, Type::kMulOperator));
        break;
      }
      case ';': case '.': case '(': case ')': {
        this->queue_token.push(Token(std::string(1, ch), line, Type::kDelimiter));
        break;
      }
      case '<':  case '>': {
        std::string lexeme(1, ch);
        ch = GetNextChar();

        if(ch=='=' or (ch=='>' and lexeme=="<"))
          lexeme += ch;
        else
          SetBuffer(ch);
        this->queue_token.push(Token(lexeme, line, Type::kRelOperator));
        break;
      }
      case '=': {
        this->queue_token.push(Token("=", line, Type::kRelOperator));
        break;
      }
      case ':': {
        ch = GetNextChar();
        if(ch=='='){
          this->queue_token.push(Token(":=", line, Type::kCommand));
        } else {
          this->queue_token.push(Token(":", line, Type::kDelimiter));
          SetBuffer(ch);
        }
        break;
      }
      case EOF: {
        this->queue_token.push(Token("EOF", line, Type::kEOF));
        done = true;
        break;
      }
      default: {
        if(std::isdigit(ch)){
          Type type = Type::kIntLiteral;
          std::string number(1, ch);
          ch = GetNextChar();

          while(std::isdigit(ch)){
            number += ch;
            ch = GetNextChar();
          }
          if(ch=='.') {
            number += ch;
            ch = GetNextChar();
            if(not isdigit(ch))
              LexerError("expected a digit after '.'");
            type = kRealLiteral;
            do {
              number += ch;
              ch = GetNextChar();
            } while(isdigit(ch));
          }
          this->queue_token.push(Token(number, line, type));
          SetBuffer(ch);

        } else if (std::isalpha(ch)) {
          Type type = Type::kIdentifier;
          std::string word(1, ch);
          ch = GetNextChar();

          while(std::isalnum(ch) or ch=='_'){
            word += ch;
            ch = GetNextChar();
          }

          // Verifica se a palavra é uma palavra-chave ou um operador lógico
          if(IsKeyWord(word) || word == "and" || word == "or") {
            if (word == "and") {
              type = Type::kMulOperator;
            } else if (word == "or") {
              type = Type::kAddOperator;
            } else {
              type = Type::kKeyword;
            }
          }

          this->queue_token.push(Token(word, line, type));
          SetBuffer(ch);

        } else {
          LexerError("unexpected token "+ch);
        }
      }
    }
  }
}
