#include "parser.h"

// -------------------------------------------------------------------------
// Funcionalidades principais: constructor, advance, eat e program ---------
// -------------------------------------------------------------------------
Parser::Parser(Scanner* scanner) : scanner(scanner) {
    advance(); // Carrega o primeiro token
}

void Parser::advance() {
    currentToken = scanner->GetNextToken();
}

void Parser::eat(Type tokenType) {
    if (currentToken.type == tokenType) {
        advance();
    } else {
        std::cerr << "Erro sintático na linha " << currentToken.line << ": esperado " << kGetStrType.at(tokenType) << ", encontrado " << currentToken.lexeme << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Estrutura inicial sugerida pelo professor...
void Parser::program() {
    eat(Type::kKeyword); // Primeiro token deve ser -> 'program'
    eat(Type::kIdentifier); // Nome do programa
    eat(Type::kDelimiter); // ';'
    
    declaracoes_variaveis();
    declaracoes_de_subprogramas();
    comando_composto();
    eat(Type::kDelimiter); // '.'
}

// -------------------------------------------------------------------------
// Declarações das variáveis -----------------------------------------------
// -------------------------------------------------------------------------
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


// -------------------------------------------------------------------------
// Declarações de subprogramas ---------------------------------------------
// -------------------------------------------------------------------------
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
    // Processa o primeiro parâmetro
    lista_de_identificadores(); // Pode haver múltiplos identificadores separados por vírgulas para o mesmo tipo
    eat(Type::kDelimiter); // ':'
    tipo();

    // Enquanto houver um ponto e vírgula, significa que há mais parâmetros a serem processados
    while (currentToken.lexeme == ";") {
        eat(Type::kDelimiter); // Consome o ';', que separa os pares nome-tipo
        lista_de_identificadores(); // Processa o próximo conjunto de identificadores
        eat(Type::kDelimiter); // ':'
        tipo();
    }
}


// -------------------------------------------------------------------------
// Comando Composto --------------------------------------------------------
// -------------------------------------------------------------------------
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
            // antes precisamos verificar se depois de := há uma expressao válida

            if (currentToken.type == Type::kIdentifier || currentToken.type == Type::kIntLiteral || currentToken.type == Type::kRealLiteral || currentToken.lexeme == "(" || currentToken.lexeme == "not") {
                expressao();
            } else {
                std::cerr << "Erro sintático na linha " << currentToken.line << ": expressão inválida após ':='." << std::endl;
                exit(EXIT_FAILURE);
            }
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
    } else if (currentToken.lexeme == "begin") {
        comando_composto();
    }
}

void Parser::variavel() {
    // Assume que variável é um identificador neste contexto.
    eat(Type::kIdentifier);
}

void Parser::parte_else() {
    if (currentToken.lexeme == "else") {
        eat(Type::kKeyword); // 'else'
        comando(); // Comando a ser executado se a condição do 'if' for falsa
    }
    // Se não houver 'else', não faz nada, correspondendo à produção ε na gramática.
}

void Parser::expressao() {
    expressao_simples();
    if (currentToken.type == Type::kRelOperator) {
        eat(Type::kRelOperator); // Operador relacional
        expressao_simples();
    }
}

void Parser::expressao_simples() {
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
    fator();
    while (currentToken.type == Type::kMulOperator) {
        eat(Type::kMulOperator); // Operador multiplicativo
        fator();
    }
}

void Parser::fator() {
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


// -------------------------------------------------------------------------
// Parse - Início da compilação --------------------------------------------
// -------------------------------------------------------------------------
void Parser::parse() {
    program(); // Inicia a análise sintática
    if (currentToken.type != Type::kEOF) {
        std::cerr << "Erro sintático: código após o final do programa." << std::endl;
    } else {
        std::cout << "Buildou com sucesso!" << std::endl;
    }
}
