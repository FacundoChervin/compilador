%{
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include "y.tab.h"
FILE  *yyin;

#define MAX_STRING 256
#define MIN_INT -32768
#define MAX_INT 32767
#define MIN_FLOAT -FLT_MAX
#define MAX_FLOAT FLT_MAX

void yyerror(char*);

int my_atoi(const char *c) {
  
    int value = 0;
    int sign = 1;
    if( *c == '-' )
    {
        if( *c == '-' ) sign = -1;
        c++;
    }
    while (isdigit(*c))
    {
        value *= 10;
        value += (int) (*c-'0');
        c++;
    }
    return (value * sign);
}

double my_atof(const char* s){
  
  double value = 0, fact = 1;
  int point_seen = 0;
  const char* aux = s;
  int cant_digit_dec = 0;
    
  if (*aux == '-'){
    aux++;
    fact = -1;
  }
  
  while (isdigit(*aux) || *aux == '.' || *aux != '\0') {

    if (*aux == '.'){
      
      point_seen = 1;
      aux++;
      continue;
    }
    
    value += (int) (*aux-'0');
    if (point_seen == 1) {
      
      cant_digit_dec++;
    } 
    value *= 10;
    aux++;
    
  }  
  
  while (cant_digit_dec >= 0) {
    value = value / 10.0;
    cant_digit_dec--;
  }
  
  return value * fact;
}

void flex_log(const char* msg, ...) {
  
  va_list arglist;
  int done;
  char buffer[1024] = "FLEX -> Se reconoce token ";

  strcat(buffer, msg);
  strcat(buffer, "\n");
  
  va_start( arglist, msg );
  vprintf( buffer, arglist );
  va_end( arglist );

  return;
}


%}
%option noyywrap  
%option yylineno 
%x C_COMMENT
%x C_COMMENT_2

WHILE                     (W|w)(H|h)(I|i)(L|l)(E|e)
IF                        (I|i)(F|f)
BETWEEN                   (B|b)(E|e)(T|t)(W|w)(E|e)(E|e)(N|n)
AVG                       (A|a)(V|v)(G|g)
WRITE                     (W|w)(R|r)(I|i)(T|t)(E|e)
READ                      (R|r)(E|e)(A|a)(D|d)
DECVAR                    (D|d)(E|e)(C|c)(V|v)(A|a)(R|r)
ENDDEC                    (E|e)(N|n)(D|d)(D|d)(E|e)(C|c)
FLOAT                     (F|f)(L|l)(O|o)(A|a)(T|t)
STRING                    (S|s)(T|t)(R|r)(I|i)(N|n)(G|g)
INTEGER                   (I|i)(N|n)(T|t)(E|e)(G|g)(E|e)(R|r)
AND                       (A|a)(N|n)(D|d)
OR                        (O|o)(R|r)
NOT                       (N|n)(O|o)(T|t)
ELSE                      (E|e)(L|l)(S|s)(E|e)

NUM_INTEGER               "-"{0,1}[0-9]+
NUM_FLOAT                 "-"{0,1}([0-9]+"."[0-9]*)|([0-9]*"."[0-9]+)
PHRASE                    .*
CHAR                      [A-Za-z]
NEG                       "-"

INIT_COMMENT              "-/"
END_COMMENT               "/-"
  
%%

{WHILE}                   {flex_log("[%s]", yytext); return WHILE;}
{IF}                      {flex_log("[%s]", yytext); return IF;}
{BETWEEN}                 {flex_log("[%s]", yytext); return BETWEEN;}
{AVG}                     {flex_log("[%s]", yytext); return AVG;}
{WRITE}                   {flex_log("[%s]", yytext); return WRITE;}
{READ}                    {flex_log("[%s]", yytext); return READ;}
{DECVAR}                  {flex_log("[%s]", yytext); return DECVAR;}
{ENDDEC}                  {flex_log("[%s]", yytext); return ENDDEC;}
{FLOAT}                   {flex_log("[%s]", yytext); return FLOAT;}
{STRING}                  {flex_log("[%s]", yytext); return STRING;}
{INTEGER}                 {flex_log("[%s]", yytext); return INTEGER;}
{AND}                     {flex_log("[%s]", yytext); return AND;}
{OR}                      {flex_log("[%s]", yytext); return OR;}
{NOT}                     {flex_log("[%s]", yytext); return NOT;}
{ELSE}                    {flex_log("[%s]", yytext); return ELSE;}

