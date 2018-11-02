
# line 2 "Grupo8.y"
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* -------------------------------------------------------------------------- */
/*                           DEFINCION CONSTANTES                             */
/* -------------------------------------------------------------------------- */
#define ESTADO_FINAL 15
#define MAXLONG 30
#define TAMMAX 100
#define TAMTOKEN 1000
#define CANTPR 21
#define USHRT_MAX   65535
#define FLT_MAX     3.40282347e+38F
#define FLT_PREC    16

//DEFINES GENERACION CODIGO
#define TIPO_ENTERO 0
#define TIPO_REAL 1
#define TIPO_STRING 2
#define ERROR_TIPO 3
#define TIPO_STRUCT 4
#define TIPO_CONST 5

#define TIPO_TERCETO_1   1 //(operando , , ) tipo
#define TIPO_TERCETO_2   2 //(operador , operando , ) tipo
#define TIPO_TERCETO_3   3 //(operador , operando , indice ) tipo
#define TIPO_TERCETO_4   4 //(operador , indice , indice ) tipo
#define TIPO_TERCETO_5   5 //(operador , indice , ) tipo
#define TIPO_TERCETO_6   6 //(operador , operando , cte ) tipo
#define TIPO_TERCETO_7   7 //(operador , operando , operando ) tipo

/* -------------------------------------------------------------------------- */
/*                             VARIABLES GLOBALES                             */
/* -------------------------------------------------------------------------- */
FILE *input;          // ARCHIVO ENTRADA.
//FILE *tokens;         //Archivo de Tokens
FILE *tos;            //Archivo de TOS
char caracter;	      // CARACTER LEIDO.
int NroToken=-1;	      // Valor que devuelve el yylex.
int TamToken=0;	      // Se utiliza para validar los tamaños de los ID y CteSTR.
char token[1000];     // Almacena el token armado.
int cteStrAbierta;    // Para validar que las ctes string esten bien formadas. Por ahora no la estamos usando.
int comAbierto;       // Para validar que los comentarios son cerrados.
int error = 0;        // para identificar si hubo o no error.
int nroLinea=1;		  // Para identificar en que línea se produjo el error.
int ptoInicio=0;	  // Para completar el nro segun corresponda EJ:.36 => 0.36
int TOStop = 0;		  // Índice de la TOS
//int yylval;           //Variable global provisoria
int tipoDato = 0;

// TABLA SIMBOLOS
struct tablaDeSimbolo{
    char nombre[100];
    char tipo  [11];
    char valor [100];
    char ren   [31];
    int longitud;
};

struct tablaDeSimbolo TOS[TAMMAX];


//VARIABLES GENERACION CODIGO INTERMEDIO -TERCETOS-
int Ind_input=0;
int Ind_out=0;
int Ind_comp=0;
int Ind_compLi=0;
int Ind_compLd=0;
int Ind_exp=0;
int Ind_condLi=0;
int Ind_condLd=0;
int Ind_cond=0;
int Ind_fact=0;
int Ind_ter=0;
int Ind_fact_tipo=0;
int Ind_exp_tipo=0;
int Ind_ter_tipo=0;
int Ind_asig=0;
int Ind_asig_tipo=0;
int Ind_if=0;
int Ind_pgm=0;
int Ind_While=0;
int Ind_asignacionmult=0;

int Ind_exps=0;
int Ind_exps_tipo=0;
int Ind_ops=0;
int Ind_ops_tipo=0;

int Ind_aux_eps=0;
int Ind_aux_eps_tipo=0;

//Variables FILTERC
int Ind_fil=0;
int Ind_expf=0;
int Ind_cont_fil=0;  //Indice de la TS de la Exp derecha en la condicion
int Ind_compf=0;
int Ind_condf=0;
int Tipo_condf=0;
int Li_aux =0;
int Ld_aux =0;
int cont_fil = 0; //Cuenta cantidad de Filterc
int Ind_cont=0; //BORRAR LUEGO
int Ind_Ts_Termino=0;
int Ind_Ts_Termino_general = 0;
int Ind_expf_aux=0;
int Ind_Ts_CondicionFilterDoble_Li = 0;
int Ind_Ts_CondicionFilterDoble_Ld = 0;
bool IsContadorCreated = false; // en condicion multiple solo lo debo crear una vez
bool IsFilterc = false; // bandera para guardar el indice de la TS para la exp del lado derecho de la condicion del filterc
char opsalto_filter_Li[5];
char opsalto_filter_Ld[5];
int Ind_exp_TS = 0;
int Ind_ter_TS = 0;
int aux_IndiceTS = 0;
int Ind_logicoFilter_Li = 0;
int Ind_logicoFilter_Ld = 0;
char opsalto_logico[5];

int flag_fil=0;
char opsalto[5];
char oplog[4];

char v_for_aux[100];
char v_for_aux_tope[100];
char v_for_aux_inc[100];
char v_etiq_for[100];
int  Ind_for_aux=0;
int  Ind_for_aux_tipo=0;
int  Ind_var_aux=0;
int  Ind_for_aux_tope=0;
int  Ind_for_aux_inc=0;
int  Ind_for_jmp=0;
int  Ind_for_idx=0;
int  Ind_etq_for=0;
int  Ind_for=0;
int  Ind_for_id=0;
int  Ind_for_cte=0;
int ii=0;
int  Ind_for_fin=0;

/* -------------------------------------------------------------------------- */
/*                           ESTRUCTURAS ESPECIALES                           */
/* -------------------------------------------------------------------------- */

//GENERACION CODIGO
struct tpila *pilaPos;
struct tpila *pilaCond;
struct tpila *pilaExp;
struct tpila *pilaCte;
struct tpila *pilaUntilRepeat;
struct tpila *pilaFor;
struct tpila *pilaForMarca;


/* -------------------------------------------------------------------------- */
/*                           DECLARACION PROTOTIPOS                           */
/* -------------------------------------------------------------------------- */
// FUNCIONES GENERALES
int yylex();
int yyparse();
int yyerror();

void GrabaArchivoBinarioTs();
void limpiaAux(tablaDeSimbolo *aux);
FILE* openFile(char *fileName, char *mode);

void leerCaracter();
void mostrarToken();
int  leerColumna(char);
void limpiarToken();

char * buscarEnTOS(int);
int insertarTOS();
void mostrarTOS();

int esPalabraReservada();
void agregarPalabrasReservadas();
void Validaciones();

// FUNCIONES LÉXICO
void Iniciar_ID();
void Agregar_ID();
void Inf_ID();
void Iniciar_Cte();
void Iniciar_Cte_Float();
void Agregar_Cte();
void Inf_Cte();
void Iniciar_Com();
void Inf_com();
void Agregar_Com();
void Fin_Com();
void Inciar_CteStr();
void Agregar_CteStr();
void Inf_CteStr();
void Inf_Asignacion();
void Inf_Menor();
void Inf_MenorIgual();
void Inf_Mayor();
void Inf_MayorIgual();
void Iniciar_Dist();
void Inf_Distinto();
void Iniciar_Igual();
void Inf_Igual();
void Iniciar_Or();
void Inf_OpOr();
void Iniciar_And();
void Inf_OpAnd();
void Inf_OpSuma();
void Inf_OpResta();
void Inf_OpProducto();
void Inf_OpDivision();
void Inf_OpParAbre();
void Inf_OpParCierra();
void Inf_OpCorchAbre();
void Inf_OpCorchCierra();
void Inf_Not();
void Nada();
void Caract_No_Val();
void Iniciar_concat();
void Inf_OpConcat();
void Inf_Sep();
void Inf_OpTipo();
void Inf_SepLista();
void Inf_Guion();

#define PR_MAIN 257
#define PR_DECLARE 258
#define PR_ENDDECLARE 259
#define PR_IF 260
#define PR_THEN 261
#define PR_ELSE 262
#define PR_FI 263
#define PR_FOR 264
#define PR_TO 265
#define PR_STEP 266
#define PR_NEXT 267
#define PR_REPEAT 268
#define PR_UNTIL 269
#define PR_GET 270
#define PR_PUT 271
#define PR_INTEGER 272
#define PR_FLOAT 273
#define PR_STRING 274
#define PR_STRUCT 275
#define ID 276
#define CTE_ENT 277
#define CTE_REAL 278
#define CTE_STRING 279
#define OP_PABRE 280
#define OP_PCIERRA 281
#define OP_OR 282
#define OP_AND 283
#define OP_NOT 284
#define OP_MENOR 285
#define OP_MENORIGUAL 286
#define OP_MAYOR 287
#define OP_MAYORIGUAL 288
#define OP_IGUAL 289
#define OP_DISTINTO 290
#define OP_SUMA 291
#define OP_RESTA 292
#define OP_MULTIPLI 293
#define OP_DIVISION 294
#define OP_CABRE 295
#define OP_CCIERRA 296
#define OP_ASIG 297
#define OP_LLABRE 298
#define OP_LLCIERRA 299
#define OP_CONCAT 300
#define SEP_DECIMAL 301
#define SEP_SENT 302
#define SEP_LISTA 303
#define PR_CONST 304
#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval, yyval;
#define YYERRCODE 256

# line 631 "Grupo8.y"




//============================================================================//
//================================= YACC CODIGO ==============================//
//============================================================================//
/* -------------------------------------------------------------------------- */
/*                            DEFINICION FUNCIONES                            */
/* -------------------------------------------------------------------------- */
/* IDENTIFICADORES ---------------------------------------------------------- */
void Iniciar_ID (){
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
    NroToken=ID; //Posible ID
}

void Agregar_ID(){
    if (strlen(token)<MAXLONG)
        token[++TamToken]=caracter;
    else{
        printf("\n ERROR LEXICO: Cantidad maxima de caracteres para un IDENTIFICADOR es: 30 caracteres. \n");
        getch();
        exit(1);
    }
}

void Inf_ID (){
    if (error == 0){
		switch (esPalabraReservada()){
            case 0:
                NroToken = ID;
				yylval = insertarTOS(); //TOS
            break;
			case 1:
                NroToken=PR_MAIN;
				yylval=1;
            break;
	     	case 2:
                NroToken=PR_DECLARE;
			  	yylval=2;
            break;
  			case 3:
                NroToken=PR_ENDDECLARE;
			  	yylval=3;
            break;
  			case 4:
                NroToken=PR_IF;
			  	yylval=4;
            break;
  	 		case 5:
                NroToken=PR_THEN;
				yylval=5;
            break;
  	 		case 6:
                NroToken=PR_ELSE;
				yylval=6;
            break;
  	 		case 7:
                NroToken=PR_FI;
				yylval=7;
            break;
  	 		case 8:
                NroToken=PR_FOR;
				yylval=8;
            break;
            case 9:
                NroToken=PR_TO;
				yylval=9;
            break;
            case 10:
                NroToken=PR_STEP;
				yylval=10;
            break;
            case 11:
              //  NroToken=PR_ROF;
				//yylval=11;
            break;
            case 12:
                NroToken=PR_REPEAT;
				yylval=12;
            break;
            case 13:
                NroToken=PR_UNTIL;
				yylval=13;
            break;
            case 14:
                NroToken=PR_GET;
				yylval=14;
            break;
            case 15:
                NroToken=PR_PUT;
				yylval=15;
            break;
            case 16:
                NroToken=PR_INTEGER;
				yylval=16;
            break;
            case 17:
                NroToken=PR_FLOAT;
				yylval=17;
            break;
            case 18:
                NroToken=PR_STRING;
				yylval=18;
            break;
            case 19:
                NroToken=PR_STRUCT;
				yylval=19;
            break;
            case 20:
                NroToken=PR_NEXT;
				yylval=20;
            break;
            case 21:
                NroToken=PR_CONST;
				yylval=21;
            break;

 		}
 		ungetc((int)caracter, input);
   }
   else
   {
        NroToken=0;
   }
}

/* CONSTANTE NUMERICAS ------------------------------------------------------ */
void Iniciar_Cte(){
    limpiarToken();
    TamToken=0;
    token[TamToken] = caracter;    	//guardamos el primer caracter del ID en la cadena auxiliar
    TamToken++;
    NroToken=CTE_ENT; //Posible CTE
}

void Iniciar_Cte_Float(){
    if (NroToken!=CTE_ENT){
        limpiarToken();
        TamToken=0;
    }

    token[TamToken] = caracter;    	//guardamos el primer caracter del ID en la cadena auxiliar
    TamToken++;
    NroToken=CTE_REAL; //Posible CTE
}

