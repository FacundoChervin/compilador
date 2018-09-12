
%{
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdarg.h>
#include <string.h>
#include "y.tab.h"
int yystopparser=0;
FILE  *yyin;

void bison_log(const char* msg, ...) {
  
  va_list arglist;
  int done;
  char buffer[1024] = "BISON -> Se reconoce regla ";

  strcat(buffer, msg);
  strcat(buffer, "\n");
  
  va_start( arglist, msg );
  vprintf( buffer, arglist );
  va_end( arglist );

  return;
}

%}

%token WHILE IF BETWEEN AVG WRITE READ DECVAR ENDDEC FLOAT STRING INTEGER AND OR NOT ELSE
%token CONST_INTEGER CONST_FLOAT CONST_STRING VARIABLE
%token ASIG A_P C_P SUM MINUS MUL DIV DOUBLE_POINTS COMMA A_C C_C A_L C_L P_C GT LT GE LE EQ NE

%%

program: list_statement                           {bison_log("%d = %s", 0, "program");}
       | area_declaracion list_statement          {bison_log("%d = %s", 0, "program");}
       ;

list_statement: statement                         {bison_log("%d = %s", 0, "body");}
              | statement list_statement          {bison_log("%d = %s", 0, "body");}
              ;

statement: between | average | write | read | while_statement | if_statement | assignement;
 
write: WRITE VARIABLE                   {bison_log("%d = %s", 0, "write");}
     | WRITE CONST_FLOAT                {bison_log("%d = %s", 0, "write");}
     | WRITE CONST_INTEGER              {bison_log("%d = %s", 0, "write");}
     | WRITE CONST_STRING               {bison_log("%d = %s", 0, "write");}
     ;

read: READ VARIABLE                     {bison_log("%d = %s", 0, "read");};

comparator: GT               {bison_log("%d = %s", 0, "comparator");}
          | LT               {bison_log("%d = %s", 0, "comparator");}
          | GE               {bison_log("%d = %s", 0, "comparator");}
          | LE               {bison_log("%d = %s", 0, "comparator");}
          | EQ               {bison_log("%d = %s", 0, "comparator");}
          | NE               {bison_log("%d = %s", 0, "comparator");};

logic_operator: AND           {bison_log("%d = %s", 0, "logic_operator");}
              | OR            {bison_log("%d = %s", 0, "logic_operator");};

while_statement: WHILE A_P condition C_P A_L list_statement C_L               {bison_log("%d = %s", 0, "while_statement");};

if_statement: IF A_P condition C_P A_L list_statement C_L                             {bison_log("%d = %s", 0, "if_statement");}
            | IF A_P condition C_P A_L list_statement C_L ELSE A_L list_statement C_L {bison_log("%d = %s", 0, "if_statement");};
            
assignement: VARIABLE ASIG expresion                      {bison_log("%d = %s", 0, "assignement");};

condition: expresion comparator expresion logic_operator expresion comparator expresion       {bison_log("%d = %s", 0, "condition");}
         | expresion comparator expresion                                                     {bison_log("%d = %s", 0, "condition");}
         | NOT expresion                                                                      {bison_log("%d = %s", 0, "condition");};

between: BETWEEN A_P VARIABLE COMMA A_C expresion P_C expresion C_C C_P {bison_log("%d = %s", 1, "between");};

average: AVG A_P A_C list_expresion C_C C_P {bison_log("%d = %s", 2, "average");};

list_expresion: expresion COMMA list_expresion  {bison_log("%d = %s", 2, "list_expresion");}
              | expresion                       {bison_log("%d = %s", 2, "list_expresion");};

expresion: expresion SUM termino   {bison_log("%d = %s", 2, "expresion");}
         | expresion MINUS termino {bison_log("%d = %s", 2, "expresion");}
         | termino                 {bison_log("%d = %s", 2, "expresion");};
         
termino: termino MUL factor        {bison_log("%d = %s", 2, "termino");}
       | termino DIV factor        {bison_log("%d = %s", 2, "termino");}
       | factor                    {bison_log("%d = %s", 2, "termino");};
       
factor: CONST_FLOAT                {bison_log("%d = %s", 2, "factor");}
      | CONST_INTEGER              {bison_log("%d = %s", 2, "factor");}
      | VARIABLE                   {bison_log("%d = %s", 2, "factor");}
      | A_P expresion C_P          {bison_log("%d = %s", 2, "factor");};
          
          
area_declaracion: DECVAR list_declaracion ENDDEC {bison_log("%d = %s", 2, "area_declaracion");};

list_declaracion: list_declaracion declaracion   {bison_log("%d = %s", 2, "list_declaracion");}
                | declaracion                    {bison_log("%d = %s", 2, "list_declaracion");};

declaracion: list_variable DOUBLE_POINTS FLOAT   {bison_log("%d = %s", 2, "declaracion");}
           | list_variable DOUBLE_POINTS STRING  {bison_log("%d = %s", 2, "declaracion");}
           | list_variable DOUBLE_POINTS INTEGER {bison_log("%d = %s", 2, "declaracion");};
          
list_variable: VARIABLE COMMA list_variable      {bison_log("%d = %s", 2, "list_variable");}
             | VARIABLE                          {bison_log("%d = %s", 2, "list_variable");};
          

%%
int main(int argc,char *argv[])
{
  if ((yyin = fopen(argv[1], "rt")) == NULL)
  {
    printf("\nNo se puede abrir el archivo: %s\n", argv[1]);
  }
  else
  {
    yyparse();
  }
  
  fclose(yyin);
  return 0;
}

int yyerror(void)
{
  printf("Syntax Error\n");
  system ("Pause");
  exit (1);
}




