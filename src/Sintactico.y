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

#define MAXSIZEPILA 500
#define TABLA_SIMBOLOS "ts.txt"
#define ARCHIVO_CODIGO_INTERMEDIO "codigo_intermedio.txt"

int yystopparser=0;
FILE  *yyin;
struct itemTabla {
    char id[50];
    char valor[50];
    int tipo;
} itemTabla ;
FILE * file;

struct itemTabla *lectura;      
struct itemTabla* stackValidacion[MAXSIZEPILA];     
char* stackPolaca[MAXSIZEPILA];     
int stackIndice[MAXSIZEPILA];     
int stackInicioWhile[MAXSIZEPILA];     
int topValidacion = -1;      
int topIndice = -1;      
int topPolaca = -1;      
int topInicioWhile = -1;    
int cantidadEnAvg = 0;      
char * comparador ="";
char * operadorlogico ="";

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

int yylex();
int yyerror();

int obtenerItemTabla(char idABuscar[50]);
int insertarEnTabla(struct itemTabla* aInsertar);
void confirmarPunteroPila(int valorAAsignar);
int isemptyValidacion();
int isfullValidacion();
struct itemTabla* peekValidacion();
struct itemTabla* pilaPopValidacion();
void pilaPushValidacion(struct itemTabla* data);
int isemptyIndice();
int isfullIndice();
int peekIndice();
int pilaPopIndice();
void pilaPushIndice(int data);
int isemptyPolaca();
int isfullPolaca();
char* peekPolaca();
char* pilaPopPolaca();
void pilaPushPolaca(char* data);
void printPolaca();
void guardarPolaca();
int pilaPopInicioWhile();
void pilaPushInicioWhile(int data);
int isemptyInicioWhile();
int isfullInicioWhile();
int peekInicioWhile();

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

start: program {bison_log("%s", "Compilacion completa"); pilaPushPolaca("FIN DE PROGRAMA"); };

program: list_statement                           
       | area_declaracion list_statement;

list_statement: statement                         
              | statement list_statement;

statement: between | average | write | read | while_statement | if_statement | assignement;
 
write: WRITE variable_no_terminal       {bison_log("%s", "WRITE"); char* aux = malloc(sizeof(char) * 1024); sprintf(aux,"WRITE %s",$<str_val>2);pilaPushPolaca(aux); }
     | WRITE CONST_FLOAT                {bison_log("%s", "WRITE"); char* aux = malloc(sizeof(char) * 1024); sprintf(aux,"WRITE %f",$<val>2);pilaPushPolaca(aux); }
     | WRITE CONST_INTEGER              {bison_log("%s", "WRITE"); char* aux = malloc(sizeof(char) * 1024); sprintf(aux,"WRITE %d",$<intval>2);pilaPushPolaca(aux); }
     | WRITE MINUS CONST_FLOAT          {bison_log("%s", "WRITE"); char* aux = malloc(sizeof(char) * 1024); sprintf(aux,"WRITE -%f",$<val>3);pilaPushPolaca(aux); }
     | WRITE MINUS CONST_INTEGER        {bison_log("%s", "WRITE"); char* aux = malloc(sizeof(char) * 1024); sprintf(aux,"WRITE -%d",$<intval>3);pilaPushPolaca(aux); }
     | WRITE CONST_STRING               {bison_log("%s", "WRITE"); char* aux = malloc(sizeof(char) * 1024); sprintf(aux,"WRITE %s",$<str_val>2);pilaPushPolaca(aux); };

read: READ variable_no_terminal         {bison_log("%s", "READ"); char* aux = malloc(sizeof(char) * 1024); sprintf(aux,"READ %s",$<str_val>2);pilaPushPolaca(aux); };

comparator: GT  {comparador ="GT";}             
          | LT  {comparador ="LT";}             
          | GE  {comparador ="GE";}            
          | LE  {comparador ="LE";}             
          | EQ  {comparador ="EQ";}           
          | NE  {comparador ="NE";};

logic_operator: AND  {operadorlogico ="AND";}             
              | OR   {operadorlogico ="OR";};