void Agregar_Cte(){
    token[TamToken] = caracter;
    TamToken++;
}

void Inf_Cte(){
    if (NroToken==CTE_ENT){
        int cte = atoi(token);
        if ( (strlen(token)>5) || (cte > USHRT_MAX) )
        {
            printf("\n ERROR LEXICO: Se supera el rango para un ENTERO.\n");
            getch();
            exit(1);
        }
    }

    if (NroToken==CTE_REAL){
        if (strcmp(token,".")==0){
            printf("\n - ERROR LEXICO: Constante REAL mal definida, se esperaba un digito \n");
            getch();
            exit(1);
        }

        int i=0, decimales=-1;

        for(i=0;i < strlen(token); i++){
            if (token[i]=='.' || decimales >-1)
                decimales++;
        }

        if (decimales > FLT_PREC ){
            printf("\n ERROR LEXICO: Se supera el rango para un REAL. \n");
            getch();
            exit(1);
        }

        double cte = atof(token);

        if (cte > FLT_MAX){
            printf("\n ERROR LEXICO: Se supera el rango para un REAL. \n");
            getch();
            exit(1);
        }

    }

    yylval= insertarTOS(); // TOS
    ungetc((int)caracter, input);
}

/* CONSTANTE STRING --------------------------------------------------------- */
void Inciar_CteStr(){
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
    cteStrAbierta++;
    NroToken=CTE_STRING; //Posible CTE_STRING
}

void Agregar_CteStr(){
    if (strlen(token)<MAXLONG)
            token[++TamToken]=caracter;

    else{
        printf("\n ERROR LEXICO: Maxima de caracteres para un STRING es: 30 caracteres. \n");
        getch();
        exit(1);
    }
}

void Inf_CteStr(){
    token[++TamToken]=caracter;
    if (error == 0){
        NroToken = CTE_STRING;
		cteStrAbierta--;
        yylval = insertarTOS(); //TOS
    }
    else{
        NroToken=0;
        cteStrAbierta=0;
    }
}

/* COMENTARIOS -------------------------------------------------------------- */
void Iniciar_Com(){
    limpiarToken();
    TamToken=0;
	token[TamToken]=caracter;
    NroToken = OP_DIVISION;
}

void Inf_com(){
    limpiarToken();
    comAbierto++;
    Nada();
    NroToken =-1;
}

void Agregar_Com(){
    limpiarToken();
    Nada();
}

void Fin_Com(){
    comAbierto=0;
	Nada();
}

/* SEPARADOR ---------------------------------------------------------------- */
void Inf_Sep(){
	tipoDato = 0;
	limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
    NroToken = SEP_SENT;
}

/* NADA --------------------------------------------------------------------- */
void Nada(){
    limpiarToken();
    TamToken=0;
}

/* CARACTER NO VALIDO ------------------------------------------------------- */
void Caract_No_Val (){
	if(strcmp(token,"&")==0)
        printf("\n ERROR LEXICO: se esperaba un & \n");

	if(strcmp(token,"|")==0)
        printf("\n ERROR LEXICO: se esperaba un | \n");

	if(strcmp(token,".")==0)
        printf("\n ERROR LEXICO: se esperaba un digito \n");
    getch();
    exit(0);
}

/* ASIGNACION --------------------------------------------------------------- */
void Inf_Asignacion(){
   NroToken = OP_ASIG;
   ungetc((int)caracter, input); //Segun Automata
}

/* SUMA --------------------------------------------------------------------- */
void Inf_OpSuma(){
    NroToken = OP_SUMA;
    ungetc((int)caracter, input); //Segun Automata
}

/* RESTA -------------------------------------------------------------------- */
void Inf_OpResta(){
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
    NroToken = OP_RESTA;
}

/* PRODUCTO ----------------------------------------------------------------- */
void Inf_OpProducto(){
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
    NroToken = OP_MULTIPLI;
}

/* DIVISION ----------------------------------------------------------------- */
void Inf_OpDivision(){
    NroToken = OP_DIVISION;
    ungetc((int)caracter, input); //Segun Automata
}

/* CONCATENA ---------------------------------------------------------------- */
void Iniciar_concat(){
    limpiarToken();
    TamToken=0;
	token[TamToken]=caracter;
	NroToken = OP_SUMA; 	//Posible SUMA
}

void Inf_OpConcat(){
    TamToken++;
    token[TamToken]=caracter;
    NroToken = OP_CONCAT;
}

/* IGUAL -------------------------------------------------------------------- */
void Iniciar_Igual(){
	limpiarToken();
    TamToken=0;
	token[TamToken]=caracter;
	NroToken = OP_ASIG;
}

void Inf_Igual(){
	TamToken++;
	token[TamToken]=caracter;
    NroToken = OP_IGUAL;
}

/* DISTINTO */
void Iniciar_Distinto(){//despues le cambio el nombre
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
    NroToken=OP_NOT;
}

void Inf_distinto(){
    TamToken++;
    token[TamToken]=caracter;
    NroToken = OP_DISTINTO;

}

/* DISTINTO ----------------------------------------------------------------- */
void Iniciar_Dist(){
	limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
	NroToken = OP_MENOR;
}

void Inf_Distinto(){
    TamToken++;
    token[TamToken]=caracter;
    NroToken = OP_DISTINTO;
}

/* MENOR -------------------------------------------------------------------- */
void Inf_Menor(){
    NroToken = OP_MENOR;
    ungetc((int)caracter, input);  //Segun Automata
}

/* MAYOR -------------------------------------------------------------------- */
void Inf_MayorIgual(){
    TamToken++;
    token[TamToken]=caracter;
    NroToken = OP_MAYORIGUAL;
}
/* asignacion ------------------------------------------------------------*/

void Iniciar_Asignacion(){
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;

}

void Inf_asig(){
    TamToken++;
    token[TamToken]=caracter;
    NroToken=OP_ASIG;
}

/* MAYORoIGUAL -------------------------------------------------------------- */
void Iniciar_MayorIgual(){
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
    NroToken = OP_MAYOR;
}

void Inf_Mayor(){
    NroToken = OP_MAYOR;
    ungetc((int)caracter, input); //segun automata
}

/* MENORoIGUAL -------------------------------------------------------------- */
void Inf_MenorIgual(){
    TamToken++;
    token[TamToken]=caracter;
    NroToken = OP_MENORIGUAL;
}

/* OR  ---------------------------------------------------------------------- */
void Iniciar_Or(){
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
}

void Inf_OpOr(){
    TamToken++;
    token[TamToken]=caracter;
    NroToken = OP_OR;
}

/* AND ---------------------------------------------------------------------- */
void Iniciar_And(){
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
}

void Inf_OpAnd(){
    TamToken++;
    token[TamToken]=caracter;
    NroToken = OP_AND;
}

/* NOT ---------------------------------------------------------------------- */
void Inf_not(){
    NroToken = OP_NOT;
    ungetc((int)caracter, input);
}

/* PARENTESIS --------------------------------------------------------------- */
void Inf_OpParAbre(){
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
    NroToken = OP_PABRE;
}

void Inf_OpParCierra(){
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
    NroToken = OP_PCIERRA;
}

/* LLAVE -------------------------------------------------------------------- */
void Inf_OpLLAbre(){
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
    NroToken = OP_LLABRE;
}

void Inf_OpLLCierra(){
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
    NroToken = OP_LLCIERRA;
}

/* CORCHETES ---------------------------------------------------------------- */
void Inf_OpCorchAbre(){
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
    NroToken = OP_CABRE;
}

void Inf_OpCorchCierra(){
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
    NroToken = OP_CCIERRA;
}

/* OPERADOR DECLARACION DE TIPO DE VARIABLE --------------------------------- */
void Inf_OpTipo(){
/*
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
    NroToken = OP_TIPO;
	*/
}

/* SEPARADOR DE UNA LISTA DE VARIALBLES O TIPOS ----------------------------- */
void Inf_SepLista(){
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
    NroToken = SEP_LISTA;

}

/* GUION ----------------------------- */
void Inf_Guion(){
/*
    limpiarToken();
    TamToken=0;
    token[TamToken]=caracter;
    NroToken = OP_GUION;
	*/
}

