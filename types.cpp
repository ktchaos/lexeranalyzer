#include "types.h"
#include <string>

const std::string kProgram("program");
const std::string kVar("var");
const std::string kInteger("integer");
const std::string kReal("real");
const std::string kBoolean("boolean");
const std::string kProcedure("procedure");
const std::string kBegin("begin");
const std::string kEnd("end");
const std::string kIf("if");
const std::string kThen("then");
const std::string kElse("else");
const std::string kWhile("while");
const std::string kDo("do");
const std::string kNot("not");

const std::map<Type, const char*> createTypeMap() {
    std::map<Type, const char*> m;
    m.insert(std::make_pair(Type::kKeyword, "Palavra reservada"));
    m.insert(std::make_pair(Type::kIdentifier, "Identificador"));
    m.insert(std::make_pair(Type::kIntLiteral, "Número inteiro"));
    m.insert(std::make_pair(Type::kRealLiteral, "Número real"));
    m.insert(std::make_pair(Type::kDelimiter, "Delimitador"));
    m.insert(std::make_pair(Type::kCommand, "Atribuição"));
    m.insert(std::make_pair(Type::kRelOperator, "Operador relacional"));
    m.insert(std::make_pair(Type::kAddOperator, "Operador de adição"));
    m.insert(std::make_pair(Type::kMulOperator, "Operador de multiplicação"));
    m.insert(std::make_pair(Type::kEOF, "EOF"));
    return m;
}

const std::map<Type, const char*> kGetStrType = createTypeMap();