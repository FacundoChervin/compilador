//============================================================================//
//=========================== CODIGO INTERMEDIO ==============================//
//============================================================================//

/* -------------------------------------------------------------------------- */
/*                           ESTRCUTURA DE TERCETO                            */
/* -------------------------------------------------------------------------- */

typedef struct
{
   int indice;      //INDICE DE TERCETO
   char dato1[40];  //OPERACION
   char dato2[40];  //OPERADOR1
   char dato3[40];  //OPERADOR2
   char dato4;      //TIPO RESULTADO TERCETO
   int tipoTerceto;
} regTerceto;

/* -------------------------------------------------------------------------- */
/*                           VARIABLES DE TERCETOS                            */
/* -------------------------------------------------------------------------- */

int indTerceto = 0;
int indTercetoAux = 0;

regTerceto terceto;
regTerceto auxTerceto[20]; // Almacena hasta 20 tercetos en MM ???
char auxTercetoOperador [40];
struct tpila *pilaOperador = NULL;

FILE * tercetoFile = NULL;      //Fichero Binario de tercetos
FILE * tercetoFileText = NULL;  //Fichero de tercetos

/* -------------------------------------------------------------------------- */
/*                    DECLARACION FUNCIONES DE TERCETOS                       */
/* -------------------------------------------------------------------------- */

//Basicas
int  crearTerceto(int tipoTerceto, char *operador, int operando1, int operando2);
void agregarTipoATerceto(int posicion, int tipo);
void modificarTerceto(int posicion, int inc);     //??
void invertirSaltoTerceto(int posicion); //Invierte el operador de salto para operaciones DO-WHILE
int recuperarTerceto(int posicion);

//Almacenamiento en memoria hasta que sea el momento de grabarlo en el archivo
int guardarTerceto(int tipoTerceto, char *operador, int operando1, int operando2);
int grabarTerceto(int pos);

//Grabado en Archivo
FILE *openFileTercetos(char *, char *);
void fileTercetoBinToText();

//???
/*
char *neg_OpRel(char *Op);
char *invertirOpRel(char *Op);
*/
// Uso de la TOS para crear  Tercetos
char * buscarNombreSimbolo(int);
char buscarTipoSimbolo(int);
int buscarIndiceSimbolo(char * nombre);

int buscarTipo(int);
int esTipo(int , char *);
char convertirTipoAChar(int);
char * negarSalto(char *);


/* -------------------------------------------------------------------------- */
/*                     DEFINICION FUNCIONES DE TERCETOS                       */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* crearTerceto(): crea terceto y lo graba en archivo binario                 */
/* -------------------------------------------------------------------------- */
int  crearTerceto(int tipoTerceto, char *operador, int operando1, int operando2)
{
	tercetoFile = openFileTercetos("Intermedia.dat","a+b");
	indTerceto++; //Arranca en 1
	terceto.indice = indTerceto;
	terceto.tipoTerceto = tipoTerceto;

   //Depende del tipo de terceto se almacenara diferente la info en el registro
   switch(tipoTerceto)
	{
		case TIPO_TERCETO_1:
		{
			strcpy(terceto.dato1, buscarNombreSimbolo(operando1));
			strcpy(terceto.dato2, "  ");
			strcpy(terceto.dato3, "  ");
			terceto.dato4 = buscarTipoSimbolo(operando1);
		}
		break;
		case TIPO_TERCETO_2:
		{
			strcpy(terceto.dato1, operador);
			strcpy(terceto.dato2, buscarNombreSimbolo(operando1));
			strcpy(terceto.dato3, "  ");
			terceto.dato4 = buscarTipoSimbolo(operando1);
		}
		break;
		case TIPO_TERCETO_3:
		{
			strcpy(terceto.dato1, operador);
			strcpy(terceto.dato2, buscarNombreSimbolo(operando1));
			//printf("TERCETO_3 %s\n",terceto.dato2);
			sprintf (terceto.dato3, " e%d ", operando2);
			terceto.dato4 = ' ';
		}
		break;
		case TIPO_TERCETO_4:
		{
			strcpy(terceto.dato1, operador);
			sprintf (terceto.dato2, " e%d ", operando1);
			sprintf (terceto.dato3, " e%d ", operando2);
			terceto.dato4 = ' ';
		}
		break;
		case TIPO_TERCETO_5:
		{
			strcpy(terceto.dato1, operador);
			sprintf (terceto.dato2, " e%d ", operando1);
			strcpy(terceto.dato3, "  ");
			terceto.dato4 = ' ';
		}
		break;
		case TIPO_TERCETO_6:
		{
			strcpy(terceto.dato1, operador);
			strcpy(terceto.dato2, buscarNombreSimbolo(operando1));
			sprintf (terceto.dato3, "%d ", operando2);
			terceto.dato4 = buscarTipoSimbolo(operando1);
		}
		break;
		case TIPO_TERCETO_7:
		{
			strcpy(terceto.dato1, operador);
			strcpy(terceto.dato2, buscarNombreSimbolo(operando1));
			strcpy(terceto.dato3, buscarNombreSimbolo(operando2));
			terceto.dato4 = buscarTipoSimbolo(operando1);
		}
		break;
	}

	fwrite (&terceto, sizeof(regTerceto), 1, tercetoFile);
  	fclose(tercetoFile);
   return indTerceto;
}