/* -------------------------------------------------------------------------- */
/*                           ESTRUCTURAS ESPECIALES                           */
/* -------------------------------------------------------------------------- */
//MATRIZ PUNTERO A FUNCION
void (*proceso[20][28])()={
//                   0 |            1  |            2  |            3  |            4  |            5  |             6  |            7  |                 8  |            9  |           10  |           11  |           12  |              13  |            14  |               15  |           16  |           17  |           18  |            19  |           20  |           21  |           22  |           23  |           24  |            25  |            26  |  27
//                   + |            /  |            *  |            #  |           Let |           Dig |             =  |            <  |                 >  |            &  |            |  |            !  |            "  |               .  |             [  |           	 ]   |            {  | 	         }   |            (  |             )  |            ,  |            _  |            ;  |           tab |           blan|            ent |             :  |   -
/*E0*/ 	{Iniciar_concat, Iniciar_Com   , Inf_OpProducto, Caract_No_Val, Iniciar_ID    , Iniciar_Cte   ,  Iniciar_Igual , Iniciar_Dist  , Iniciar_MayorIgual , Iniciar_And   , Iniciar_Or    , Iniciar_Distinto  , Inciar_CteStr , Iniciar_Cte_Float, Inf_OpCorchAbre, Inf_OpCorchCierra, Inf_OpLLAbre  , Inf_OpLLCierra, Inf_OpParAbre , Inf_OpParCierra, Inf_SepLista  , Inf_Guion     , Inf_Sep       , Nada          , Nada          , Nada           , Iniciar_Asignacion   ,Inf_OpResta},
/*E1*/ 	{Inf_ID        , Inf_ID        , Inf_ID        , Inf_ID        , Agregar_ID    , Agregar_ID    ,  Inf_ID        , Inf_ID        , Inf_ID             , Inf_ID        , Inf_ID        , Inf_ID        , Inf_ID        , Agregar_ID       , Inf_ID         , Inf_ID           , Inf_ID        , Inf_ID        , Inf_ID        , Inf_ID         , Inf_ID        , Inf_ID        , Inf_ID        , Inf_ID        , Inf_ID        , Inf_ID         , Inf_ID         ,Inf_ID},
/*E2*/ 	{Inf_Cte       , Inf_Cte       , Inf_Cte       , Inf_Cte       , Inf_Cte       , Agregar_Cte   ,  Inf_Cte       , Inf_Cte       , Inf_Cte            , Inf_Cte       , Inf_Cte       , Inf_Cte       , Inf_Cte       , Iniciar_Cte_Float, Inf_Cte        , Inf_Cte          , Inf_Cte       , Inf_Cte       , Inf_Cte       , Inf_Cte        , Inf_Cte       , Inf_Cte       , Inf_Cte       , Inf_Cte       , Inf_Cte       , Inf_Cte        , Inf_Cte       ,Inf_Cte},
/*E3*/ 	{Inf_Cte       , Inf_Cte       , Inf_Cte       , Inf_Cte       , Inf_Cte       , Agregar_Cte   ,  Inf_Cte       , Inf_Cte       , Inf_Cte            , Inf_Cte       , Inf_Cte       , Inf_Cte       , Inf_Cte       , Caract_No_Val    , Inf_Cte        , Inf_Cte          , Inf_Cte       , Inf_Cte       , Inf_Cte       , Inf_Cte        , Inf_Cte       , Inf_Cte       , Inf_Cte       , Inf_Cte       , Inf_Cte       , Inf_Cte        , Inf_Cte        ,Inf_Cte},
/*E4*/  {Inf_OpDivision, Inf_OpDivision, Inf_OpDivision, Inf_com       , Inf_OpDivision, Inf_OpDivision, Inf_OpDivision, Inf_OpDivision, Inf_OpDivision      ,Inf_OpDivision,  Inf_OpDivision, Inf_OpDivision, Inf_OpDivision, Inf_OpDivision   , Inf_OpDivision ,  Inf_OpDivision,  Inf_OpDivision, Inf_OpDivision, Inf_OpDivision, Inf_OpDivision, Inf_OpDivision, Inf_OpDivision,  Inf_OpDivision,   Inf_OpDivision,  Inf_OpDivision,  Inf_OpDivision, Inf_OpDivision, Inf_OpDivision },
/*E5*/ 	{Agregar_Com   , Agregar_Com   , Agregar_Com   , Agregar_Com   , Agregar_Com   , Agregar_Com   ,  Agregar_Com   , Agregar_Com   , Agregar_Com        , Agregar_Com   , Agregar_Com   , Agregar_Com   , Agregar_Com   , Agregar_Com      , Agregar_Com    , Agregar_Com      , Agregar_Com   , Agregar_Com   , Agregar_Com   , Agregar_Com    , Agregar_Com   , Agregar_Com   , Agregar_Com   , Agregar_Com   , Agregar_Com   , Agregar_Com    , Agregar_Com    ,Agregar_Com},
/*E6*/ 	{Agregar_Com   , Fin_Com       , Agregar_Com   , Agregar_Com   , Agregar_Com   , Agregar_Com   ,  Agregar_Com   , Agregar_Com   , Agregar_Com        , Agregar_Com   , Agregar_Com   , Agregar_Com   , Agregar_Com   , Agregar_Com      , Agregar_Com    , Agregar_Com      , Agregar_Com   , Agregar_Com   , Agregar_Com   , Agregar_Com    , Agregar_Com   , Agregar_Com   , Agregar_Com   , Agregar_Com   , Agregar_Com   , Agregar_Com    , Agregar_Com    ,Agregar_Com},
/*E7*/ 	{Inf_OpConcat  , Inf_OpSuma    , Inf_OpSuma    , Inf_OpSuma    , Inf_OpSuma    , Inf_OpSuma    ,  Inf_OpSuma    , Inf_OpSuma    , Inf_OpSuma         , Inf_OpSuma    , Inf_OpSuma    , Inf_OpSuma    , Inf_OpSuma    , Inf_OpSuma       , Inf_OpSuma     , Inf_OpSuma       , Inf_OpSuma    , Inf_OpSuma    , Inf_OpSuma    , Inf_OpSuma     , Inf_OpSuma    , Inf_OpSuma    , Inf_OpSuma    , Inf_OpSuma    , Inf_OpSuma    , Inf_OpSuma     , Inf_OpSuma     ,Inf_OpSuma},
/*E8*/ 	{Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val ,  Inf_Igual     , Caract_No_Val , Caract_No_Val      , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val    , Caract_No_Val  , Caract_No_Val    , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val  , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val  , Caract_No_Val  ,Caract_No_Val},
/*E9*/ 	{Inf_Menor     , Inf_Menor     , Inf_Menor     , Inf_Menor     , Inf_Menor     , Inf_Menor     ,  Inf_MenorIgual, Inf_Menor     , Inf_Menor       , Inf_Menor     , Inf_Menor     , Inf_Menor     , Inf_Menor     , Inf_Menor        , Inf_Menor      , Inf_Menor        , Inf_Menor     , Inf_Menor     , Inf_Menor     , Inf_Menor      , Inf_Menor     , Inf_Menor     , Inf_Menor     , Inf_Menor     , Inf_Menor     , Inf_Menor      , Inf_Menor      ,Inf_Menor},
/*E10*/	{Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val ,  Caract_No_Val , Caract_No_Val , Caract_No_Val      , Inf_OpAnd     , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val    , Caract_No_Val  , Caract_No_Val    , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val  , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val  , Caract_No_Val  ,Caract_No_Val},
/*E11*/	{Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val ,  Caract_No_Val , Caract_No_Val , Caract_No_Val      , Caract_No_Val , Inf_OpOr      , Caract_No_Val , Caract_No_Val , Caract_No_Val    , Caract_No_Val  , Caract_No_Val    , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val  , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val  , Caract_No_Val  ,Caract_No_Val},
/*E12*/	{Inf_Mayor     , Inf_Mayor     , Inf_Mayor     , Inf_Mayor     , Inf_Mayor     , Inf_Mayor     ,  Inf_MayorIgual, Inf_Mayor     , Inf_Mayor          , Inf_Mayor     , Inf_Mayor     , Inf_Mayor     , Inf_Mayor     , Inf_Mayor        , Inf_Mayor      , Inf_Mayor        , Inf_Mayor     , Inf_Mayor     , Inf_Mayor     , Inf_Mayor      , Inf_Mayor     , Inf_Mayor     , Inf_Mayor     , Inf_Mayor     , Inf_Mayor     , Inf_Mayor      , Inf_Mayor      ,Inf_Mayor},
/*E13*/	{Agregar_CteStr, Agregar_CteStr, Agregar_CteStr, Agregar_CteStr, Agregar_CteStr, Agregar_CteStr,  Agregar_CteStr, Agregar_CteStr, Agregar_CteStr     , Agregar_CteStr, Agregar_CteStr, Agregar_CteStr, Inf_CteStr    , Agregar_CteStr   , Agregar_CteStr , Agregar_CteStr   , Agregar_CteStr, Agregar_CteStr, Agregar_CteStr, Agregar_CteStr , Agregar_CteStr, Agregar_CteStr, Agregar_CteStr, Agregar_CteStr, Agregar_CteStr, Caract_No_Val  , Caract_No_Val  ,Agregar_CteStr},
/*E14*/	{Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val ,  Caract_No_Val , Caract_No_Val , Caract_No_Val      , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val    , Caract_No_Val  , Caract_No_Val    , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val  , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val  , Caract_No_Val  ,Caract_No_Val},
/*E15*/	{Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val ,  Caract_No_Val , Caract_No_Val , Caract_No_Val      , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val    , Caract_No_Val  , Caract_No_Val    , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val  , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val  , Caract_No_Val  ,Caract_No_Val},
/*E16*/ {Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val ,  Inf_asig    , Caract_No_Val   , Caract_No_Val      , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val    , Caract_No_Val  , Caract_No_Val    , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val  , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val  , Caract_No_Val  ,Caract_No_Val},
/*E17*/ {Inf_not       , Inf_not       ,  Inf_not      , Inf_not       , Inf_not       , Inf_not       , Inf_distinto , Inf_not         , Inf_not            ,  Inf_not      , Inf_not       , Inf_not        , Inf_not       , Inf_not         , Inf_not        , Inf_not          ,  Inf_not      , Inf_not       ,  Inf_not      , Inf_not        , Inf_not       , Inf_not       , Inf_not       , Inf_not       , Inf_not       , Inf_not        , Inf_not        , Inf_not},
/*E18*/	{Caract_No_Val , Caract_No_Val , Caract_No_Val ,Caract_No_Val  , Agregar_ID    , Caract_No_Val ,  Caract_No_Val , Caract_No_Val , Caract_No_Val      , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val    ,Caract_No_Val   , Caract_No_Val    , Caract_No_Val , Caract_No_Val , Caract_No_Val , Caract_No_Val  ,Caract_No_Val  ,Caract_No_Val  , Caract_No_Val , Caract_No_Val ,Caract_No_Val  , Caract_No_Val  , Caract_No_Val  ,Caract_No_Val},
/*E19*/	{Inf_ID        , Inf_ID        , Inf_ID        , Inf_ID        , Agregar_ID    , Agregar_ID    ,  Inf_ID        , Inf_ID        , Inf_ID             , Inf_ID        , Inf_ID        , Inf_ID        , Inf_ID        , Inf_ID           , Inf_ID         , Inf_ID           , Inf_ID        , Inf_ID        , Inf_ID        , Inf_ID         , Inf_ID        , Inf_ID        , Inf_ID        , Inf_ID        , Inf_ID        , Inf_ID         , Inf_ID         ,Inf_ID},
};

//MATRIZ ESTADOS
static int nEstado[20][28] ={
//        0 |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18 |  19  | 20  | 21  | 22  | 23  | 24  |  25 |  26 |  27 |
//        + |  /  |  *  |  #  | Let | Dig |  =  |  <  |  >  |  &  |  |  |  !  |  "  |  .  |  [  |  ]  |  {  |  }  | (  |   )  |  ,  |  _  |  ;  | tab | blan|  ent|   : |   - |
/*E0*/ {   7,    4,   15,   15,    1,    2,    8,    9,   12,   10,   11,   17,   13,    3,   15,   15,   15,   15,   15,   15,   15,   15,   15,    0,    0,    0,   16 , 15},
/*E1*/ {  15,   15,   15,   15,    1,    1,   15,   15,   15,   15,   15,   15,   15,   18,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15 , 15},
/*E2*/ {  15,   15,   15,   15,   15,    2,   15,   15,   15,   15,   15,   15,   15,    3,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15 , 15},
/*E3*/ {  15,   15,   15,   15,   15,    3,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15 , 15},
/*E4*/ {  15,   15,   15,    5,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15 , 15},
/*E5*/ {   5,    5,    5,    6,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5 ,  5},
/*E6*/ {   5,    0,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5 ,  5},
/*E7*/ {  15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15 , 15},
/*E8*/ {  15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15 , 15},
/*E9*/ {  15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15 , 15},
/*E10*/{  14,   14,   14,   14,   14,   14,   14,   14,   14,   15,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14 , 14},
/*E11*/{  14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   15,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14 , 14},
/*E12*/{  15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15 , 15},
/*E13*/{  13,   13,   13,   13,   13,   13,   13,   13,   13,   13,   13,   13,   15,   13,   13,   13,   13,   13,   13,   13,   13,   13,   13,   13,   13,   13,   13 , 13},
/*E14*/{  14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14 , 14},
/*E15*/{  14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14 , 14}, //ESTADO FINAL
/*E16*/{  15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15 , 15}, //FILA AGREGADA PARA EL STADO INTERMEDIO DEL : PARA LA ASIG
/*E17*/{  15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15 , 15},
/*E18*/{  15,   15,   15,   15,   19,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15 , 15},
/*E19*/{  15,   15,   15,   15,   19,   19,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,   15 , 15}
};

/* -------------------------------------------------------------------------- */
/* limpiarToken(): Limpia buffer de tokens                                    */
/* -------------------------------------------------------------------------- */
void limpiarToken(){
	int i;
	for (i=0; i<=MAXLONG; i++)
   	token[i] = '\0';
}

/* -------------------------------------------------------------------------- */
/* leerCaracter(): Lee un caracter del archivo                                */
/* -------------------------------------------------------------------------- */
void leerCaracter(){
	caracter = fgetc(input);
}