while_statement:  WHILE { pilaPushInicioWhile(topPolaca+1); } 
                  A_P condition C_P A_L { confirmarPunteroPila(topPolaca+2);pilaPushPolaca("BRANCH");pilaPushIndice(topPolaca); } 
                  list_statement 
                  {
                    char* aux = malloc(sizeof(char) * 1024);
                    sprintf(aux, "%s%d", "JUMP ",pilaPopInicioWhile());
                    pilaPushPolaca(aux);
       
                  } C_L { 
                  
                    bison_log("%s", "WHILE"); 
                    confirmarPunteroPila(topPolaca+1);
                  };


if_statement: if_header { bison_log("%s", "IF"); pilaPushPolaca("BRANCH"); confirmarPunteroPila(topPolaca+1); pilaPushIndice(topPolaca); } 
              if_body 
              {
                confirmarPunteroPila(topPolaca+1);
              }; 
         
if_header: IF A_P condition C_P;

if_body: if_struct
       | if_struct ELSE 
         {
            confirmarPunteroPila(topPolaca+2);

            pilaPushPolaca("JUMP");
            pilaPushIndice(topPolaca);
            
         } if_struct;
         
if_struct: A_L list_statement C_L                               
         | A_L C_L;

assignement:  variable_no_terminal ASIG expresion {
  
                bison_log("%s","ASIGNACION");
                struct itemTabla * aAsignar = pilaPopValidacion();
                struct itemTabla * variable = pilaPopValidacion();
                              
                if(variable->tipo!=aAsignar->tipo){
                  
                  printf("Asignacion Invalida\n");
                  
                  printf("aaa %s %s %d\n", variable->id, variable->valor, variable->tipo);
                  
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
                                
                  exit(1);
                }       
                pilaPushPolaca(":=");
				
              };

variable_no_terminal: VARIABLE {
                        if(obtenerItemTabla($<str_val>1) == NO_EXISTE_EN_TABLA)
                        {
                          printf("La variable %s no fue declarada\n",$<str_val>1);
                          exit(1);
                        }
                        
                        pilaPushValidacion(lectura);    
                        pilaPushPolaca(lectura->id);
                      };