/* -------------------------------------------------------------------------- */
/* agregarTipoATerceto(): Al terceto creado le agrega el tipo                 */
/* -------------------------------------------------------------------------- */
void agregarTipoATerceto(int posicion, int tipo)
{
     tercetoFile = openFileTercetos("Intermedia.dat","rb+");

     fseek(tercetoFile, (posicion-1)*sizeof(regTerceto), SEEK_SET);
     fread(&terceto, sizeof(regTerceto), 1, tercetoFile);

     terceto.dato4 = convertirTipoAChar(tipo);

     fseek(tercetoFile, (posicion-1)*sizeof(regTerceto), SEEK_SET);
     fwrite(&terceto, sizeof(regTerceto), 1, tercetoFile);

     fclose(tercetoFile);
}

void modificarTerceto(int posicion, int inc)
{
     tercetoFile = openFileTercetos("Intermedia.dat","rb+");

     fseek(tercetoFile, (posicion-1)*sizeof(regTerceto), SEEK_SET);
     fread(&terceto, sizeof(regTerceto), 1, tercetoFile);

     if (strcmp(terceto.dato1, "JMP") == 0)
     {
          sprintf (terceto.dato2, " e%d ", indTerceto+inc);
     } else {
          sprintf (terceto.dato3, " e%d ", indTerceto+inc);
     }

     fseek(tercetoFile, (posicion-1)*sizeof(regTerceto), SEEK_SET);
     fwrite(&terceto, sizeof(regTerceto), 1, tercetoFile);

     fclose(tercetoFile);
}


void invertirSaltoTerceto(int posicion)
{
     tercetoFile = openFileTercetos("Intermedia.dat","rb+");

     fseek(tercetoFile, (posicion-1)*sizeof(regTerceto), SEEK_SET);
     fread(&terceto, sizeof(regTerceto), 1, tercetoFile);
     strcpy (terceto.dato1, negarSalto(terceto.dato1));

     fseek(tercetoFile, (posicion-1)*sizeof(regTerceto), SEEK_SET);
     fwrite(&terceto, sizeof(regTerceto), 1, tercetoFile);

     fclose(tercetoFile);
}