/* -------------------------------------------------------------------------- */
/* mostrarToken(): Muestra el token por pantalla y lo guarda en tabla tokens  */
/* -------------------------------------------------------------------------- */
void mostrarToken(){
   switch(NroToken){
        case ID:
             printf("< ID       : %s >\n", token);
           //  fprintf(tokens, "< ID       : %s >\n", token);
             break;
        case PR_MAIN:
             printf("< PR MAIN  : %s >\n", token);
            // fprintf(tokens, "< PR MAIN       : %s >\n", token);
             break;
        case PR_DECLARE:
             printf("< PR DECLARE   : %s >\n", token);
          //   fprintf(tokens, "< PR DECLARE       : %s >\n", token);
             break;
        case PR_ENDDECLARE:
             printf("< PR ENDDECLARE: %s >\n", token);
         //    fprintf(tokens, "< PR ENDDECLARE      : %s >\n", token);
             break;
        case PR_IF:
             printf("< PR IF    : %s >\n", token);
         //    fprintf(tokens, "< PR IF       : %s >\n", token);
             break;
        case PR_THEN:
             printf("< PR THEN  : %s >\n", token);
         //    fprintf(tokens, "< PR THEN       : %s >\n", token);
             break;
        case PR_ELSE:
             printf("< PR ELSE  : %s >\n", token);
         //    fprintf(tokens, "< PR ELSE      : %s >\n", token);
             break;
        case PR_FI:
             printf("< PR FI    : %s >\n", token);
         //    fprintf(tokens, "< PR FI       : %s >\n", token);
             break;
        case PR_FOR:
             printf("< PR FOR   : %s >\n", token);
         //    fprintf(tokens, "< PR FOR       : %s >\n", token);
             break;
        case PR_TO:
             printf("< PR TO    : %s >\n", token);
         //    fprintf(tokens, "< PR TO       : %s >\n", token);
             break;
        case PR_STEP:
             printf("< PR STEP  : %s >\n", token);
         //    fprintf(tokens, "< PR STEP       : %s >\n", token);
             break;
       /*
        case PR_ROF:
             printf("< PR ROF   : %s >\n", token);
             fprintf(tokens, "< PR ROF       : %s >\n", token);
             break;
             */
        case PR_REPEAT:
             printf("< PR REPEAT    : %s >\n", token);
            // fprintf(tokens, "< PR REPEAT       : %s >\n", token);
             break;
        case PR_UNTIL:
             printf("< PR UNTIL : %s >\n", token);
           //  fprintf(tokens, "< PR UNTIL       : %s >\n", token);
             break;
        case PR_GET:
             printf("< PR GET: %s >\n", token);
            // fprintf(tokens, "< PR GET       : %s >\n", token);
             break;
        case PR_PUT:
             printf("< PR PUT: %s >\n", token);
           //  fprintf(tokens, "< PR PUT       : %s >\n", token);
             break;
        case PR_STRING:
             printf("< PR STRING : %s >\n", token);
          //   fprintf(tokens, "< PR STRING       : %s >\n", token);
             break;
        case PR_INTEGER:
             printf("< PR INTEGER : %s >\n", token);
          //   fprintf(tokens, "< PR INTEGER       : %s >\n", token);
             break;
        case PR_FLOAT:
             printf("< PR FLOAT : %s >\n", token);
          //   fprintf(tokens, "< PR FLOAT       : %s >\n", token);
             break;
        case CTE_ENT:
             printf("< CTE ENT  : %s >\n", token);
            // fprintf(tokens, "< CTE ENT      : %s >\n", token);
             break;
        case CTE_REAL:
             printf("< CTE REAL : %s >\n", token);
          //   fprintf(tokens, "< CTE REAL       : %s >\n", token);
             break;
        case CTE_STRING:
             printf("< CTE STRING  : %s >\n", token);
         //    fprintf(tokens, "< CTE STRING      : %s >\n", token);
             break;
        case OP_PABRE:
             printf("< ABRE PARENTESIS : %s >\n", token);
         //    fprintf(tokens, "< ABRE PARENTESIS       : %s >\n", token);
             break;
        case OP_PCIERRA:
             printf("< CIERRA PARENTESIS: %s >\n", token);
         //    fprintf(tokens, "< CIERRA PARENTESIS       : %s >\n", token);
             break;
        case OP_OR:
             printf("< OP_OR    : %s >\n", token);
         //    fprintf(tokens, "< OP_OR       : %s >\n", token);
             break;
        case OP_AND:
             printf("< OP_AND   : %s >\n", token);
         //    fprintf(tokens, "< OP_AND       : %s >\n", token);
             break;
        case OP_NOT:
             printf("< OP_NOT   : %s >\n", token);
            // fprintf(tokens, "< OP_NOT       : %s >\n", token);
             break;
        case OP_MENOR:
             printf("< OP_MENOR : %s >\n", token);
           //  fprintf(tokens, "< OP_MENOR      : %s >\n", token);
             break;
        case OP_MENORIGUAL:
             printf("< OP_MENOR_IGUAL: %s >\n", token);
          //   fprintf(tokens, "< OP_MENOR_IGUAL       : %s >\n", token);
             break;
        case OP_MAYOR:
             printf("< OP_MAYOR : %s >\n", token);
          //   fprintf(tokens, "< OP_MAYOR       : %s >\n", token);
             break;
        case OP_MAYORIGUAL:
             printf("< OP_MAYOR_IGUAL: %s >\n", token);
           //  fprintf(tokens, "< OP_MAYOR_IGUAL       : %s >\n", token);
             break;
        case OP_IGUAL:
             printf("< OP_IGUAL_IGUAL  : %s >\n", token);
          //   fprintf(tokens, "< OP_IGUAL_IGUAL       : %s >\n", token);
             break;
        case OP_DISTINTO:
             printf("< OP_DISTINTO: %s >\n", token);
          //   fprintf(tokens, "< OP_DISTINTO       : %s >\n", token);
             break;
        case OP_SUMA:
             printf("< OP_SUMA  : %s >\n", token);
          //  fprintf(tokens, "< OP_SUMA       : %s >\n", token);
             break;
        case OP_RESTA:
             printf("< OP_RESTA : %s >\n", token);
            // fprintf(tokens, "< OP_RESTA       : %s >\n", token);
             break;
        case OP_MULTIPLI:
             printf("< OP_MULTIPLICACION : %s >\n", token);
          //   fprintf(tokens, "< OP_MULTIPLICACION       : %s >\n", token);
             break;
        case OP_DIVISION:
             printf("< OP_DIVISION   : %s >\n", token);
          //   fprintf(tokens, "< OP_DIVISION       : %s >\n", token);
             break;
        case OP_CABRE:
             printf("< CORCHETE ABRE : %s >\n", token);
          //   fprintf(tokens, "< CORCHETE ABRE     : %s >\n", token);
             break;
        case OP_CCIERRA:
             printf("< CORCHETE CIERRA: %s >\n", token);
          //   fprintf(tokens, "< CORCHETE CIERRA       : %s >\n", token);
             break;
        case OP_ASIG:
		     printf("< OP_ASIG  : %s >\n", token);
         //    fprintf(tokens, "< OP_ASIG       : %s >\n", token);
             break;
        case OP_LLABRE:
             printf("< LLAVE ABRE : %s >\n", token);
            // fprintf(tokens, "< LLAVE ABRE       : %s >\n", token);
             break;
        case OP_LLCIERRA:
             printf("< LLAVE CIERRA: %s >\n", token);
            // fprintf(tokens, "< LLAVE CIERRA       : %s >\n", token);
             break;
        case OP_CONCAT:
             printf("< OP_CONCAT: %s >\n", token);
             //fprintf(tokens, "< OP_CONCAT       : %s >\n", token);
             break;
        case SEP_SENT:
		     printf("< SEPARADOR_SENTENCIAS: %s >\n", token);
           //  fprintf(tokens, "< SEPARADOR_SENTENCIAS       : %s >\n", token);
             break;
        case SEP_LISTA:
             printf("< COMA: %s >\n", token);
             //fprintf(tokens, "< COMA       : %s >\n", token);
             break;
        /*
        case OP_GUION:
             printf("< OP_GUION : %s >\n", token);
             fprintf(tokens, "< OP_GUION       : %s >\n", token);
             break;
             */
        case PR_STRUCT:
             printf("< PR_STRUCT : %s >\n", token);
           //  fprintf(tokens, "< PR STRUCT       : %s >\n", token);
             break;
        case PR_NEXT:
             printf("< PR_NEXT : %s >\n", token);
            // fprintf(tokens, "< PR_NEXT : %s >\n", token);
             break;
        case PR_CONST:
             printf("< PR_CONST : %s >\n", token);
            // fprintf(tokens, "< PR_CONST : %s >\n", token);
             break;
     }
}

/* -------------------------------------------------------------------------- */
/* leerColumna(): Retorna la columna que corresponde al caracter ingresado    */
/* -------------------------------------------------------------------------- */
int leerColumna(char caracter){
	//LETRAS
	if(caracter <= 'z' && caracter >= 'a' )
		return 4;
	if (caracter <= 'Z' && caracter >= 'A')
	  	return 4;

	//DIGITOS
	if(caracter >= '0' && caracter <= '9')
	  return 5;

	//OTROS CARACTERES
    switch(caracter){
		case '"':
		    return 12;
		    break;
		case '=':
		    return 6;
		    break;
		case '<':
		    return 7;
		    break;
		case '>':
		    return 8;
		    break;
		case '!':
		    return 11;
		    break;
		case '|':
		    return 10;
		    break;
		case '&':
		    return 9;
		    break;
		case '+':
		    return 0;
		    break;
		case '/':
		    return 1;
		    break;
		case '*':
		    return 2;
		    break;
		case '#':
		    return 3;
		    break;
		case '(':
		   return 18;
		   break;
		case ')':
		   return 19;
		   break;
		case '[':
			return 14;
		   break;
		case ']':
		   return 15;
		   break;
		case '{':
			return 16;
		   break;
		case '}':
		   return 17;
		   break;
		case ';':
		   return 22;
		   break;
		case ',':
		   return 20;
		   break;
		case '.':
		   return 13;
		   break;
		case '\n':
			return 25;
		   break;
		case '\t':
		   return 23;
		   break;
		case ' ':
			return 24;
		   break;
		case '\0':
		   return 24;
		   break;
        case ':':
            return 26;
            break;
        case '_':
            return 21;
            break;
        case '-':
            return 27;
            break;
        case EOF:
           return EOF;
		   break;
	}

	//Cualquier otro caracter es ignorado
	if ( cteStrAbierta ==0 && comAbierto ==0){
        printf("\n ERROR LEXICO: Caracter no reconocido: '%c'. \n", caracter);
        getch();
        exit(1);
	}

}

/* -------------------------------------------------------------------------- */
/* buscarEnTOS(): Busca en la TOS un simbolo por el indice                    */
/* -------------------------------------------------------------------------- */
char * buscarEnTOS(int index){
    return TOS[index].nombre;
}

/* -------------------------------------------------------------------------- */
/* insertarTOS(): Inserta en la TOS o si ya existe devuelve la posicion       */
/* -------------------------------------------------------------------------- */
int insertarTOS(){
	int i,j,x=0;
    int ii=0;
    char aux[100];
    char auxStr[100];

    if (NroToken==CTE_STRING){
        strcpy(auxStr," ");

        for (j=0;j< strlen(token);j++){
            if(token[j]!='"'){
                auxStr[x]= token[j];
                x++;
            }
        }

        auxStr[strlen(token)-1]='\0';
    }


    for (i=0; i<TOStop;  i++)
    {
        if (NroToken==ID)
        {
            if (strcmp(TOS[i].nombre,token)==0)
                return i;
        }
        else if (NroToken==CTE_STRING)
        {
            if (strcmp(TOS[i].valor,auxStr)==0)
                return i;
        }
        else
        {
            if (strcmp(TOS[i].valor,token)==0)
                return i;
        }
    }

  	switch (NroToken){
        case ID:
            strcat(aux, token);
            strcpy(TOS[TOStop].nombre,token);
			switch(tipoDato){
                case 1: strcpy(TOS[TOStop].tipo,"INTEGER" ); break;
                case 2: strcpy(TOS[TOStop].tipo,"FLOAT" ); break;
                case 3: strcpy(TOS[TOStop].tipo,"STRING" ); break;
                case 4: strcpy(TOS[TOStop].tipo,"STRUCT" ); break;
				case 5: strcpy(TOS[TOStop].tipo,"CONST" ); break;
            }
            TOStop++;
        break;
        case CTE_ENT:
            strcpy(aux,"_");
            strcat(aux, token);
            strcpy(TOS[TOStop].nombre, aux);
            strcpy(TOS[TOStop].tipo,"CTE_ENT");
            strcpy(TOS[TOStop].valor, token);
   			TOStop++;
		break;
        case CTE_REAL:
            strcpy(aux,"_");
            strcat(aux, token);
            strcpy(TOS[TOStop].nombre,aux);
            strcpy(TOS[TOStop].tipo,"CTE_REAL");
            strcpy(TOS[TOStop].valor, token);
   			TOStop++;
		break;
       	case CTE_STRING:
            strcpy(aux,"_");
            strcat(aux, auxStr);
            strcpy(TOS[TOStop].nombre, aux);
            strcpy(TOS[TOStop].tipo,"CTE_STRING" );
            strcpy(TOS[TOStop].valor, auxStr);
            TOS[TOStop].longitud = (strlen(auxStr));
            TOStop++;
        break;
    }

    return TOStop-1;
}

void mostrarTOS(){
    int i;

    //printf("\n------------------------------ TABLA DE  SIMBOLOS ------------------------------\n");
    fprintf(tos,"TABLA DE  SIMBOLOS:\n");

    //printf ("Nro\t | Nombre\t\t\t | Tipo\t | Valor\n");
    fprintf(tos,"Nro\t | Nombre\t\t\t | Tipo\t | Valor\t | Longitud \n");
    for (i=21; i<TOStop; i++)
        fprintf(tos,"%d     \t | %s     \t\t\t | %s     \t | %s \t | %d \n",i-21,TOS[i].nombre, TOS[i].tipo, TOS[i].valor, TOS[i].longitud);

}

