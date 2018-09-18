
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

#define MAXSIZEPILA 20
#define TABLA_SIMBOLOS "ts.txt"

int yystopparser=0;
FILE  *yyin;
struct itemTabla {
    char id[50];
    char valor[50];
    int tipo;
} itemTabla ;
FILE * file;

struct itemTabla *lectura;      
struct itemTabla* stack[MAXSIZEPILA];     
int top = -1;      
int cantidadEnAvg = 0;      


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

int isempty() {

   if(top == -1)
      return 1;
   else
      return 0;
}
   
int isfull() {

   if(top == MAXSIZEPILA)
      return 1;
   else
      return 0;
}

struct itemTabla* peek() {
   return stack[top];
}

struct itemTabla* pilaPop() {
   struct itemTabla* data;
  
   if(!isempty()) {
      data = stack[top];
      top = top - 1;   
      return data;
   } else {
      printf("Could not retrieve data, Stack is empty.\n");
   }
}

void pilaPush(struct itemTabla* data) {

   if(!isfull()) {
      top = top + 1;   
      stack[top] = data;
   } else {
      printf("Could not insert data, Stack is full.\n");
   }
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

start: program {bison_log("%s", "Compilacion completa");printf("\n\n pila %d",top);}

program: list_statement                           
       | area_declaracion list_statement          
       ;

list_statement: statement                         
              | statement list_statement          
              ;

statement: between | average | write | read | while_statement | if_statement | assignement;
 
write: WRITE VARIABLE       {bison_log("%s", "WRITE");}
     | WRITE CONST_FLOAT    {bison_log("%s", "WRITE");}
     | WRITE CONST_INTEGER  {bison_log("%s", "WRITE");}
     | WRITE CONST_STRING   {bison_log("%s", "WRITE");}
     ;

read: READ VARIABLE {bison_log("%s", "READ");};

comparator: GT               
          | LT               
          | GE               
          | LE               
          | EQ               
          | NE;

logic_operator: AND           
              | OR;

while_statement: WHILE A_P condition C_P A_L list_statement C_L { bison_log("%s", "WHILE"); };

if_statement: IF A_P condition C_P A_L list_statement C_L                             {bison_log("%s", "IF");}
            | IF A_P condition C_P A_L list_statement C_L ELSE A_L list_statement C_L {bison_log("%s", "IF");};
            
assignement: variable_no_terminal ASIG expresion {
  
                bison_log("%s","ASIGNACION");
                struct itemTabla * aAsignar = pilaPop();
                struct itemTabla * variable = pilaPop();
                              
                if(variable->tipo!=aAsignar->tipo){
                  
                  printf("Asignacion Invalida\n");
                  char tipo1[50];
                  char tipo2[50];
                  if(variable->tipo == TIPO_ENTERO){
                    strcpy(tipo1,"TIPO_ENTERO");
                  }
                  if(variable->tipo == TIPO_FLOAT){
                    strcpy(tipo1,"TIPO_FLOAT");
                  }
                  if(variable->tipo == TIPO_STRING){
                    strcpy(tipo1,"TIPO_STRING");
                  }
                  if(aAsignar->tipo == TIPO_ENTERO){
                    strcpy(tipo2,"TIPO_ENTERO");
                  }
                  if(aAsignar->tipo == TIPO_FLOAT){
                    strcpy(tipo2,"TIPO_FLOAT");
                  }
                  if(aAsignar->tipo == TIPO_STRING){
                    strcpy(tipo2,"TIPO_STRING");
                  }
                  printf("Tipos incompatibles %s con %s\n",&tipo1,&tipo2);
                                
                  return;
                }                             
              };

variable_no_terminal: VARIABLE {
                        if(obtenerItemTabla($<str_val>1) == NO_EXISTE_EN_TABLA)
                        {
                          printf("La variable %s no fue declarada\n",$<str_val>1);
                          return;
                        }
                        pilaPush(lectura);                
                      };

condition: expresion comparator expresion logic_operator expresion comparator expresion       {
																			bison_log("%s", "CONDICION");
																			struct itemTabla * varCondicion2 = pilaPop();
																			struct itemTabla * varCondicion1 = pilaPop();		
																			if(varCondicion1->tipo == TIPO_STRING || varCondicion2->tipo == TIPO_STRING){
																				if(varCondicion1->tipo != varCondicion2->tipo){
																					printf("Comparacion no valida, TIPO_STRING con variable no TIPO_STRING\n");
																					return;
																				}
																			}
																			
																			varCondicion2 = pilaPop();
																			varCondicion1 = pilaPop();		
																			if(varCondicion1->tipo == TIPO_STRING || varCondicion2->tipo == TIPO_STRING){
																				if(varCondicion1->tipo != varCondicion2->tipo){
																					printf("Comparacion no valida, TIPO_STRING con variable no TIPO_STRING\n");
																					return;
																				}
																			}
																			
																								}
         | expresion comparator expresion                                                     {bison_log("%s", "CONDICION");
																			struct itemTabla * varCondicion2 = pilaPop();
																			struct itemTabla * varCondicion1 = pilaPop();		
																			if(varCondicion1->tipo == TIPO_STRING || varCondicion2->tipo == TIPO_STRING){
																				if(varCondicion1->tipo != varCondicion2->tipo){
																					printf("Comparacion no valida, TIPO_STRING con variable no TIPO_STRING\n");
																					return;
																				}
																			}
																								}
         | NOT expresion                                                                      {bison_log("%s", "CONDICION");
																							//No valido nada
																							pilaPop();};

between: BETWEEN A_P variable_no_terminal COMMA A_C expresion P_C expresion C_C C_P {bison_log("%s", "BETWEEN");
																	  struct itemTabla * varLimite2 = pilaPop();
																	  struct itemTabla * varLimite1 = pilaPop();
																	  struct itemTabla * varAComparar = pilaPop();
																	  if(varLimite2->tipo == TIPO_STRING){
																			printf("Operacion between no acepta variables TIPO_STRING\n");
																			return;
																	  }
																	  if(varLimite1->tipo == TIPO_STRING){
																			printf("Operacion between no acepta variables TIPO_STRING\n");
																			return;
																	  }
																	  if(varAComparar->tipo == TIPO_STRING){
																			printf("Operacion between no acepta variables TIPO_STRING\n");
																			return;
																	  }

																		};

average: AVG A_P A_C list_expresion_avg C_C C_P {bison_log("%s", "AVERAGE");
												while(cantidadEnAvg !=0){
													struct itemTabla * variable = pilaPop();
													cantidadEnAvg--;
													if(variable->tipo == TIPO_STRING){
															printf("Operacion avg no acepta variables TIPO_STRING\n");
															return;
													}
													
												}
					
					};

list_expresion_avg: expresion COMMA list_expresion_avg {cantidadEnAvg++;}
              | expresion {cantidadEnAvg++;};

expresion: expresion SUM termino   {

                  struct itemTabla * variable1 = pilaPop();
                  struct itemTabla * variable2 = pilaPop();
                  struct itemTabla * var =malloc(sizeof(struct itemTabla));
                  if(variable1->tipo == TIPO_STRING){
                    printf("Operacion (+) Invalida para TIPO_STRING\n");
                    return;
                  }
                  if(variable2->tipo == TIPO_STRING){
                    printf("Operacion (+) Invalida para TIPO_STRING\n");
                    return;
                  }
                  if(variable1->tipo == TIPO_FLOAT || variable2->tipo == TIPO_FLOAT){
                    strcpy(var->id,"VariableFloatAux");
                    var->tipo=TIPO_FLOAT;
                    pilaPush(var);
                  }else{
                    strcpy(var->id,"VariableEnteroAux");
                    var->tipo=TIPO_ENTERO;
                    pilaPush(var);                  
                  }

                }
                
         | expresion MINUS termino {
           
                  struct itemTabla * variable1 = pilaPop();
                  struct itemTabla * variable2 = pilaPop();
                  struct itemTabla * var =malloc(sizeof(struct itemTabla));
                  if(variable1->tipo == TIPO_STRING){
                    printf("Operacion (-) Invalida para TIPO_STRING\n");
                    return;
                  }
                  if(variable2->tipo == TIPO_STRING){
                    printf("Operacion (-) Invalida para TIPO_STRING\n");
                    return;
                  }
                  if(variable1->tipo == TIPO_FLOAT || variable2->tipo == TIPO_FLOAT){
                    strcpy(var->id,"VariableFloatAux");
                    var->tipo=TIPO_FLOAT;
                    pilaPush(var);
                  }else{
                    strcpy(var->id,"VariableEnteroAux");
                    var->tipo=TIPO_ENTERO;
                    pilaPush(var);                  
                  }
                }
                  
         | termino;
         
termino: termino MUL factor {
                  
                  struct itemTabla * variable1 = pilaPop();
                  struct itemTabla * variable2 = pilaPop();
                  struct itemTabla * var =malloc(sizeof(struct itemTabla));
                  if(variable1->tipo == TIPO_STRING){
                    printf("Operacion (*) Invalida para TIPO_STRING\n");
                    return;
                  }
                  if(variable2->tipo == TIPO_STRING){
                    printf("Operacion (*) Invalida para TIPO_STRING\n");
                    return;
                  }
                  if(variable1->tipo == TIPO_FLOAT || variable2->tipo == TIPO_FLOAT){
                    strcpy(var->id,"VariableFloatAux");
                    var->tipo=TIPO_FLOAT;
                    pilaPush(var);
                  }else{
                    strcpy(var->id,"VariableEnteroAux");
                    var->tipo=TIPO_ENTERO;
                    pilaPush(var);                  
                  }}
                  
       | termino DIV factor {
         
                  struct itemTabla * variable1 = pilaPop();
                  struct itemTabla * variable2 = pilaPop();
                  struct itemTabla * var =malloc(sizeof(struct itemTabla));
                  if(variable1->tipo == TIPO_STRING){
                    printf("Operacion (/) Invalida para TIPO_STRING\n");
                    return;
                  }
                  if(variable2->tipo == TIPO_STRING){
                    printf("Operacion (/) Invalida para TIPO_STRING\n");
                    return;
                  }
                  strcpy(var->id,"VariableFloatAux");
                  var->tipo=TIPO_FLOAT;
                  pilaPush(var); 
                }
                  
       | factor;
       
factor: CONST_FLOAT {
  
                  struct itemTabla * var =malloc(sizeof(struct itemTabla));
                  sprintf( var->id,"%f", $<val>1 );
                  var->tipo = TIPO_FLOAT;
                  pilaPush(var);
                  insertarEnTabla(var);
                  
      }
      | CONST_INTEGER {
        
                  struct itemTabla * var =malloc(sizeof(struct itemTabla));
                  sprintf( var->id,"%d", $<intval>1 );
                  var->tipo = TIPO_ENTERO;
                  pilaPush(var);
                  insertarEnTabla(var);
      }                 
      | VARIABLE {
        
                  if(obtenerItemTabla($<str_val>1) == NO_EXISTE_EN_TABLA)
                  {
                    printf("La variable %s no fue declarada\n",$<str_val>1);
                    return;
                  }
                  pilaPush(lectura);
                  
      }
      | A_P expresion C_P;
          
          
area_declaracion: DECVAR list_declaracion ENDDEC {bison_log("%s", "AREA DECLARACION");};

list_declaracion: list_declaracion declaracion   
                | declaracion;

declaracion: list_variable DOUBLE_POINTS FLOAT {
  
                          while(isempty()==0){
                            struct itemTabla * var = pilaPop();
                            var->tipo = TIPO_FLOAT;
                            insertarEnTabla(var);
                          }
           }
           | list_variable DOUBLE_POINTS STRING {
             
                          while(isempty()==0){
                            struct itemTabla * var = pilaPop();
                            var->tipo = TIPO_STRING;
                            insertarEnTabla(var);
                            
                          }   
           }
           | list_variable DOUBLE_POINTS INTEGER {
             
                            while(isempty()==0){
                              struct itemTabla * var = pilaPop();
                              var->tipo = TIPO_ENTERO;
                              insertarEnTabla(var);
                            }      
};
          
list_variable: VARIABLE COMMA list_variable {
  
                          struct itemTabla * var =malloc(sizeof(struct itemTabla));
                          strcpy(var->id,$<str_val>1);
                          var->id[strlen(var->id)-(1+(top+1)*2)]='\0';            
                          pilaPush(var);
             }
             | VARIABLE {
               
                          struct itemTabla * var =malloc(sizeof(struct itemTabla));
                          strcpy(var->id,$<str_val>1);
                          var->id[strlen(var->id)-1]=0;           
                          pilaPush(var);
             };
          

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
            printf("No se pudo abrir el archivo\n");
            exit(0);
        }
    }else{
        return YA_EXISTE_EN_TABLA;
    }
}



int obtenerItemTabla(char idABuscar[50]){
    //DEVUELVE CERO NO EXISTE
    //DEVUELVE UNO, LO ENCONTRO
    lectura=malloc(sizeof(struct itemTabla));
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
        printf("No se pudo abrir el archivo\n");
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