int recuperarTerceto(int posicion)
{
	 tercetoFile = openFileTercetos("Intermedia.dat","rb+");

     fseek(tercetoFile, (posicion-1)*sizeof(regTerceto), SEEK_SET);
     fread(&terceto, sizeof(regTerceto), 1, tercetoFile);

	if (pilaOperador == NULL)
	{
		if ((crearPila(&pilaOperador)) == -1)
		{
			printf("error al crear la pila \n");
			system("PAUSE");
			return -1;
		}
	}

	 int retIndiceTerceto = 0;
	 switch(terceto.tipoTerceto)
	{
		case TIPO_TERCETO_1:
		{
			printf("posicion: %d", posicion);
			retIndiceTerceto = crearTerceto(TIPO_TERCETO_1,"",buscarIndiceSimbolo(terceto.dato1),-1);
		}
		break;
		case TIPO_TERCETO_4:
		{
			if (strcmp(terceto.dato1, "SUB") == 0)
			{
				apilar(pilaOperador, 1);
			}
			else if (strcmp(terceto.dato1, "ADD") == 0)
			{
				apilar(pilaOperador, 2);
			}
			else if (strcmp(terceto.dato1, "MUL") == 0)
			{
				apilar(pilaOperador, 3);
			}
			else if (strcmp(terceto.dato1, "DIV") == 0)
			{
				apilar(pilaOperador, 4);
			}
			else
			{
				printf("ERROR: Operador invalido\n");
			}
			//|  2       |   ( _5 ,    ,    ) I
			//|  3       |   ( _8 ,    ,    ) I
			//|  4       |   ( SUB ,  [ 2 ]  ,  [ 3 ]  ) I

			/*
				|  2       |   ( _3 ,    ,    ) I
				|  3       |   ( _1 ,    ,    ) I
				|  4       |   ( SUB ,  [ 2 ]  ,  [ 3 ]  ) I
				|  5       |   ( _1 ,    ,    ) I
				|  6       |   ( ADD ,  [ 4 ]  ,  [ 5 ]  ) I
*/
			int x=int(terceto.dato2[2]-'0');
			int y=int(terceto.dato3[2]-'0');

			x = recuperarTerceto(x);
			y = recuperarTerceto(y);
			printf("recuperar t2: %i\n",x);
			printf("recuperar t3: %i\n",y);

			int IndOperador = desapilar(pilaOperador);
			if (IndOperador == 1)
			{
				strcpy(auxTercetoOperador, "SUB");
			}
			else if (IndOperador == 2)
			{
				strcpy(auxTercetoOperador, "ADD");
			}
			else if (IndOperador == 3)
			{
				strcpy(auxTercetoOperador, "MUL");
			}
			else if (IndOperador == 4)
			{
				strcpy(auxTercetoOperador, "DIV");
			}
			retIndiceTerceto = crearTerceto(TIPO_TERCETO_4,auxTercetoOperador, x,y);
		}
		break;
	}

     fclose(tercetoFile);
	 return retIndiceTerceto;
	 //strcpy(aux->dato2,"");
}

// Uso de la TOS para crear  Tercetos
int buscarIndiceSimbolo(char * nombre)
{
	for (int i=0; i< TAMMAX ; i++)
	{
		if (strcmp(TOS[i].nombre, nombre) == 0)
		{
			return i;
		}
	}
	return 0;
}


//Almacenamiento en memoria hasta que sea el momento de grabarlo en el archivo
int guardarTerceto(int tipoTerceto, char *operador, int operando1, int operando2)
{
	return 0;
}

int grabarTerceto(int pos)
{
	return 0;
}

//Grabado en Archivo
FILE *openFileTercetos(char *fileName, char *mode)
{
	FILE* file = fopen(fileName, mode);
	if (file == NULL)
	{
	    printf( "No se puede abrir el archivo %s.\n", fileName);
	    exit( 1 );
	}

	return file;
}

// Uso de la TOS para crear  Tercetos
char * buscarNombreSimbolo(int posicion)
{
	if ((posicion >= 0) && (posicion < TAMMAX )) // Maximo de TOS
	{
		return TOS[posicion].nombre;
	}
	return "";
}

char buscarTipoSimbolo(int posicion)
{
	if ((posicion >= 0) && (posicion < TAMMAX ))
	{
		if (esTipo(posicion, "CTE_ENT") || esTipo(posicion,"INTEGER"))
		{
			return convertirTipoAChar(TIPO_ENTERO);
		}
		else if (esTipo(posicion, "CTE_REAL") || esTipo(posicion, "FLOAT"))
		{
      	return convertirTipoAChar(TIPO_REAL);
	  	}
	  	else if (esTipo(posicion, "CTE_STRING") || esTipo(posicion, "STRING"))
	  	{
	  		return convertirTipoAChar(TIPO_STRING);
		}
		else if (esTipo(posicion, "STRUCT"))
	  	{
	     	return convertirTipoAChar(TIPO_STRUCT);
		}
		else if (esTipo(posicion, "CONST"))
	  	{
	     	return convertirTipoAChar(TIPO_CONST);
		}
		else
		{
			printf("\n Error de validacion - Variable Inexistente\n");
            printf("%d\n" , posicion);
			system("PAUSE");
			exit(1);
		}
	}

	return ' ';
}