/* -------------------------------------------------------------------------- */
/* esPalabraReservada(): Varifica si un ID es una palabra reservada           */
/* -------------------------------------------------------------------------- */
int esPalabraReservada(){
    int i;
    for (i=0; i<CANTPR; i++){
        if (strcmpi(TOS[i].nombre,token)==0){
             if (strcmpi("INTEGER",token)==0)
                 tipoDato = 1;
             else {
                if (strcmpi("FLOAT",token)==0)
                    tipoDato = 2;
                else {
                    if (strcmpi("STRING",token)==0)
                        tipoDato = 3;
                    else {
                        if (strcmpi("STRUCT",token)==0)
                        tipoDato = 4;

						else{
                                if(strcmpi("CONST",token)==0)
                                tipoDato = 5;
                                }
                    }
                }
             }
            return ++i;
		}
    }

    return 0;
}

/* -------------------------------------------------------------------------- */
/* agregarPalabrasReservadas(): Agrega las Palabras reservadas a la Tabla de S*/
/* -------------------------------------------------------------------------- */
void agregarPalabrasReservadas(){
    TOStop=0;
    strcpy(TOS[TOStop].nombre, "MAIN");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "DECLARE");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "ENDDECLARE");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "IF");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "THEN");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "ELSE");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "FI");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "FOR");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "TO");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "STEP");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "ROF");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "REPEAT");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "UNTIL");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "GET");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "PUT");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "INTEGER");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "FLOAT");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "STRING");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "STRUCT");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "NEXT");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;

    strcpy(TOS[TOStop].nombre, "CONST");
    strcpy(TOS[TOStop].tipo, "PR");
    TOStop++;
}

void Validaciones(){
    if (comAbierto != 0){
        printf("\n ERROR LEXICO: COMENTARIO abierto. \n");
        getch();
        error=1;
    }

    if (cteStrAbierta !=0){
        printf("\n ERROR LEXICO: Falta una comilla para cerrar una CTE STRING. \n");
        getch();
        error=1;
    }

    if ( (strcmp(token,"&")==0) || (strcmp(token,"|")==0) ){
        printf("\n ERROR: Se esperaba %s \n", token);
        getch();
        exit(1);
    }

    if ( NroToken==CTE_ENT || NroToken==CTE_REAL)
        Inf_Cte();

}

/* -------------------------------------------------------------------------- */
/*                               FUNCION YYLEX                                */
/* -------------------------------------------------------------------------- */
int yylex(){ //FUNCION  QUE LEE HASTA FINAL DE TOKEN O EOF
	int estado=0;
	int columna;

    leerCaracter();

    if (caracter== EOF){
        NroToken=EOF;
        return NroToken;
    }

	while(estado != ESTADO_FINAL){
        columna = leerColumna( caracter);

        //Ejecuto la funcion de la matriz
		(*proceso[estado][columna])();

        //Luego cambio de estado
		estado = nEstado[estado][columna];

		if (estado != ESTADO_FINAL){
		    leerCaracter();

		    if (caracter== EOF){
                Validaciones();

                if (NroToken==ID && error ==0)
                    Inf_ID();

                if (estado ==0)
                    return EOF;

                return NroToken;
		    }
		}
    }

    return NroToken;
}


/* -------------------------------------------------------------------------- */
/*                               FUNCION YYERROR                              */
/* -------------------------------------------------------------------------- */
int yyerror(char *msg){
	  printf("\n%s\nPresione una tecla para continuar...", msg);
	  getch();
}


//GENERACION ASSEMBLER

/* Graba la TS a un archvo binario, que sera usado en todas las etapas
  de la compilacion*/
void GrabaArchivoBinarioTs(){
     FILE * tsBin = openFile("TS_bin.txt","wb");
     int k;
     for(k=0;k<TOStop;k++)
         fwrite(&TOS[k],sizeof(tablaDeSimbolo),1,tsBin);
     fclose(tsBin);
}

void limpiaAux(tablaDeSimbolo *aux){
	strcpy(aux->nombre,"");
	strcpy(aux->tipo,"");
	strcpy(aux->valor,"");
	aux->longitud = 0;
	strcpy(aux->ren,"");
}

FILE* openFile(char *fileName, char *mode){
	FILE* file = fopen(fileName, mode);
	if (file == NULL)
	{
	    printf( "No se puede abrir el archivo %s.\n", fileName);
	    exit( 1 );
	}
	return file;
}


/* -------------------------------------------------------------------------- */
/*                        FUNCIONES GENERACION CODIGO                         */
/* -------------------------------------------------------------------------- */

#include "pila.h"
#include "terceto.h"
#include "ttipos.h"
#include "assembler.h"

int crearPilas(){

	if ((crearPila(&pilaPos)) == -1)
	{
		printf("error al crear la pila \n");
    	system("PAUSE");
  		return -1;
	}
	if ((crearPila(&pilaExp)) == -1)
	{
		printf("error al crear la pila \n");
    	system("PAUSE");
  		return -1;
	}
	if ((crearPila(&pilaCte)) == -1)
	{
		printf("error al crear la pila \n");
    	system("PAUSE");
  		return -1;
	}
	if ((crearPila(&pilaCond)) == -1)
	{
		printf("error al crear la pila \n");
    	system("PAUSE");
  		return -1;
	}
	if ((crearPila(&pilaUntilRepeat)) == -1)
	{
		printf("error al crear la pila \n");
    	system("PAUSE");
  		return -1;
	}
	if ((crearPila(&pilaFor)) == -1)
	{
		printf("error al crear la pila \n");
    	system("PAUSE");
  		return -1;
	}
	if ((crearPila(&pilaForMarca)) == -1)
	{
		printf("error al crear la pila \n");
    	system("PAUSE");
  		return -1;
	}
   return 0;
}

void crearTercetoExStr(int oper1, int oper2){
	Ind_ops=crearTerceto(TIPO_TERCETO_1,"",oper1,-1);
	Ind_ops_tipo=buscarTipo(oper1);

	Ind_aux_eps=Ind_ops;
	Ind_aux_eps_tipo=Ind_ops_tipo;

	Ind_ops=crearTerceto(TIPO_TERCETO_1,"",oper2,-1);
	Ind_ops_tipo=buscarTipo(oper2);

	Ind_exps=crearTerceto(TIPO_TERCETO_4,"ADD", Ind_aux_eps,Ind_ops);
	agregarTipoATerceto(Ind_exps,verificarTipo(' ',Ind_aux_eps_tipo,Ind_ops_tipo));
	Ind_exps_tipo=ts_Operacion[Ind_aux_eps_tipo][Ind_ops_tipo];

	Ind_aux_eps=0;
	Ind_aux_eps_tipo=0;
}

int insertarCfTOS(int ind){
	char indFc[50];
	sprintf(indFc,"CF_%d",ind);
   strcpy(TOS[TOStop].nombre,indFc);
   strcpy(TOS[TOStop].tipo,"INTEGER" );
   strcpy(TOS[TOStop].valor,"0" );
   TOStop++;
	return (TOStop-1);
}

void crearTercetoContador(){
	if (!IsContadorCreated){
		Ind_cont_fil= insertarCfTOS(cont_fil); //Crea el contador en la TOS
		Ind_condf=crearTerceto(TIPO_TERCETO_6, "=", Ind_cont_fil, 0);
		IsContadorCreated = true;
		agregarTipoATerceto(Ind_condf, TIPO_ENTERO);
	}
}

void crearTercetoCondicion (int exp){
	if(Tipo_condf==1)
	{
		// Comparacion simple
		Ind_expf=crearTerceto(TIPO_TERCETO_4,"CMP", exp, recuperarTerceto(Li_aux));
		Ind_expf=crearTerceto(TIPO_TERCETO_4,opsalto,Ind_expf, Ind_expf+4);
		Ind_expf=crearTerceto(TIPO_TERCETO_6,"ADD", Ind_cont_fil,1);
		agregarTipoATerceto(Ind_expf, TIPO_ENTERO);
		Ind_cont = Ind_expf;
		Ind_expf=crearTerceto(TIPO_TERCETO_3,"=", Ind_cont_fil, Ind_cont);
	}
	else if (Tipo_condf==2)
	{
		// Comparacion simple negada
		Ind_expf=crearTerceto(TIPO_TERCETO_4,"CMP", exp, recuperarTerceto(Li_aux));
		Ind_expf=crearTerceto(TIPO_TERCETO_4,negarSalto(opsalto),Ind_expf, Ind_expf+4);
		Ind_expf=crearTerceto(TIPO_TERCETO_6,"ADD", Ind_cont_fil,1);
		agregarTipoATerceto(Ind_expf, TIPO_ENTERO);
		Ind_cont = Ind_expf;
		Ind_expf=crearTerceto(TIPO_TERCETO_3,"=", Ind_cont_fil, Ind_cont);

	}
	else if (Tipo_condf==3)
	{
		// Comparacion compuesta
		Ind_expf=crearTerceto(TIPO_TERCETO_4,"CMP", exp, recuperarTerceto(Li_aux)); //Ind_compLi=crearTerceto(TIPO_TERCETO_4,"CMP", exp, Li_aux);
		Ind_cond=crearTerceto(TIPO_TERCETO_4,opsalto_filter_Li,Ind_expf+2,-1);	//Ind_cond=crearTerceto(TIPO_TERCETO_4,opsalto_filter_Li,Ind_compLi+6,-1);
		Ind_logicoFilter_Li = Ind_cond;
		Ind_expf=crearTerceto(TIPO_TERCETO_4,"CMP", recuperarTerceto(Ld_aux)-1, recuperarTerceto(exp)+1); //Ind_compLd=crearTerceto(TIPO_TERCETO_4,"CMP", Ind_expf_aux, Ind_expf);
		Ind_cond=crearTerceto(TIPO_TERCETO_4,opsalto_filter_Ld,Ind_expf+2,-1); //Ind_cond=crearTerceto(TIPO_TERCETO_4,opsalto_filter_Ld,Ind_compLd+6,-1);
		Ind_logicoFilter_Ld = Ind_cond;
		Ind_expf=crearTerceto(TIPO_TERCETO_4,oplog,Ind_logicoFilter_Li,Ind_logicoFilter_Ld); //Ind_expf=crearTerceto(TIPO_TERCETO_4,oplog,Ind_compLi+1,Ind_compLd+1);
		Ind_expf=crearTerceto(TIPO_TERCETO_4,"JZ",-1,Ind_expf+4);

		Ind_expf=crearTerceto(TIPO_TERCETO_6,"ADD", Ind_cont_fil,1);
		agregarTipoATerceto(Ind_expf, TIPO_ENTERO);
		Ind_expf=crearTerceto(TIPO_TERCETO_3,"=",Ind_cont_fil, Ind_expf);
	}
	else if (Tipo_condf==4)
	{
		// Comparacion compuesta negada
		Ind_expf=crearTerceto(TIPO_TERCETO_4,"CMP", exp, recuperarTerceto(Li_aux));
		Ind_cond=crearTerceto(TIPO_TERCETO_4,opsalto_filter_Li,Ind_expf+2,-1);
		Ind_logicoFilter_Li = Ind_cond;

		Ind_expf=crearTerceto(TIPO_TERCETO_4,"CMP", recuperarTerceto(Ld_aux)-1, recuperarTerceto(exp)+1);
		Ind_cond=crearTerceto(TIPO_TERCETO_4,opsalto_filter_Ld,Ind_expf+2,-1);
		Ind_logicoFilter_Ld = Ind_cond;

		Ind_expf=crearTerceto(TIPO_TERCETO_4,oplog,Ind_logicoFilter_Li,Ind_logicoFilter_Ld);
		Ind_expf=crearTerceto(TIPO_TERCETO_4,negarSalto("JZ"),-1,Ind_expf+4); //salto negado para el not

		Ind_expf=crearTerceto(TIPO_TERCETO_6,"ADD", Ind_cont_fil,1);
		agregarTipoATerceto(Ind_expf, TIPO_ENTERO);
		Ind_expf=crearTerceto(TIPO_TERCETO_3,"=",Ind_cont_fil, Ind_expf);
	}
}