condition: expresion comparator expresion logic_operator expresion comparator expresion       {
																			bison_log("%s", "CONDICION");
																			struct itemTabla * varCondicion2 = pilaPopValidacion();
																			struct itemTabla * varCondicion1 = pilaPopValidacion();		
																			if(varCondicion1->tipo == TIPO_STRING || varCondicion2->tipo == TIPO_STRING){
																				if(varCondicion1->tipo != varCondicion2->tipo){
																					printf("Comparacion no valida, TIPO_STRING con variable no TIPO_STRING\n");
																					exit(1);
																				}
																			}
																			
																			varCondicion2 = pilaPopValidacion();
																			varCondicion1 = pilaPopValidacion();		
																			if(varCondicion1->tipo == TIPO_STRING || varCondicion2->tipo == TIPO_STRING){
																				if(varCondicion1->tipo != varCondicion2->tipo){
																					printf("Comparacion no valida, TIPO_STRING con variable no TIPO_STRING\n");
																					exit(1);
																				}
																			}
																			
																				if(strcmp(comparador,"EQ") == 0){
																					pilaPushPolaca("-");
																					pilaPushPolaca("CMP");
																					pilaPushPolaca("BRANCH IF ZERO ");
																					pilaPushIndice(topPolaca);
																					
																				}
																				if(strcmp(comparador,"NE") == 0){
																					pilaPushPolaca("-");
																					pilaPushPolaca("CMP");
																					pilaPushPolaca("BRANCH IF NOT ZERO ");
																					pilaPushIndice(topPolaca);
																					
																				}
																				if(strcmp(comparador,"GT") == 0){
																					pilaPushPolaca("-");
																					pilaPushPolaca("CMP");
																					pilaPushPolaca("BRANCH IF GREATER THAN ZERO ");
																					pilaPushIndice(topPolaca);
																					
																				}
																				if(strcmp(comparador,"LT") == 0){
																					pilaPushPolaca("-");
																					pilaPushPolaca("CMP");
																					pilaPushPolaca("BRANCH IF LESS THAN ZERO ");
																					pilaPushIndice(topPolaca);
																					
																				}
																				if(strcmp(comparador,"GE") == 0){
																					pilaPushPolaca("-");
																					pilaPushPolaca("CMP");
																					pilaPushPolaca("BRANCH IF GREATER OR ZERO ");
																					pilaPushIndice(topPolaca);
																					
																				}
																				if(strcmp(comparador,"LE") == 0){
																					pilaPushPolaca("-");
																					pilaPushPolaca("CMP");
																					pilaPushPolaca("BRANCH IF LESS OR ZERO ");
																					pilaPushIndice(topPolaca);
																					
																				}
																			
																			}
         | expresion comparator expresion                                   {bison_log("%s", "CONDICION");
																			struct itemTabla * varCondicion2 = pilaPopValidacion();
																			struct itemTabla * varCondicion1 = pilaPopValidacion();		
																			if(varCondicion1->tipo == TIPO_STRING || varCondicion2->tipo == TIPO_STRING){
																				if(varCondicion1->tipo != varCondicion2->tipo){
																					printf("Comparacion no valida, TIPO_STRING con variable no TIPO_STRING\n");
																					exit(1);
																				}
																			
																			
																			
																			}
																				if(strcmp(comparador,"EQ") == 0){
																					pilaPushPolaca("-");
																					pilaPushPolaca("CMP");
																					pilaPushPolaca("BRANCH IF ZERO ");
																					pilaPushIndice(topPolaca);
																					
																				}
																				if(strcmp(comparador,"NE") == 0){
																					pilaPushPolaca("-");
																					pilaPushPolaca("CMP");
																					pilaPushPolaca("BRANCH IF NOT ZERO ");
																					pilaPushIndice(topPolaca);
																					
																				}
																				if(strcmp(comparador,"GT") == 0){
																					pilaPushPolaca("-");
																					pilaPushPolaca("CMP");
																					pilaPushPolaca("BRANCH IF GREATER THAN ZERO ");
																					pilaPushIndice(topPolaca);
																					
																				}
																				if(strcmp(comparador,"LT") == 0){
																					pilaPushPolaca("-");
																					pilaPushPolaca("CMP");
																					pilaPushPolaca("BRANCH IF LESS THAN ZERO ");
																					pilaPushIndice(topPolaca);
																					
																				}
																				if(strcmp(comparador,"GE") == 0){
																					pilaPushPolaca("-");
																					pilaPushPolaca("CMP");
																					pilaPushPolaca("BRANCH IF GREATER OR ZERO ");
																					pilaPushIndice(topPolaca);
																					
																				}
																				if(strcmp(comparador,"LE") == 0){
																					pilaPushPolaca("-");
																					pilaPushPolaca("CMP");
																					pilaPushPolaca("BRANCH IF LESS OR ZERO ");
																					pilaPushIndice(topPolaca);
																					
																				}
																			}
         | NOT expresion                                                                      {bison_log("%s", "CONDICION");
																							//No valido nada
																				pilaPopValidacion();
																				pilaPushPolaca("CMP");
																				pilaPushPolaca("BRANCH GREATER THAN ZERO ");
																				pilaPushIndice(topPolaca);
																							
																							};

