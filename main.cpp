#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "scanner.h"
#include "parser.h"

int main(){
  FILE* f;
  f = fopen("Test4.pas", "r");
  Scanner *s = new Scanner(f);

  // ANALISADOR LÉXICO
  // s->BuildToken();
  // s->PrintToken();
  // fclose(f);

  // ANALISADOR SINTÁTICO
  while (s->BuildToken()) {} // Constrói todos os tokens antes de iniciar a análise sintática.
  Parser parser(s);
  parser.parse();
  fclose(f);
  delete s;

  return 0;

}