/* -------------------------------------------------------------------------- */
/*                                      MAIN                                  */
/* -------------------------------------------------------------------------- */
int main(int argc, char* argv[]){
    agregarPalabrasReservadas();
	remove("Intermedia.txt");

	//GENERACION CODIGO  ------------------------------------------------------//
	cargarTablaSintesis(); //Para Combinaciones de tipos de datos
	crearPilas();			  //Para Estructuras de control

     if ((input = fopen ("prueba.txt","r"))== NULL){
            printf("No se puede abrir el archivo de entrada");
            getch();
            exit(1);
     }
 	// ANALIZADOR LEXICO ------------------------------------------------------//
    /*
    if ((tokens = fopen ("tokens.txt","w"))== NULL){
		printf("No se puede crear el archivo de tokens");
		getch();
		exit(1);
	}

	while (feof(input)== 0 ){
        if (yylex()!=EOF)
        {
            if (error==0)
                mostrarToken();
        }
	}

    if(fclose(input)!=0){
        printf("No se puede CERRAR el archivo de entrada");
        getch();
        exit(1);
    }

    if(fclose(tokens)!=0){
        printf("No se puede CERRAR el archivo de tokens");
        getch();
        exit(1);
    }

    if (error==0){
        printf("\n - ANALISIS LEXICO FINALIZADO CORRECTAMENTE - \n");
        getch();
        if ((tos = fopen ("tos.txt","w"))== NULL){
            printf("No se puede crear el archivo de la tabla de TOS");
            getch();
            exit(1);
        }

        mostrarTOS();

        if(fclose(tos)!=0){
            printf("No se puede CERRAR el archivo de la tabla de simbolos");
            getch();
            exit(1);
        }
    }

    if(error==1){
        printf("\n - ANALISIS LEXICO FINALIZADO INCORRECTAMENTE - \n");
        getch();
    }
    */
	// ANALIZADOR LEXICO ------------------------------------------------------//
    // ANALIZADOR SINTACTICO --------------------------------------------------//
   yyparse();

	if (error==0)
	{
	  printf("\n - Analisis CORRECTO - \n");

	  if ((tos = fopen ("tos.txt","w"))== NULL)
	  {
	      printf("No se puede crear el archivo de la tabla de TOS");
	      getch();
	      exit(1);
	  }

	  mostrarTOS();

	  if(fclose(tos)!=0)
	  {
	      printf("No se puede CERRAR el archivo de la tabla de tos");
	      getch();
	      exit(1);
	  }
	}
	else if(error==1)
   	printf("\n - Analisis Lexico completo con ERRORES - \n");

	remove("TSAssembler_bin.txt");
	remove("TS_bin.txt");
	remove("intermediaAssembler.dat");
	remove("Intermedia.dat");

	return 0;
}
FILE *yytfilep;
char *yytfilen;
int yytflag = 0;
int svdprd[2];
char svdnams[2][2];

int yyexca[] = {
  -1, 1,
  0, -1,
  -2, 0,
  -1, 60,
  282, 59,
  283, 59,
  -2, 55,
  -1, 83,
  282, 59,
  283, 59,
  -2, 56,
  0,
};

#define YYNPROD 75
#define YYLAST 314

int yyact[] = {
      20,     127,      15,      16,      17,      13,      29,      30,
      30,     132,      81,      57,      53,      15,      16,      17,
      13,      44,      51,     103,      35,      46,     128,     102,
      28,      45,     100,      47,      48,      15,      16,      17,
      52,      33,      14,      75,      44,      97,      98,     135,
      46,      31,      95,      96,      45,      14,      47,      48,
      15,      16,      17,      27,      33,      87,      86,      19,
      80,      89,      90,      91,      92,      93,      94,     101,
      95,      96,      67,      68,      69,      44,     139,      18,
     138,      46,      61,      55,       6,      45,     115,      47,
      48,      15,      16,      17,      44,      33,     148,     122,
      46,      67,      68,      69,      45,      50,      47,      48,
      15,      16,      17,      44,      33,     120,     118,      46,
     121,     119,     146,      45,     114,      47,      48,      15,
      16,      17,      44,      33,      73,     137,      46,      74,
     129,      72,      45,      71,      47,      48,      15,      16,
      17,      23,      33,      78,      68,      69,      79,      15,
      16,      17,      49,     147,      33,      24,      25,     144,
      26,     126,     130,      82,       3,       4,       5,      37,
      64,      41,      59,      62,      22,      38,      12,      36,
      60,      12,      66,       8,     106,     108,     101,      85,
      88,      63,     142,      21,     136,     145,     141,      70,
     123,      77,      43,      42,      40,      39,     116,      32,
      34,       8,      11,      10,      56,       9,      26,       7,
      38,       2,       1,      54,       0,       0,      58,       0,
       0,      65,       0,       0,       0,      76,      56,       0,
      37,       0,       0,       0,       0,       0,      70,      22,
       0,      84,       0,       0,       0,       0,      83,       0,
       0,       0,       0,       0,       0,      88,      99,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
      62,     109,     104,       0,      85,       0,     107,       0,
       0,       0,       0,      56,     105,       0,       0,     117,
      56,       0,       0,      82,     112,     113,     126,       0,
     130,       0,     107,       0,      60,     125,     124,     128,
       0,      85,       0,      98,      66,       0,       0,     131,
      59,       0,     133,       0,      64,       0,      56,      41,
       0,     106,     134,       0,      56,       0,     136,      56,
     140,     110,     111,       0,     141,     143,       0,      36,
       0,       0,       0,       0,       0,       0,       0,       0,
      96,      65,
};

int yypact[] = {
    -109,   -1000,    -107,    -204,    -270,    -209,    -226,    -259,
   -1000,   -1000,   -1000,   -1000,    -147,    -135,    -130,   -1000,
   -1000,   -1000,    -230,    -274,   -1000,   -1000,    -296,   -1000,
    -254,    -136,    -136,    -278,    -146,   -1000,    -138,    -184,
    -284,    -265,    -290,    -146,    -224,    -291,   -1000,   -1000,
   -1000,    -147,   -1000,   -1000,    -210,   -1000,    -153,    -155,
    -160,   -1000,    -261,   -1000,    -145,   -1000,    -243,   -1000,
    -292,   -1000,    -295,    -114,   -1000,    -187,    -229,    -228,
    -249,    -256,   -1000,   -1000,   -1000,   -1000,    -146,    -271,
   -1000,   -1000,   -1000,    -217,    -249,   -1000,    -277,    -281,
   -1000,   -1000,    -146,   -1000,    -229,    -187,   -1000,   -1000,
    -187,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,    -187,
    -187,    -187,    -187,    -161,    -199,    -137,    -174,    -175,
    -176,    -187,   -1000,   -1000,   -1000,    -249,    -256,    -256,
   -1000,   -1000,    -210,    -120,    -280,    -156,   -1000,   -1000,
   -1000,   -1000,   -1000,    -116,   -1000,   -1000,    -187,    -293,
    -137,   -1000,    -146,    -227,   -1000,    -159,    -191,    -207,
    -146,   -1000,   -1000,   -1000,    -146,    -146,    -124,    -146,
    -170,    -128,   -1000,    -190,   -1000,
};

int yypgo[] = {
       0,     194,     193,     159,     191,     163,     189,     187,
     186,     153,     156,     182,     157,     151,     181,     180,
     179,     178,     152,     177,     201,     162,     154,     176,
     175,     174,     173,     172,     170,     160,     155,     167,
     164,     169,     168,     165,
};

int yyr1[] = {
       0,       1,       1,       2,       4,       4,       5,       5,
       5,       6,       7,      11,      11,       8,       8,      10,
      10,       3,       3,      13,      13,      13,      13,      13,
      13,      12,      12,      19,      19,      19,      19,      19,
      18,      18,      18,      20,      20,      20,      21,      21,
      21,       9,       9,       9,      14,      23,      14,      24,
      15,      25,      26,      15,      27,      28,      15,      22,
      22,      22,      22,      30,      32,      31,      31,      29,
      33,      35,      34,      34,      34,      34,      34,      34,
      17,      17,      16,
};

int yyr2[] = {
       2,       7,       6,       3,       1,       2,       1,       1,
       1,       3,       9,       2,       4,       4,       4,       1,
       3,       2,       3,       1,       1,       1,       2,       1,
       1,       3,       3,       3,       3,       3,       3,       1,
       3,       3,       1,       3,       3,       1,       1,       1,
       1,       1,       1,       1,       5,       0,       8,       0,
       5,       0,       0,      13,       0,       0,      11,       1,
       2,       3,       4,       1,       1,       1,       1,       3,
       1,       1,       1,       1,       1,       1,       1,       1,
       2,       2,       2,
};

int yychk[] = {
   -1000,      -1,      -2,     257,     258,     257,     280,      -4,
      -5,      -6,      -7,      -8,      -9,     275,     304,     272,
     273,     274,     280,     281,     259,      -5,     -10,     276,
     276,     272,     274,     281,     298,     302,     303,     295,
     -12,     276,     -12,     298,      -3,     -13,     -12,     -14,
     -15,      -9,     -16,     -17,     260,     268,     264,     270,
     271,     276,     277,     302,     297,     302,      -3,     299,
     -13,     302,     -10,     -22,     -29,     284,     -30,     -33,
     -18,     -20,     -21,     276,     277,     278,     -24,     276,
     276,     276,     279,     296,     -18,     -19,     276,     279,
     299,     302,     261,     -29,     -30,     -31,     283,     282,
     -34,     285,     286,     287,     288,     289,     290,     291,
     292,     293,     294,      -3,     297,     280,     300,     300,
      -3,     -31,     -32,     -29,     -35,     -18,     -20,     -20,
     -21,     -21,     269,     277,     -11,      -9,     276,     279,
     276,     279,     263,     -23,     -32,     -22,     265,     281,
     302,     276,     262,     -18,     302,      -9,      -3,     266,
     -27,     276,     263,     277,      -3,     -25,     -28,      -3,
     267,     -26,     276,     267,     276,
};

int yydef[] = {
       0,      -2,       0,       0,       0,       0,       0,       0,
       4,       6,       7,       8,       0,       0,       0,      41,
      42,      43,       0,       0,       3,       5,       0,      15,
       0,       0,       0,       0,       0,       9,       0,       0,
       0,       0,       0,       0,       0,       0,      19,      20,
      21,       0,      23,      24,       0,      47,       0,       0,
       0,      16,       0,      13,       0,      14,       0,       2,
       0,      17,      22,       0,      -2,       0,       0,       0,
      64,      34,      37,      38,      39,      40,       0,       0,
      74,      72,      73,       0,      25,      26,      38,      31,
       1,      18,       0,      -2,       0,       0,      61,      62,
       0,      66,      67,      68,      69,      70,      71,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
      45,       0,      57,      60,      63,      65,      32,      33,
      35,      36,       0,       0,       0,       0,      27,      29,
      28,      30,      44,       0,      58,      48,       0,       0,
       0,      11,       0,      52,      10,       0,       0,       0,
       0,      12,      46,      49,      53,       0,       0,      50,
       0,       0,      54,       0,      51,
};

int *yyxi;


/*****************************************************************/
/* PCYACC LALR parser driver routine -- a table driven procedure */
/* for recognizing sentences of a language defined by the        */
/* grammar that PCYACC analyzes. An LALR parsing table is then   */
/* constructed for the grammar and the skeletal parser uses the  */
/* table when performing syntactical analysis on input source    */
/* programs. The actions associated with grammar rules are       */
/* inserted into a switch statement for execution.               */
/*****************************************************************/


#ifndef YYMAXDEPTH
#define YYMAXDEPTH 200
#endif
#ifndef YYREDMAX
#define YYREDMAX 1000
#endif
#define PCYYFLAG -1000
#define WAS0ERR 0
#define WAS1ERR 1
#define WAS2ERR 2
#define WAS3ERR 3
#define yyclearin pcyytoken = -1
#define yyerrok   pcyyerrfl = 0
YYSTYPE yyv[YYMAXDEPTH];     /* value stack */
int pcyyerrct = 0;           /* error count */
int pcyyerrfl = 0;           /* error flag */
int redseq[YYREDMAX];
int redcnt = 0;
int pcyytoken = -1;          /* input token */