between: BETWEEN A_P variable_no_terminal COMMA A_C expresion P_C expresion C_C C_P {bison_log("%s", "BETWEEN");
																	  struct itemTabla * varLimite2 = pilaPopValidacion();
																	  struct itemTabla * varLimite1 = pilaPopValidacion();
																	  struct itemTabla * varAComparar = pilaPopValidacion();
																	  if(varLimite2->tipo == TIPO_STRING){
																			printf("Operacion between no acepta variables TIPO_STRING\n");
																			exit(1);
																	  }
																	  if(varLimite1->tipo == TIPO_STRING){
																			printf("Operacion between no acepta variables TIPO_STRING\n");
																			exit(1);
																	  }
																	  if(varAComparar->tipo == TIPO_STRING){
																			printf("Operacion between no acepta variables TIPO_STRING\n");
																			exit(1);
																	  }
																	  
																	  pilaPushPolaca(varAComparar->id);
																	  pilaPushPolaca("-");
																	  pilaPushPolaca("CMP");
																	  pilaPushPolaca("BRANCH IF HIGHER ");
																	  pilaPushIndice(topPolaca);
																	  pilaPushPolaca(varAComparar->id);
																	  pilaPushPolaca("-");
																	  pilaPushPolaca("CMP");
																	  pilaPushPolaca("BRANCH IF HIGHER ");
																	  pilaPushIndice(topPolaca);
																	  pilaPushPolaca("true ");
																	  pilaPushPolaca("jump ");
																	  pilaPushIndice(topPolaca);
																	  pilaPushPolaca("false ");
																	
																	//Confirmo los punteros de los saltos
																	  confirmarPunteroPila(topPolaca+1);
																	  confirmarPunteroPila(topPolaca);
																	  confirmarPunteroPila(topPolaca);


																	 
																	  
																		
																		
																		
																		};

average: AVG A_P A_C list_expresion_avg C_C C_P {bison_log("%s", "AVERAGE");
												int cantidadInicial = cantidadEnAvg;
												char aux[10];
												
												while(cantidadEnAvg !=0){
													struct itemTabla * variable = pilaPopValidacion();
													cantidadEnAvg--;
													if(variable->tipo == TIPO_STRING){
															printf("Operacion avg no acepta variables TIPO_STRING\n");
															exit(1);
													}
													
													if(cantidadEnAvg > 0){
														pilaPushPolaca("+");
													}
												}
												
												sprintf(aux, "%d", cantidadInicial);
												pilaPushPolaca(aux);
												pilaPushPolaca("/");
					
					};

list_expresion_avg: expresion COMMA list_expresion_avg {cantidadEnAvg++;}
              | expresion {cantidadEnAvg++;};

expresion: expresion SUM termino   {

                  struct itemTabla * variable1 = pilaPopValidacion();
                  struct itemTabla * variable2 = pilaPopValidacion();
                  struct itemTabla * var =malloc(sizeof(struct itemTabla));
                  if(variable1->tipo == TIPO_STRING){
                    printf("Operacion (+) Invalida para TIPO_STRING\n");
                    exit(1);
                  }
                  if(variable2->tipo == TIPO_STRING){
                    printf("Operacion (+) Invalida para TIPO_STRING\n");
                    exit(1);
                  }
                  if(variable1->tipo == TIPO_FLOAT || variable2->tipo == TIPO_FLOAT){
                    strcpy(var->id,"VariableFloatAux");
                    var->tipo=TIPO_FLOAT;
                    pilaPushValidacion(var);
                  }else{
                    strcpy(var->id,"VariableEnteroAux");
                    var->tipo=TIPO_ENTERO;
                    pilaPushValidacion(var);                  
                  }
				  
				  pilaPushPolaca("+");

                }
                
         | expresion MINUS termino {
           
                  struct itemTabla * variable1 = pilaPopValidacion();
                  struct itemTabla * variable2 = pilaPopValidacion();
                  struct itemTabla * var =malloc(sizeof(struct itemTabla));
                  if(variable1->tipo == TIPO_STRING){
                    printf("Operacion (-) Invalida para TIPO_STRING\n");
                    exit(1);
                  }
                  if(variable2->tipo == TIPO_STRING){
                    printf("Operacion (-) Invalida para TIPO_STRING\n");
                    exit(1);
                  }
                  if(variable1->tipo == TIPO_FLOAT || variable2->tipo == TIPO_FLOAT){
                    strcpy(var->id,"VariableFloatAux");
                    var->tipo=TIPO_FLOAT;
                    pilaPushValidacion(var);
                  }else{
                    strcpy(var->id,"VariableEnteroAux");
                    var->tipo=TIPO_ENTERO;
                    pilaPushValidacion(var);                  
                  }
				  
				  pilaPushPolaca("-");
				  
                }
                  
         | termino;
         
