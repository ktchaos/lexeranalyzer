#ifndef PASCC_PARSER
#define PASCC_PARSER

#include <iostream>
#include <stdio.h>
#include "scanner.h"

class Parser {
private:
    Scanner* scanner;
    Token currentToken;

    void advance();
    void eat(Type tokenType);
    void programa();

    void declaracoes_variaveis(); // Implementação da função conforme a gramática
    void declaracoes_de_subprogramas(); // Implementação da função conforme a gramática
    void comando_composto(); // Implementação da função conforme a gramática

    void lista_declaracoes_variaveis();
    void lista_de_identificadores();
    void tipo();
    void argumentos();
    void lista_de_parametros();
    void comandos_opcionais();
    void lista_de_comandos();
    void comando();
    void expressao();
    void expressao_simples();
    void termo();
    void fator();
    void ativacao_de_procedimento();
    void lista_de_expressoes();

    void variavel();
    void parte_else();

    
public:
    Parser(Scanner* scanner);
    void parse();
};


#endif // PASCC_PARSER