int yyparse()
{
  int statestack[YYMAXDEPTH]; /* state stack */
  int      j, m;              /* working index */
  YYSTYPE *yypvt;
  int      tmpstate, tmptoken, *yyps, n;
  YYSTYPE *yypv;


  tmpstate = 0;
  pcyytoken = -1;
#ifdef YYDEBUG
  tmptoken = -1;
#endif
  pcyyerrct = 0;
  pcyyerrfl = 0;
  yyps = &statestack[-1];
  yypv = &yyv[-1];


  enstack:    /* push stack */
#ifdef YYDEBUG
    printf("at state %d, next token %d\n", tmpstate, tmptoken);
#endif
    if (++yyps - &statestack[YYMAXDEPTH] > 0) {
      yyerror("pcyacc internal stack overflow");
      return(1);
    }
    *yyps = tmpstate;
    ++yypv;
    *yypv = yyval;


  newstate:
    n = yypact[tmpstate];
    if (n <= PCYYFLAG) goto defaultact; /*  a simple state */


    if (pcyytoken < 0) if ((pcyytoken=yylex()) < 0) pcyytoken = 0;
    if ((n += pcyytoken) < 0 || n >= YYLAST) goto defaultact;


    if (yychk[n=yyact[n]] == pcyytoken) { /* a shift */
#ifdef YYDEBUG
      tmptoken  = pcyytoken;
#endif
      pcyytoken = -1;
      yyval = yylval;
      tmpstate = n;
      if (pcyyerrfl > 0) --pcyyerrfl;
      goto enstack;
    }


  defaultact:


    if ((n=yydef[tmpstate]) == -2) {
      if (pcyytoken < 0) if ((pcyytoken=yylex())<0) pcyytoken = 0;
      for (yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=tmpstate); yyxi += 2);
      while (*(yyxi+=2) >= 0) if (*yyxi == pcyytoken) break;
      if ((n=yyxi[1]) < 0) { /* an accept action */
        if (yytflag) {
          int ti; int tj;
          yytfilep = fopen(yytfilen, "w");
          if (yytfilep == NULL) {
            fprintf(stderr, "Can't open t file: %s\n", yytfilen);
            return(0);          }
          for (ti=redcnt-1; ti>=0; ti--) {
            tj = svdprd[redseq[ti]];
            while (strcmp(svdnams[tj], "$EOP"))
              fprintf(yytfilep, "%s ", svdnams[tj++]);
            fprintf(yytfilep, "\n");
          }
          fclose(yytfilep);
        }
        return (0);
      }
    }


    if (n == 0) {        /* error situation */
      switch (pcyyerrfl) {
        case WAS0ERR:          /* an error just occurred */
          yyerror("syntax error");
          yyerrlab:
            ++pcyyerrct;
        case WAS1ERR:
        case WAS2ERR:           /* try again */
          pcyyerrfl = 3;
	   /* find a state for a legal shift action */
          while (yyps >= statestack) {
	     n = yypact[*yyps] + YYERRCODE;
	     if (n >= 0 && n < YYLAST && yychk[yyact[n]] == YYERRCODE) {
	       tmpstate = yyact[n];  /* simulate a shift of "error" */
	       goto enstack;
            }
	     n = yypact[*yyps];


	     /* the current yyps has no shift on "error", pop stack */
#ifdef YYDEBUG
            printf("error: pop state %d, recover state %d\n", *yyps, yyps[-1]);
#endif
	     --yyps;
	     --yypv;
	   }


	   yyabort:
            if (yytflag) {
              int ti; int tj;
              yytfilep = fopen(yytfilen, "w");
              if (yytfilep == NULL) {
                fprintf(stderr, "Can't open t file: %s\n", yytfilen);
                return(1);              }
              for (ti=1; ti<redcnt; ti++) {
                tj = svdprd[redseq[ti]];
                while (strcmp(svdnams[tj], "$EOP"))
                  fprintf(yytfilep, "%s ", svdnams[tj++]);
                fprintf(yytfilep, "\n");
              }
              fclose(yytfilep);
            }
	     return(1);


	 case WAS3ERR:  /* clobber input char */
#ifdef YYDEBUG
          printf("error: discard token %d\n", pcyytoken);
#endif
          if (pcyytoken == 0) goto yyabort; /* quit */
	   pcyytoken = -1;
	   goto newstate;      } /* switch */
    } /* if */


    /* reduction, given a production n */
#ifdef YYDEBUG
    printf("reduce with rule %d\n", n);