termino: termino MUL factor {
                  
                  struct itemTabla * variable1 = pilaPopValidacion();
                  struct itemTabla * variable2 = pilaPopValidacion();
                  struct itemTabla * var =malloc(sizeof(struct itemTabla));
                  if(variable1->tipo == TIPO_STRING){
                    printf("Operacion (*) Invalida para TIPO_STRING\n");
                    exit(1);
                  }
                  if(variable2->tipo == TIPO_STRING){
                    printf("Operacion (*) Invalida para TIPO_STRING\n");
                    exit(1);
                  }
                  if(variable1->tipo == TIPO_FLOAT || variable2->tipo == TIPO_FLOAT){
                    strcpy(var->id,"VariableFloatAux");
                    var->tipo=TIPO_FLOAT;
                    pilaPushValidacion(var);
                  }else{
                    strcpy(var->id,"VariableEnteroAux");
                    var->tipo=TIPO_ENTERO;
                    pilaPushValidacion(var);                  
                  }
				  
				  pilaPushPolaca("*");
				  
				  }
                  
       | termino DIV factor {
         
                  struct itemTabla * variable1 = pilaPopValidacion();
                  struct itemTabla * variable2 = pilaPopValidacion();
                  struct itemTabla * var =malloc(sizeof(struct itemTabla));
                  if(variable1->tipo == TIPO_STRING){
                    printf("Operacion (/) Invalida para TIPO_STRING\n");
                    exit(1);
                  }
                  if(variable2->tipo == TIPO_STRING){
                    printf("Operacion (/) Invalida para TIPO_STRING\n");
                    exit(1);
                  }
                  strcpy(var->id,"VariableFloatAux");
                  var->tipo=TIPO_FLOAT;
                  pilaPushValidacion(var); 
                
				pilaPushPolaca("/");
				
				}
                  
       | factor;
       
factor: CONST_FLOAT {
  
                  struct itemTabla * var =malloc(sizeof(struct itemTabla));
                  sprintf( var->id,"%f", $<val>1 );
				  strcpy(var->valor," ");
                  var->tipo = TIPO_FLOAT;
                  pilaPushValidacion(var);
                  insertarEnTabla(var);
				  
				  pilaPushPolaca(var->id);
                  
      }
	  | MINUS CONST_FLOAT {
  
                  struct itemTabla * var =malloc(sizeof(struct itemTabla));
                  sprintf( var->id,"-%f", $<val>2 );
				  strcpy(var->valor," ");
                  var->tipo = TIPO_FLOAT;
                  pilaPushValidacion(var);
                  insertarEnTabla(var);
				  
				  pilaPushPolaca(var->id);
                  
      }
      | CONST_INTEGER {
        
                  struct itemTabla * var =malloc(sizeof(struct itemTabla));
                  sprintf( var->id,"%d", $<intval>1 );
				  strcpy(var->valor," ");
                  var->tipo = TIPO_ENTERO;
                  pilaPushValidacion(var);
                  insertarEnTabla(var);
				  
				  pilaPushPolaca(var->id);
      }       
      | MINUS CONST_INTEGER {
        
                  struct itemTabla * var =malloc(sizeof(struct itemTabla));
                  sprintf( var->id,"-%d", $<intval>2 );
				  strcpy(var->valor," ");
                  var->tipo = TIPO_ENTERO;
                  pilaPushValidacion(var);
                  insertarEnTabla(var);
				  
				  pilaPushPolaca(var->id);
      } 	  
      | VARIABLE {
        
                  if(obtenerItemTabla($<str_val>1) == NO_EXISTE_EN_TABLA)
                  {
                    printf("La variable %s no fue declarada\n",$<str_val>1);
                    exit(1);
                  }
                  pilaPushValidacion(lectura);
				  
				  pilaPushPolaca(lectura->id);
				  
                  
      }
      | A_P expresion C_P;
          
          
