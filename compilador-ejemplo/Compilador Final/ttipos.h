#include <stdio.h>
#include <stdlib.h>


int ts_Operacion[3][3];

int verificarTipo(char, int, int);


void cargarTablaSintesis()
{
     ts_Operacion[TIPO_ENTERO][TIPO_ENTERO] =  TIPO_ENTERO;
     ts_Operacion[TIPO_ENTERO][TIPO_REAL] =  ERROR_TIPO;
     ts_Operacion[TIPO_ENTERO][TIPO_STRING] = ERROR_TIPO;
     ts_Operacion[TIPO_REAL][TIPO_ENTERO] =  ERROR_TIPO;
     ts_Operacion[TIPO_REAL][TIPO_REAL] =  TIPO_REAL;
     ts_Operacion[TIPO_REAL][TIPO_STRING] =  ERROR_TIPO;
     ts_Operacion[TIPO_STRING][TIPO_ENTERO] =  ERROR_TIPO;
     ts_Operacion[TIPO_STRING][TIPO_REAL] =  ERROR_TIPO;
     ts_Operacion[TIPO_STRING][TIPO_STRING] =  TIPO_STRING;
}

//Verifica tipos de operaciones de +, -,* y /
int verificarTipo(char oper, int indLeft, int indRight)
{
	switch(ts_Operacion[indLeft][indRight])
	{
		case ERROR_TIPO:
			printf("\n\t ERROR: de incompatibilidad de tipos\n");
			system("PAUSE");
			exit(1);
		break;
		default : 
		    return ts_Operacion[indLeft][indRight];
		break;
	}
	return 0;
}
