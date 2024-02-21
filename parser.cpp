#include "parser.h"

Parser::Parser(Scanner* scanner) : scanner(scanner) {
    advance(); // Carrega o primeiro token
}

void Parser::advance() {
    currentToken = scanner->GetNextToken();
}

void Parser::eat(Type tokenType) {
    if (currentToken.type == tokenType) {
        std::cout << "EAT IF --> " + currentToken.lexeme << std::endl;
        advance();
    } else {
        std::cout << "EAT ELSE --> " + currentToken.lexeme << std::endl;
        std::cerr << "Erro sintático na linha " << currentToken.line << ": esperado " << kGetStrType.at(tokenType) << ", encontrado " << currentToken.lexeme << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Parser::programa() {
    eat(Type::kKeyword); // Primeiro token deve ser -> 'program'
    eat(Type::kIdentifier); // Nome do programa
    eat(Type::kDelimiter); // ';'
    
    declaracoes_variaveis();
    declaracoes_de_subprogramas();
    comando_composto();
    eat(Type::kDelimiter); // '.'
}

// Declarações - Variáveis
void Parser::declaracoes_variaveis() {
    if (currentToken.lexeme == "var") {
        eat(Type::kKeyword); // 'var'
        do { // Lê até o token não ser mais um identificador
            lista_declaracoes_variaveis();
        } while (currentToken.type == Type::kIdentifier);
    }
}

void Parser::lista_declaracoes_variaveis() {
    // Primeiramente, chama a lista de identificadores para caso tenha alguma vírgula
    lista_de_identificadores();
    eat(Type::kDelimiter); // ':'
    tipo();
    eat(Type::kDelimiter); // ';'
}

void Parser::lista_de_identificadores() {
    eat(Type::kIdentifier);

    while (currentToken.lexeme == ",") {
        eat(Type::kCommaChar); // ','
        eat(Type::kIdentifier);
    }
}

void Parser::tipo() {
    if (currentToken.lexeme == "integer" || currentToken.lexeme == "real" || currentToken.lexeme == "boolean") {
        eat(Type::kKeyword);
    } else {
        std::cerr << "Erro sintático na linha " << currentToken.line << ": tipo inválido '" << currentToken.lexeme << "'" << std::endl;
        exit(EXIT_FAILURE);
    }
}






// Declarações - Subprogramas
void Parser::declaracoes_de_subprogramas() {
    while (currentToken.lexeme == "procedure") {
        eat(Type::kKeyword); // 'procedure'
        eat(Type::kIdentifier); // nome do subprograma
        argumentos();
        eat(Type::kDelimiter); // ';'
        declaracoes_variaveis();
        declaracoes_de_subprogramas();
        comando_composto();
        eat(Type::kDelimiter); // ';'
    }
}

void Parser::argumentos() {
    if (currentToken.lexeme == "(") {
        eat(Type::kDelimiter); // '('
        lista_de_parametros();
        eat(Type::kDelimiter); // ')'
    }
}

void Parser::lista_de_parametros() {
    lista_de_identificadores();
    eat(Type::kDelimiter); // ':'
    tipo();
    while (currentToken.lexeme == ";") {
        eat(Type::kDelimiter); // ';'
        lista_de_identificadores();
        eat(Type::kDelimiter); // ':'
        tipo();
    }
}





// Comando Composto
void Parser::comando_composto() {
    eat(Type::kKeyword); // 'begin'
    comandos_opcionais();
    eat(Type::kKeyword); // 'end'
}

void Parser::comandos_opcionais() {
    if (currentToken.type != Type::kKeyword || currentToken.lexeme != "end") {
        lista_de_comandos();
    }
}

void Parser::lista_de_comandos() {
    comando();
    while (currentToken.lexeme == ";") {
        eat(Type::kDelimiter); // ';'
        comando();
    }
}

void Parser::comando() {
    if (currentToken.type == Type::kIdentifier) {
        // Pode ser uma atribuição (variável := expressão) ou ativação de procedimento.
        Token nextToken = scanner->PeekNextToken();
        if (nextToken.lexeme == ":=") {
            // Trata-se de uma atribuição.
            variavel();
            eat(Type::kCommand); // ':='
            expressao();
        } else {
            // Trata-se de uma ativação de procedimento.
            ativacao_de_procedimento();
        }
    } else if (currentToken.lexeme == "if") {
        // Trata-se de um comando condicional if.
        eat(Type::kKeyword); // 'if'
        expressao(); // Condição
        eat(Type::kKeyword); // 'then'
        comando(); // Comando a ser executado se a condição for verdadeira
        parte_else(); // Parte opcional 'else'
    } else if (currentToken.lexeme == "while") {
        // Trata-se de um comando de loop while.
        eat(Type::kKeyword); // 'while'
        expressao(); // Condição
        eat(Type::kKeyword); // 'do'
        comando(); // Comando a ser executado enquanto a condição for verdadeira
    }// else {
    //     std::cout << "ERROR ELSE --> " + currentToken.lexeme << std::endl;
    //     std::cerr << "Erro sintático: comando desconhecido na linha " << currentToken.line << std::endl;
    //     exit(EXIT_FAILURE);
    // }
}

void Parser::variavel() {
    // Assume-se que variável é um identificador neste contexto.
    eat(Type::kIdentifier);
}

void Parser::parte_else() {
    if (currentToken.lexeme == "else") {
        eat(Type::kKeyword); // 'else'
        comando(); // Comando a ser executado se a condição do 'if' for falsa
    }
    // Se não houver 'else', não faz nada, correspondendo à produção ε (vazio) na gramática.
}

void Parser::expressao() {
    // Implementação simplificada. Deve ser expandida para cobrir todos os casos de expressão.
    expressao_simples();
    if (currentToken.type == Type::kRelOperator) {
        eat(Type::kRelOperator); // Operador relacional
        expressao_simples();
    }
}

void Parser::expressao_simples() {
    // Implementação simplificada. Deve ser expandida para cobrir todos os casos de expressão simples.
    if (currentToken.type == Type::kAddOperator || currentToken.lexeme == "-" || currentToken.lexeme == "+") {
        eat(currentToken.type); // Sinal
    }
    termo();
    while (currentToken.type == Type::kAddOperator) {
        eat(Type::kAddOperator); // Operador aditivo
        termo();
    }
}

void Parser::termo() {
    // Implementação simplificada. Deve ser expandida para cobrir todos os casos de termo.
    fator();
    while (currentToken.type == Type::kMulOperator) {
        eat(Type::kMulOperator); // Operador multiplicativo
        fator();
    }
}

void Parser::fator() {
    // Implementação simplificada. Deve ser expandida para cobrir todos os casos de fator.
    if (currentToken.type == Type::kIdentifier) {
        Token nextToken = scanner->PeekNextToken();
        if (nextToken.lexeme == "(") {
            ativacao_de_procedimento();
        } else {
            eat(Type::kIdentifier);
        }
    } else if (currentToken.type == Type::kIntLiteral || currentToken.type == Type::kRealLiteral) {
        eat(currentToken.type); // Número
    } else if (currentToken.lexeme == "(") {
        eat(Type::kDelimiter); // '('
        expressao();
        eat(Type::kDelimiter); // ')'
    } else if (currentToken.lexeme == "not") {
        eat(Type::kKeyword); // 'not'
        fator();
    }
    // Adicione casos para true, false, etc.
}

void Parser::ativacao_de_procedimento() {
    eat(Type::kIdentifier); // Nome do procedimento
    if (currentToken.lexeme == "(") {
        eat(Type::kDelimiter); // '('
        lista_de_expressoes();
        eat(Type::kDelimiter); // ')'
    }
}

void Parser::lista_de_expressoes() {
    expressao();
    while (currentToken.lexeme == ",") {
        eat(Type::kDelimiter); // ','
        expressao();
    }
}



// "Parse" - Análise em si
void Parser::parse() {
    programa(); // Inicia a análise sintática
    if (currentToken.type != Type::kEOF) {
        std::cerr << "Erro sintático: código após o final do programa." << std::endl;
    } else {
        std::cout << "Buildou madafoca!" << std::endl;
    }
}