area_declaracion: DECVAR list_declaracion ENDDEC {bison_log("%s", "AREA DECLARACION");};

list_declaracion: list_declaracion declaracion   
                | declaracion;

declaracion: list_variable DOUBLE_POINTS FLOAT {
  
                          while(isemptyValidacion()==0){
                            struct itemTabla * var = pilaPopValidacion();
                            var->tipo = TIPO_FLOAT;
                            insertarEnTabla(var);
                          }
           }
           | list_variable DOUBLE_POINTS STRING {
             
                          while(isemptyValidacion()==0){
                            struct itemTabla * var = pilaPopValidacion();
                            var->tipo = TIPO_STRING;
                            insertarEnTabla(var);
                            
                          }   
           }
           | list_variable DOUBLE_POINTS INTEGER {
             
                            while(isemptyValidacion()==0){
                              struct itemTabla * var = pilaPopValidacion();
                              var->tipo = TIPO_ENTERO;

                              insertarEnTabla(var);
                            }      
};
          
list_variable: VARIABLE COMMA list_variable {
  
                          struct itemTabla * var =malloc(sizeof(struct itemTabla));
                          strcpy(var->id,$<str_val>1);
						  strcpy(var->valor," ");
                          var->id[strlen(var->id)-(1+(topValidacion+1)*2)]='\0';            
                          pilaPushValidacion(var);
             }
             | VARIABLE {
               
                          struct itemTabla * var =malloc(sizeof(struct itemTabla));
                          strcpy(var->id,$<str_val>1);
						  strcpy(var->valor," ");
                          var->id[strlen(var->id)-1]=0;           
                          pilaPushValidacion(var);
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
    printPolaca();
    guardarPolaca();
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
                  
                char * txtAInsertar = (char*)malloc(200);
                sprintf(txtAInsertar,"%s|%s|%d\n",aInsertar->id, aInsertar->valor, aInsertar->tipo);
                fputs(txtAInsertar,file);
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
    file= fopen(TABLA_SIMBOLOS, "r");
    char * linea = (char*)malloc(200);
    if (file != NULL) {

        while(fgets(linea,BUFSIZ,file) != NULL){
            char *token;
            char sTipo[256];

            token = strtok(linea, "|");
            strcpy(lectura->id,token);

            token = strtok(NULL, "|");
            strcpy(lectura->valor,token);

            token = strtok(NULL, "|");
            
            lectura->tipo = atoi(token);

            // printf("id :%s valor:%s tipo:%d \n",lectura->id,lectura->valor,lectura->tipo);

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
        file= fopen(TABLA_SIMBOLOS, "r");
        FILE * file2= fopen("output2", "a");
        char * linea = (char*)malloc(200);
        struct itemTabla *lectura=malloc(sizeof(struct itemTabla));
        if (file != NULL) {
         while(fgets(linea,BUFSIZ,file) != NULL){

                char *token;

                token = strtok(linea, "|");
                strcpy(lectura->id,token);

                token = strtok(NULL, "|");
                strcpy(lectura->valor,token);

                token = strtok(NULL, "|");
                lectura->tipo = atoi(token);

             if(strcmp(aInsertar->id,lectura->id)==0){
                if(aInsertar->tipo != lectura->tipo){
                    return ASIGNACION_INCOMPATIBLE;
                }
                char * txtAInsertar = (char*)malloc(200);
                sprintf(txtAInsertar,"%s|%s|%d\n",aInsertar->id,aInsertar->valor,aInsertar->tipo);
                fputs(txtAInsertar,file2);

             }else{
                char * txtAInsertar = (char*)malloc(200);
                sprintf(txtAInsertar,"%s|%s|%d\n",lectura->id,lectura->valor,lectura->tipo);
                fputs(txtAInsertar,file2);

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


void confirmarPunteroPila(int valorAAsignar) {
			
			int indicePolaca = pilaPopIndice();
			char* aux = malloc(sizeof(char) * 1024);
			sprintf(aux, "%s%d", stackPolaca[indicePolaca],valorAAsignar);
			stackPolaca[indicePolaca] = aux;
			}

int isemptyValidacion() {

   if(topValidacion == -1)
      return 1;
   else
      return 0;
}
   
int isfullValidacion() {

   if(topValidacion == MAXSIZEPILA)
      return 1;
   else
      return 0;
}

struct itemTabla* peekValidacion() {
   return stackValidacion[topValidacion];
}

struct itemTabla* pilaPopValidacion() {
   struct itemTabla* data;
  
   if(!isemptyValidacion()) {
      data = stackValidacion[topValidacion];
      topValidacion = topValidacion - 1;   
      return data;
   } else {
      printf("Could not retrieve data, stackValidacion is empty.\n");
   }
}

void pilaPushValidacion(struct itemTabla* data) {

   if(!isfullValidacion()) {
      topValidacion = topValidacion + 1;   
      stackValidacion[topValidacion] = data;
   } else {
      printf("Could not insert data, stackValidacion is full.\n");
   }
}

int isemptyIndice() {

   if(topIndice == -1)
      return 1;
   else
      return 0;
}
   
int isfullIndice() {

   if(topIndice == MAXSIZEPILA)
      return 1;
   else
      return 0;
}

int peekIndice() {
   return stackIndice[topIndice];
}

int pilaPopIndice() {
   int data;
  
   if(!isemptyIndice()) {
      data = stackIndice[topIndice];
      topIndice = topIndice - 1;   
      return data;
   } else {
      printf("Could not retrieve data, stackIndice is empty.\n");
   }
}

void pilaPushIndice(int data) {

   if(!isfullIndice()) {
      topIndice = topIndice + 1;   
      stackIndice[topIndice] = data;
   } else {
      printf("Could not insert data, stackIndice is full.\n");
   }
}

int isemptyPolaca() {

   if(topPolaca == -1)
      return 1;
   else
      return 0;
}
   
int isfullPolaca() {

   if(topPolaca == MAXSIZEPILA)
      return 1;
   else
      return 0;
}

char* peekPolaca() {
   return stackPolaca[topPolaca];
}

char* pilaPopPolaca() {
   char* data;
  
   if(!isemptyPolaca()) {
      data = stackPolaca[topPolaca];
      topPolaca = topPolaca - 1;   
      return data;
   } else {
      printf("Could not retrieve data, stackPolaca is empty.\n");
   }
}

void pilaPushPolaca(char* data) {

   if(!isfullPolaca()) {
      topPolaca = topPolaca + 1;   
      stackPolaca[topPolaca] = data;
   } else {
      printf("Could not insert data, stackPolaca is full.\n");
   }
}

void printPolaca() {
	int i = 0;
	for(i = 0; i <= topPolaca ; i++){
		printf("%d : %s\n",i,stackPolaca[i]);
	}
	
}

void guardarPolaca() {
  
  int i;
  FILE* f = fopen(ARCHIVO_CODIGO_INTERMEDIO, "w");
  if (f) {

    for(i = 0; i <= topPolaca ; i++){
      fprintf(f, "%d) %s\n", i, stackPolaca[i]);
    } 
  
  }

}

int pilaPopInicioWhile() {
   int data;
  
   if(!isemptyInicioWhile()) {
      data = stackInicioWhile[topInicioWhile];
      topInicioWhile = topInicioWhile - 1;   
      return data;
   } else {
      printf("Could not retrieve data, stackInicioWhile is empty.\n");
   }
}

void pilaPushInicioWhile(int data) {

   if(!isfullInicioWhile()) {
      topInicioWhile = topInicioWhile + 1;   
      stackInicioWhile[topInicioWhile] = data;
   } else {
      printf("Could not insert data, stackInicioWhile is full.\n");
   }
}

int isemptyInicioWhile() {

   if(topInicioWhile == -1)
      return 1;
   else
      return 0;
}
   
int isfullInicioWhile() {

   if(topInicioWhile == MAXSIZEPILA)
      return 1;
   else
      return 0;
}

int peekInicioWhile() {
   return stackInicioWhile[topInicioWhile];
}