":="                      {flex_log("[%s]", yytext); return ASIG;}
"("                       {flex_log("[Abre parentesis]"); return A_P;}
")"                       {flex_log("[Cierra parentesis]"); return C_P;}
"{"                       {flex_log("[Abre llaves]"); return A_L;}
"}"                       {flex_log("[Cierra llaves]"); return C_L;}
"+"                       {flex_log("[Suma]"); return SUM;}
"-"                       {flex_log("[Resta]"); return MINUS;}
"*"                       {flex_log("[Multiplicacion]"); return MUL;}
"/"                       {flex_log("[Division]"); return DIV;}
":"                       {flex_log("[Dos Puntos]"); return DOUBLE_POINTS;}
","                       {flex_log("[Coma]"); return COMMA;}
"["                       {flex_log("[Abre corchetes]"); return A_C;}
"]"                       {flex_log("[Cierra corchetes]"); return C_C;}
";"                       {flex_log("[Punto y coma]"); return P_C;}
">"                       {flex_log("[Mayor]"); return GT;}
"<"                       {flex_log("[Menor]"); return LT;}
">="                      {flex_log("[Mayor o igual]"); return GE;}
"<="                      {flex_log("[Menor o igual]"); return LE;}
"=="                      {flex_log("[Igual]"); return EQ;}
"!="                      {flex_log("[Distinto]"); return NE;}

{NUM_INTEGER}                   { flex_log("[ENTERO] %d", my_atoi(yytext));	
                                  yylval.intval = my_atoi(yytext); 
                                  if (yylval.intval < MIN_INT || yylval.intval > MAX_INT) {
                                    printf("Rango de entero invalido\n"); exit(1);
                                  } else {
                                    return CONST_INTEGER;                                   
                                  }
                                }
                                   
{NUM_FLOAT}                     { flex_log("[REAL]   %f", my_atof(yytext));	
                                  yylval.val = my_atof(yytext); 
                                  if (yylval.val < MIN_FLOAT || yylval.val > MAX_FLOAT) {
                                    printf("Rango de float invalido\n"); exit(1);
                                  } else {
                                    return CONST_FLOAT;
                                  }  
                                }
                                
"\""{PHRASE}"\""                { flex_log("[STRING] %s", yytext);			
                                  yylval.str_val = yytext; 
                                  if (strlen(yylval.str_val) > MAX_STRING){
                                    printf("Tamaño de string mayor a la cota\n"); exit(1);
                                  } else {
                                    return CONST_STRING;
                                  }
                                }
                                
({CHAR}|{NUM_INTEGER}){1,32}    {flex_log("[VARIABLE] %s", yytext);		yylval.str_val = yytext; return VARIABLE;}



{INIT_COMMENT}            	{ BEGIN(C_COMMENT); }
<C_COMMENT>{END_COMMENT} 	  { BEGIN(INITIAL); }
<C_COMMENT>\n   			      { }
<C_COMMENT>.    			      { }
<C_COMMENT>{INIT_COMMENT} 	{ BEGIN(C_COMMENT_2);}
<C_COMMENT_2>{INIT_COMMENT} {flex_log("Solo se permite un nivel de anidamiento de comentarios") ;yyerror("Solo se permite un nivel de anidamiento de comentarios"); }
<C_COMMENT_2>\n   			    { }
<C_COMMENT_2>.    			    { }
<C_COMMENT_2>{END_COMMENT} 	{ BEGIN(C_COMMENT); }

" "
"\n"
"\t"
"\r\n"

.                           { printf("Caracter invalido!"); exit(1); }
