#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "scanner.h"

int main(){
  FILE* f;
  f = fopen("lex1.pas", "r");
  Scanner *s = new Scanner(f);
  s->BuildToken();
  s->PrintToken();

  fclose(f);
}