int buscarTipo(int posicion)
{
	if ((posicion >= 0) && (posicion < TAMMAX ))
   {
		if (esTipo(posicion, "CTE_ENT") || esTipo(posicion,"INTEGER"))
		{
	     	return TIPO_ENTERO;
		}
		else if (esTipo(posicion, "CTE_REAL") || esTipo(posicion, "FLOAT"))
		{
	     	return TIPO_REAL;
	  	}
	  	else if (esTipo(posicion, "CTE_STRING") || esTipo(posicion, "STRING"))
	  	{
	     	return TIPO_STRING;
		}
		else if (esTipo(posicion, "STRUCT"))
	  	{
	     	return TIPO_STRUCT;
		}
		else if (esTipo(posicion, "CONST"))
	  	{
	     	return TIPO_CONST;
		}
		else
		{
			printf("\n Error de validacion - Variable Inexistente\n");
			system("PAUSE");
			exit(1);
		}
	}
   return ' ';
}

int esTipo(int posicion, char * strTipo)
{
	if(strcmp (TOS[posicion].tipo, strTipo) == 0)
		return 1;
	else
		return 0;
}

char convertirTipoAChar(int tipo)
{
 	 switch(tipo)
 	 {
		 case TIPO_ENTERO:
		 	  return 'I';
		 case TIPO_REAL:
		 	  return 'F';
		 case TIPO_STRING:
		 	  return 'S';
         case TIPO_STRUCT:
		 	  return 'T';
         case TIPO_CONST:
		 	  return 'C';
		 default:
		 	  printf("\n Tipo inexistente");
		 	  system("PAUSE");
		 	  exit(1);
	 }
    return 'X';
}

char obtenerIndice(char destino[],char fuente[])
{
	int i=0;
	int j=0;

   while(fuente[i])
   {
		if (fuente[i] != 'e') {
		 destino[j]=fuente[i];
		 j++;
		}
		i++;
	}

	destino[j]='\0';
	return *destino;
}

char * negarSalto(char * operadorSalto)
{
	if (strcmp(operadorSalto, "JNB") == 0)
	{
		return "JB";
	}
	if (strcmp(operadorSalto, "JB") == 0)
	{
		return "JNB";
	}
	if (strcmp(operadorSalto, "JNBE") == 0)
	{
		return "JBE";
	}
	if (strcmp(operadorSalto, "JBE") == 0)
	{
		return "JNBE";
	}
	if (strcmp(operadorSalto, "JNE") == 0)
	{
		return "JE";
	}
	if (strcmp(operadorSalto, "JE") == 0)
	{
		return "JNE";
	}
	if (strcmp(operadorSalto, "JZ") == 0)
	{
		return "JNZ";
	}
	if (strcmp(operadorSalto, "JNZ") == 0)
	{
		return "JZ";
	}
}



void fileTercetoBinToText()
{
	char aux1[40],aux2[40],aux3[40];

	tercetoFile = openFileTercetos("Intermedia.dat","a+b");
	tercetoFileText = openFileTercetos("Intermedia.txt","at");
	fread(&terceto, sizeof(regTerceto), 1, tercetoFile);

	while (feof(tercetoFile) == 0)
	{
	    switch(terceto.tipoTerceto)
	    {
			case TIPO_TERCETO_3:
			{
				obtenerIndice(aux1,terceto.dato3);
				sprintf (terceto.dato3, " [%s] ", aux1);
			}
			break;
			case TIPO_TERCETO_4:
			{
				obtenerIndice(aux1,terceto.dato3);
				sprintf (terceto.dato3, " [%s] ", aux1); //Operando2
				obtenerIndice(aux2,terceto.dato2);
				sprintf (terceto.dato2, " [%s] ", aux2); //Operando1
			}
			break;
			case TIPO_TERCETO_5:
			{
				obtenerIndice(aux2,terceto.dato2);
				sprintf (terceto.dato2, " [%s] ", aux2);
			}
			break;
	    }
	    fprintf(tercetoFileText, "|  %-*d |   ( %s , %s , %s ) %c\n",(sizeof(int)*1 + 3),terceto.indice,terceto.dato1,terceto.dato2,terceto.dato3,terceto.dato4);
	    fread(&terceto, sizeof(regTerceto), 1, tercetoFile);
	}

    fclose(tercetoFile);
    fclose(tercetoFileText);
}

void limpiaTerceto(regTerceto * aux)
{
     aux->indice   = 0;
     strcpy(aux->dato1,"");
     strcpy(aux->dato2,"");
     strcpy(aux->dato3,"");
     aux->dato4  = ' ';
     aux->tipoTerceto  = 0;
}