#endif
    if (yytflag && redcnt<YYREDMAX) redseq[redcnt++] = n;
    yyps -= yyr2[n];
    yypvt = yypv;
    yypv -= yyr2[n];
    yyval = yypv[1];
    m = n;
    /* find next state from goto table */
    n = yyr1[n];
    j = yypgo[n] + *yyps + 1;
    if (j>=YYLAST || yychk[ tmpstate = yyact[j] ] != -n) tmpstate = yyact[yypgo[n]];
    switch (m) { /* actions associated with grammar rules */

      case 1:
# line 292 "Grupo8.y"
      {
      					Ind_pgm = crearTerceto(TIPO_TERCETO_2,"END",-1,-1);
      					GrabaArchivoBinarioTs();
      					fileTercetoBinToText();
      					fileIntermediaToAssembler();
      					printf("|PROGRAMA-DECLAR			REGLA 1|\n");
      					} break;
      case 2:
# line 299 "Grupo8.y"
      {
      					Ind_pgm = crearTerceto(TIPO_TERCETO_2,"END",-1,-1);
      					GrabaArchivoBinarioTs();
      					fileTercetoBinToText();
      					fileIntermediaToAssembler();
      					printf("|PROGRAMA				REGLA 1|\n");
      } break;
      case 3:
# line 308 "Grupo8.y"
      { printf("|BLOQUE-DECLARACION			REGLA 2|\n"); } break;
      case 4:
# line 311 "Grupo8.y"
      { printf("|DECLARACION				REGLA 3|\n"); } break;
      case 5:
# line 312 "Grupo8.y"
      { printf("|DECLARACION				REGLA 3|\n"); } break;
      case 6:
# line 315 "Grupo8.y"
      { printf("|DECLARACION NORMAL			REGLA 4|\n"); } break;
      case 7:
# line 316 "Grupo8.y"
      { printf("|DECLARACION STRUCT			REGLA 4|\n"); } break;
      case 8:
# line 317 "Grupo8.y"
      { printf("|DECLARACION CONST C NOMBRE		REGLA 4|\n"); } break;
      case 9:
# line 320 "Grupo8.y"
      { printf("|DECL NORMAL				REGLA 5|\n"); } break;
      case 10:
# line 323 "Grupo8.y"
      { printf("|DECL STRUCT				REGLA 6|\n"); } break;
      case 11:
# line 326 "Grupo8.y"
      { printf("|DECL DENTRO STRUCT			REGLA 7|\n"); } break;
      case 12:
# line 327 "Grupo8.y"
      { printf("|DECL DENTRO STRUCT			REGLA 7|\n"); } break;
      case 13:
# line 330 "Grupo8.y"
      {
      																	printf("|DECL CON NOMB INT				REGLA 8|\n");
      																} break;
      case 14:
# line 333 "Grupo8.y"
      {
      											printf("|DECL CON NOMB STRING				REGLA 8|\n");
      										} break;
      case 15:
# line 339 "Grupo8.y"
      { printf("|DECL ID				REGLA 9|\n"); } break;
      case 16:
# line 340 "Grupo8.y"
      { printf("|DECL LISTA				REGLA 9|\n"); } break;
      case 17:
# line 343 "Grupo8.y"
      { printf("|SENTENCIA				REGLA 10|\n"); } break;
      case 18:
# line 344 "Grupo8.y"
      { printf("|SENTENCIA REC				REGLA 10|\n"); } break;
      case 19:
# line 347 "Grupo8.y"
      { printf("|ASIGNACION				REGLA 11|\n"); } break;
      case 20:
# line 348 "Grupo8.y"
      { printf("|SELECCION				REGLA 11|\n"); } break;
      case 21:
# line 349 "Grupo8.y"
      { printf("|ITERACION				REGLA 11|\n"); } break;
      case 22:
# line 350 "Grupo8.y"
      { printf("|DECLARACION-MAIN				REGLA 11|\n"); } break;
      case 23:
# line 351 "Grupo8.y"
      { printf("|ENTRADA				REGLA 11|\n"); } break;
      case 24:
# line 352 "Grupo8.y"
      { printf("|SALIDA					REGLA 11|\n"); } break;
      case 25:
# line 355 "Grupo8.y"
      {
      			Ind_asig = crearTerceto(TIPO_TERCETO_3,":=",yypvt[-2],Ind_exp);
      			agregarTipoATerceto(Ind_asig, verificarTipo('A', buscarTipo(yypvt[-2]), Ind_exp_tipo));
      			Ind_asig_tipo = ts_Operacion[buscarTipo(yypvt[-2])][Ind_exp_tipo];
      			printf("|ASIGNACION-NUM				REGLA 12|\n");
      			} break;
      case 26:
# line 361 "Grupo8.y"
      {
      			  Ind_asig = crearTerceto(TIPO_TERCETO_3,":=",yypvt[-2],Ind_exps);
                               agregarTipoATerceto(Ind_asig, verificarTipo('A', buscarTipo(yypvt[-2]), Ind_exps_tipo));
                              Ind_asig_tipo = ts_Operacion[buscarTipo(yypvt[-2])][Ind_exps_tipo];




      			printf("|ASIGNACION-STR				REGLA 12|\n");
      			} break;
      case 27:
# line 373 "Grupo8.y"
      {
      										crearTercetoExStr(yypvt[-2], yypvt[-0]);
      										printf("|ID-CONCAT-ID				REGLA 13|\n");
      									} break;
      case 28:
# line 377 "Grupo8.y"
      {
      										crearTercetoExStr(yypvt[-2], yypvt[-0]);
      										printf("|CTE-CONCAT-ID				REGLA 13|\n");
      									} break;
      case 29:
# line 381 "Grupo8.y"
      {
      										crearTercetoExStr(yypvt[-2], yypvt[-0]);
      										printf("|ID-CONCAT-CTE				REGLA 13|\n");
      										} break;
      case 30:
# line 385 "Grupo8.y"
      {
      										crearTercetoExStr(yypvt[-2], yypvt[-0]);
      										printf("|CTE-CONCAT-CTE				REGLA 13|\n");
      										} break;
      case 31:
# line 389 "Grupo8.y"
      {
      										Ind_exps=crearTerceto(TIPO_TERCETO_1,"", yypvt[-0],-1);
      										Ind_exps_tipo=buscarTipo(yypvt[-0]);
      										printf("|CTE_STRING				REGLA 13|\n");
      							} break;
      case 32:
# line 396 "Grupo8.y"
      {
      										Ind_exp=crearTerceto(TIPO_TERCETO_4,"ADD", Ind_exp,Ind_ter);
      										agregarTipoATerceto(Ind_exp,verificarTipo('+',Ind_exp_tipo,Ind_ter_tipo));
      										Ind_exp_tipo=ts_Operacion[Ind_exp_tipo][Ind_ter_tipo];
      										yyval= Ind_exp;
      										printf("|EXP+TERM				REGLA 14|\n");
      										} break;
      case 33:
# line 403 "Grupo8.y"
      {
      										Ind_exp=crearTerceto(TIPO_TERCETO_4,"SUB", Ind_exp,Ind_ter);
      										agregarTipoATerceto(Ind_exp,verificarTipo('-',Ind_exp_tipo,Ind_ter_tipo));
      										Ind_exp_tipo=ts_Operacion[Ind_exp_tipo][Ind_ter_tipo];
      										yyval= Ind_exp;
      										printf("|EXP-TERM				REGLA 14|\n");
      										} break;
      case 34:
# line 410 "Grupo8.y"
      {
      						Ind_exp = Ind_ter;
      						Ind_exp_tipo = Ind_ter_tipo;
      						Ind_exp_TS = Ind_ter_TS;
      						printf("|TERMINO				REGLA 14|\n");
      			} break;
      case 35:
# line 418 "Grupo8.y"
      {
      											Ind_ter=crearTerceto(TIPO_TERCETO_4,"MUL",Ind_ter,Ind_fact);
      											agregarTipoATerceto(Ind_ter,verificarTipo('*',Ind_ter_tipo,Ind_fact_tipo));
      											Ind_ter_tipo=ts_Operacion[Ind_ter_tipo][Ind_fact_tipo];
      											printf("|TERM * FACT				REGLA 15|\n");
      										} break;
      case 36:
# line 424 "Grupo8.y"
      {
      											Ind_ter=crearTerceto(TIPO_TERCETO_4,"DIV",Ind_ter,Ind_fact);
      											agregarTipoATerceto(Ind_ter,verificarTipo('/',Ind_ter_tipo,Ind_fact_tipo));
      											Ind_ter_tipo=ts_Operacion[Ind_ter_tipo][Ind_fact_tipo];
      											printf("|TERM / FACT				REGLA 15|\n");
      											} break;
      case 37:
# line 430 "Grupo8.y"
      {
      						Ind_ter=Ind_fact;
      						Ind_ter_tipo=Ind_fact_tipo;
      						Ind_ter_TS = Ind_Ts_Termino_general;
      						printf("|FACTOR					REGLA 15|\n");
      			} break;
      case 38:
# line 438 "Grupo8.y"
      {
      					yyval=yypvt[-0];
      					aux_IndiceTS = yypvt[-0];
      					Ind_fact=crearTerceto(TIPO_TERCETO_1,"",aux_IndiceTS,-1);
      					Ind_fact_tipo=buscarTipo(yypvt[-0]);
      					Ind_Ts_Termino_general = yypvt[-0];
      		} break;
      case 39:
# line 445 "Grupo8.y"
      {
      					aux_IndiceTS = yypvt[-0];
      					Ind_fact=crearTerceto(TIPO_TERCETO_1,"",aux_IndiceTS,-1);
      					Ind_fact_tipo=buscarTipo(yypvt[-0]);
      					Ind_Ts_Termino_general = yypvt[-0];
      		} break;
      case 40:
# line 451 "Grupo8.y"
      {
      					aux_IndiceTS = yypvt[-0];
      					Ind_fact=crearTerceto(TIPO_TERCETO_1,"",aux_IndiceTS,-1);
      					Ind_fact_tipo=buscarTipo(yypvt[-0]);
      					Ind_Ts_Termino_general = yypvt[-0];
      		} break;
      case 41:
# line 459 "Grupo8.y"
      { printf("|TIPO_DATO INT				REGLA 17|\n"); } break;
      case 42:
# line 460 "Grupo8.y"
      { printf("|TIPO_DATO FLOAT			REGLA 17|\n"); } break;
      case 43:
# line 461 "Grupo8.y"
      { printf("|TIPO_DATO STRING			REGLA 17|\n"); } break;
      case 44:
# line 464 "Grupo8.y"
      {
      																modificarTerceto(desapilar(pilaPos) , 1);
      																printf("|IF				REGLA 18|\n");
      												} break;
      case 45:
# line 469 "Grupo8.y"
      {
      																modificarTerceto(desapilar(pilaPos), 2);
      																Ind_if = crearTerceto(TIPO_TERCETO_5,"JMP",-1,-1);
      																apilar (pilaPos,Ind_if);
      														} break;
      case 46:
# line 474 "Grupo8.y"
      {
      																modificarTerceto(desapilar(pilaPos), 1);
      																printf("|IF-ELSE				REGLA 18|\n");
      													} break;
      case 47:
# line 480 "Grupo8.y"
      {
      						apilar(pilaUntilRepeat,crearTerceto(TIPO_TERCETO_4,"DW", indTerceto+1, indTerceto+1));
      						} break;
      case 48:
# line 483 "Grupo8.y"
      {
      												modificarTerceto(indTerceto, desapilar(pilaUntilRepeat) - indTerceto);
      												invertirSaltoTerceto(indTerceto);
      												printf("|REPEAT					REGLA 19|\n");
      												} break;
      case 49:
# line 490 "Grupo8.y"
      {

														Ind_for_id = crearTerceto(TIPO_TERCETO_1,"",yypvt[-4],-1);
      														Ind_for_id = crearTerceto(TIPO_TERCETO_3,":=",yypvt[-6],Ind_for_id);
      														agregarTipoATerceto(Ind_for_id, TIPO_ENTERO);

														//Ind_for_id = crearTerceto(TIPO_TERCETO_1,"",yypvt[-0],-1); //Agregue esta linea para el CTE_ENT del STEP
      														//Ind_for=crearTerceto(TIPO_TERCETO_8,"SUB",yypvt[-6],Ind_for_id);
      														Ind_for=crearTerceto(TIPO_TERCETO_6,"SUB",yypvt[-6],1);
      														agregarTipoATerceto(Ind_for, TIPO_ENTERO);
      														Ind_for=crearTerceto(TIPO_TERCETO_3,":=", yypvt[-6], Ind_for);

      														apilar(pilaForMarca,crearTerceto(TIPO_TERCETO_4,"FR", indTerceto+1, indTerceto+1));

      														//Ind_for_id = crearTerceto(TIPO_TERCETO_1,"",yypvt[-0],-1); //Agregue esta linea para el CTE_ENT del STEP
      														//Ind_for=crearTerceto(TIPO_TERCETO_8,"ADD",yypvt[-6],Ind_for_id); //Pongo el ID del terceto del CTE_ENT del STEP
      														Ind_for=crearTerceto(TIPO_TERCETO_6,"ADD",yypvt[-6],1);
      														agregarTipoATerceto(Ind_for, TIPO_ENTERO);
      														Ind_for_id=crearTerceto(TIPO_TERCETO_3,":=", yypvt[-6], Ind_for);

      														Ind_for_id = crearTerceto(TIPO_TERCETO_1,"",yypvt[-6],-1);
      														Ind_for = crearTerceto(TIPO_TERCETO_1,"",yypvt[-2],-1); //esto era yypvt[-2] en el tercer parametro
      														Ind_for=crearTerceto(TIPO_TERCETO_4,"CMP",Ind_for_id, Ind_for);
      														Ind_for=crearTerceto(TIPO_TERCETO_4,"JB",Ind_for,-1);
      		            									apilar(pilaFor,Ind_for);
      												} break;
      case 50:
# line 516 "Grupo8.y"
      {} break;
      case 51:
# line 518 "Grupo8.y"
      {
      														modificarTerceto(desapilar(pilaFor), 2);
      														Ind_if = crearTerceto(TIPO_TERCETO_5,"JMP",desapilar(pilaForMarca),-1);
      														printf("|FOR-STEP				REGLA 19|\n");
      													} break;
      case 52:
# line 524 "Grupo8.y"
      {

														Ind_for_id = crearTerceto(TIPO_TERCETO_1,"",yypvt[-2],-1);
      														Ind_for_id = crearTerceto(TIPO_TERCETO_3,":=",yypvt[-4],Ind_for_id);
      														agregarTipoATerceto(Ind_for_id, TIPO_ENTERO);

      														Ind_for=crearTerceto(TIPO_TERCETO_6,"SUB",yypvt[-4],1);
      														agregarTipoATerceto(Ind_for, TIPO_ENTERO);
      														Ind_for=crearTerceto(TIPO_TERCETO_3,":=", yypvt[-4], Ind_for);

      														apilar(pilaForMarca,crearTerceto(TIPO_TERCETO_4,"FR", indTerceto+1, indTerceto+1));

      														Ind_for=crearTerceto(TIPO_TERCETO_6,"ADD",yypvt[-4],1);
      														agregarTipoATerceto(Ind_for, TIPO_ENTERO);
      														Ind_for_id=crearTerceto(TIPO_TERCETO_3,":=", yypvt[-4], Ind_for);

      														Ind_for_id = crearTerceto(TIPO_TERCETO_1,"",yypvt[-4],-1);
      														Ind_for = crearTerceto(TIPO_TERCETO_1,"",yypvt[-0],-1); //esto era yypvt[-0] en el tercer parametro
      														Ind_for=crearTerceto(TIPO_TERCETO_4,"CMP",Ind_for_id, Ind_for);
      														Ind_for=crearTerceto(TIPO_TERCETO_4,"JB",Ind_for,-1);
      		            									apilar(pilaFor,Ind_for);

												} break;
      case 53:
# line 547 "Grupo8.y"
      {} break;
      case 54:
# line 549 "Grupo8.y"
      {
      														modificarTerceto(desapilar(pilaFor), 2);
      														Ind_if = crearTerceto(TIPO_TERCETO_5,"JMP",desapilar(pilaForMarca),-1);
      														printf("|FOR					REGLA 19|\n");
      													} break;
      case 55:
# line 556 "Grupo8.y"
      {
      									Ind_cond=crearTerceto(TIPO_TERCETO_4,opsalto,Ind_comp,-1);
      									apilar(pilaPos,Ind_cond);
      									printf("|CONDICION				REGLA 20|\n");
      						} break;
      case 56:
# line 561 "Grupo8.y"
      {
      									Ind_cond=crearTerceto(TIPO_TERCETO_4,negarSalto(opsalto),Ind_comp,-1);
      				            		apilar(pilaPos,Ind_cond);
      									printf("|NOT					REGLA 20|\n");
      								} break;
      case 57:
# line 566 "Grupo8.y"
      {
      													Ind_cond=crearTerceto(TIPO_TERCETO_4,oplog,Ind_compLi,Ind_compLd);
      													Ind_cond=crearTerceto(TIPO_TERCETO_4,"JZ",-1, Ind_cond+3);
      													apilar(pilaPos,Ind_cond);
      													printf("|COND-DOBLE				REGLA 20|\n");
      													} break;
      case 58:
# line 572 "Grupo8.y"
      {
      															Ind_cond=crearTerceto(TIPO_TERCETO_4,oplog,Ind_compLi,Ind_compLd);
      															Ind_cond=crearTerceto(TIPO_TERCETO_4,negarSalto("JZ"),-1,Ind_cond+3);
      															apilar(pilaPos,Ind_cond);
      															printf("|COND-DOBLE-NEG				REGLA 20|\n");
      														 } break;
      case 59:
# line 580 "Grupo8.y"
      {
      									Ind_cond=crearTerceto(TIPO_TERCETO_4,opsalto,Ind_comp+2,-1);
      									Ind_compLi = Ind_cond;
      								} break;
      case 60:
# line 586 "Grupo8.y"
      {
      									Ind_cond=crearTerceto(TIPO_TERCETO_4,opsalto,Ind_comp+2,-1);
      									Ind_compLd = Ind_cond;
      								} break;
      case 61:
# line 592 "Grupo8.y"
      {strcpy(oplog,"AND");} break;
      case 62:
# line 593 "Grupo8.y"
      {strcpy(oplog,"OR");} break;
      case 63:
# line 596 "Grupo8.y"
      {
      											Ind_comp=crearTerceto(TIPO_TERCETO_4,"CMP",Ind_condLi,Ind_condLd);
      											printf("|COMPARACION				REGLA 21|\n");
      										} break;
      case 64:
# line 602 "Grupo8.y"
      { Ind_condLi=Ind_exp; 	} break;
      case 65:
# line 605 "Grupo8.y"
      {Ind_condLd=Ind_exp;	} break;
      case 66:
# line 608 "Grupo8.y"
      {strcpy(opsalto,"JBE");  /* JNB  Jump short if not below  JNB-JA*/	} break;
      case 67:
# line 609 "Grupo8.y"
      {strcpy(opsalto,"JB");   /* JNBE Jump short if not below or equal*/	} break;
      case 68:
# line 610 "Grupo8.y"
      {strcpy(opsalto,"JNB");  /* JBE  Jump short if below or equal*/		} break;
      case 69:
# line 611 "Grupo8.y"
      {strcpy(opsalto,"JNBE"); /* JB   Jump short if below*/					} break;
      case 70:
# line 612 "Grupo8.y"
      {strcpy(opsalto,"JNE");  /*  Jump short if not equal JNZ*/				} break;
      case 71:
# line 613 "Grupo8.y"
      {strcpy(opsalto,"JE");   /*  Jump short if equal JE*/						} break;
      case 72:
# line 616 "Grupo8.y"
      {
      							Ind_out = crearTerceto(TIPO_TERCETO_2,"output",yypvt[-0],-1);
      							printf("|PUT-ID					REGLA 23|\n");
      					} break;
      case 73:
# line 620 "Grupo8.y"
      {
      							Ind_out = crearTerceto(TIPO_TERCETO_2,"output",yypvt[-0],-1);
      							printf("|PUT CTE-STR				REGLA 23|\n");
      					} break;
      case 74:
# line 626 "Grupo8.y"
      {
      							Ind_input = crearTerceto(TIPO_TERCETO_2,"input",yypvt[-0],-1);
      							printf("|GET-ID					REGLA 24|\n");
      		} break;    }
    goto enstack;
}
