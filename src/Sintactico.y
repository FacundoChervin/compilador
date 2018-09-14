
%{
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdarg.h>
#include <string.h>
#include "y.tab.h"

#define YA_EXISTE_EN_TABLA 100
#define NO_EXISTE_EN_TABLA 101
#define ASIGNACION_INCOMPATIBLE 102

#define TIPO_STRING 200
#define TIPO_ENTERO 201
#define TIPO_FLOAT 202

#define TABLA_SIMBOLOS "ts.txt"

int yystopparser=0;
FILE  *yyin;
struct itemTabla {
    char id[50];
    char valor[50];
    int tipo;
} itemTabla ;
FILE * file;

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
%union {
int intval;
double val;
char *str_val;
}

%token WHILE IF BETWEEN AVG WRITE READ DECVAR ENDDEC FLOAT STRING INTEGER AND OR NOT ELSE
%token <intval>CONST_INTEGER 
%token <val>CONST_FLOAT
%token <str_val>CONST_STRING
%token <str_val>VARIABLE
%token ASIG A_P C_P SUM MINUS MUL DIV DOUBLE_POINTS COMMA A_C C_C A_L C_L P_C GT LT GE LE EQ NE

%%

start: program									  {bison_log("%d = %s", 0, "Compilacion completa");}

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

declaracion: list_variable DOUBLE_POINTS FLOAT   {bison_log("%d = %s", 2, "declaracion");
													struct itemTabla * var =malloc(sizeof(struct itemTabla));
													strcpy(var->id,$<str_val>1);
													strcpy(var->valor,"");
													var->tipo = TIPO_FLOAT;
													insertarEnTabla(var);
													printf("INSERTO EN TABLA LA VARIABLE %s",$<str_val>1);}
           | list_variable DOUBLE_POINTS STRING  {bison_log("%d = %s", 2, "declaracion");}
           | list_variable DOUBLE_POINTS INTEGER {bison_log("%d = %s", 2, "declaracion");};
          
list_variable: VARIABLE COMMA list_variable      {bison_log("%d = %s", 2, "list_variable");}
             | VARIABLE                          {bison_log("%d = %s", 2, "list_variable");};
          

%%
void crearTabla(){
    file = fopen(TABLA_SIMBOLOS, "w");
    fclose(file);
}

int main(int argc,char *argv[])
{
  if ((yyin = fopen(argv[1], "rt")) == NULL)
  {
    printf("\nNo se puede abrir el archivo: %s\n", argv[1]);
  }
  else
  {
	crearTabla();
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




int insertarEnTabla(struct itemTabla* aInsertar){
    if(obtenerItemTabla(aInsertar->id) == NO_EXISTE_EN_TABLA){
        file= fopen(TABLA_SIMBOLOS, "a");
        if (file != NULL) {
                fwrite(aInsertar, sizeof(struct itemTabla), 1, file);
                fclose(file);
        }else{
            printf("No se pudo abrir el archivo");
            exit(0);
        }
    }else{
        return YA_EXISTE_EN_TABLA;
    }
}



int obtenerItemTabla(char idABuscar[50]){
    //DEVUELVE CERO NO EXISTE
    //DEVUELVE UNO, LO ENCONTRO
    struct itemTabla *lectura=malloc(sizeof(struct itemTabla));
    file= fopen(TABLA_SIMBOLOS, "rb");
    if (file != NULL) {

        while(fread(lectura, sizeof(struct itemTabla), 1, file) == 1){

             if(strcmp(idABuscar,lectura->id)==0){
                fclose(file);
                return YA_EXISTE_EN_TABLA;
             }
        }
        fclose(file);
        return NO_EXISTE_EN_TABLA;
        //NO EXISTE EN LA TABLA
    }else{
        printf("No se pudo abrir el archivo");
        exit(0);
    }

}

int actualizarItemTabla (struct itemTabla* aInsertar){
    if(obtenerItemTabla(aInsertar->id) == YA_EXISTE_EN_TABLA){
                //Ya existe, tengo que actualizarlo
        file= fopen(TABLA_SIMBOLOS, "rb");
        FILE * file2= fopen("output2", "ab");
        struct itemTabla *lectura=malloc(sizeof(struct itemTabla));
        if (file != NULL) {
         while(fread(lectura, sizeof(struct itemTabla), 1, file) == 1){

             if(strcmp(aInsertar->id,lectura->id)==0){
                if(aInsertar->tipo != lectura->tipo){
                    return ASIGNACION_INCOMPATIBLE;
                }
                fwrite(aInsertar, sizeof(struct itemTabla), 1, file2);
             }else{
                fwrite(lectura, sizeof(struct itemTabla), 1, file2);
             }
        }
        }
        fclose(file2);
        fclose(file);
        remove(TABLA_SIMBOLOS);
        rename("output2", TABLA_SIMBOLOS);
    }else{
        return NO_EXISTE_EN_TABLA;
    }
}




