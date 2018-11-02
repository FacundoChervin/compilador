#include <string.h>
#include <ctype.h>

#define _MAXLONGITUD 50

FILE *assemblerFile = NULL;

//estructura usada para la lectura de los tercetos desde el intermedia.txt
regTerceto lectura;

//cadenas auxiliares usadas durante la generacion de codigo
char aux1[_MAXLONGITUD]={'\0'};
char aux2[_MAXLONGITUD]={'\0'};
char aux3[_MAXLONGITUD]={'\0'};
char aux4[_MAXLONGITUD]={'\0'};

//boolean para saber si las var aux de string se encuentran ocupadas
int  bAux1 = 0;

//banderas para saber si estan ocupadas leftcond y rightcond
  int leftCond = 0;
  int rightCond = 0;


//*****************************************************************************
//                               declaracion de funciones
//*****************************************************************************

//funciones usadas para lograr la generacion del codigo ASM
void fileIntermediaToAssembler();
void agregarCabecera();
void agregarDeclaracion();
void agregarFunciones();
void generarCodigo(regTerceto *);
void agregarFin(char *etiqueta);
void generarASM();

int esOperacionAritmetica(char *operacion);
int esComparacion(char *operacion);

void modificarNombresTS();
void modificarNombresIntermedia();
void renameSimbolo(char *simbolo);
//funciones de ayuda para el and y or
void cargarOperacion(char *operacion, char *opTerceto);
void cargarEtiqueta(char *etiquetaNueva, char *etiqueta, char *append);
char *getCondicion();

//*****************************************************************************
//                               desarrollo de funciones
//*****************************************************************************

void fileIntermediaToAssembler()
{
   modificarNombresTS();
   modificarNombresIntermedia();
   unlink("final.asm");
   agregarCabecera();
   agregarDeclaracion();
   agregarFunciones();
   generarASM();
}

void agregarCabecera()
{
   //apertura del archivo
   assemblerFile = openFileTercetos("final.asm","at");

   //cabecera
   fprintf(assemblerFile, ".MODEL LARGE ; Modelo de memoria. \n");
   fprintf(assemblerFile, ".386 ; Tipo de procesador. \n");
   fprintf(assemblerFile, ".STACK 200h ;Bytes en el Stack. \n\n");

   //cierre del archivo
   fclose(assemblerFile);
}

void agregarDeclaracion()
{
   FILE *tsFile;
   tablaDeSimbolo tsData;

   //variable usada para guardar los tipos de datos assembler
   char tipo[3]="";
   //variables usadas para guardar los contenidos de las variables
   //y constantes en assembler
   /*float real;
   int   entero;
   char  cadena[50];*/

   assemblerFile = openFileTercetos("final.asm","at");
   tsFile = openFileTercetos("TSAssembler_bin.txt","rb");

   fprintf(assemblerFile, "\n");
   fprintf(assemblerFile, ".DATA\n");
   fprintf(assemblerFile, "MAXTEXTSIZE EQU 50\n");
   fprintf(assemblerFile, "msgPRESIONE DB 0DH,0AH, \" PROGRAMA FINALIZADO: presione una tecla para continuar...\" ,'$'\n");
   fprintf(assemblerFile, "NEWLINE    DB 0Dh,0Ah,'$'\n");
   fprintf(assemblerFile, "PUNTO       DB \".$\"\n");
   fprintf(assemblerFile, "GUION       DB \"-\" ,'$'\n");
   //se declara una cte para el incremento y decremento
   fprintf(assemblerFile, "&1    dd   1\n");
   //se declara una cte para la inicializacion
   fprintf(assemblerFile, "&0    dd   0\n");
   //se declaran las var aux para la concatenacion
   fprintf(assemblerFile, "&aux1    db    MAXTEXTSIZE dup(?) , '$' \n");
   fprintf(assemblerFile, "&aux2    db    MAXTEXTSIZE dup(?) , '$' \n");
   //se declaran auxiliares para resultado de condiciones
   fprintf(assemblerFile, "_&leftCond     dd    0 \n");
   fprintf(assemblerFile, "_&rightCond    dd    0 \n");
   //se declaran auxiliares para el display
   fprintf(assemblerFile, "&cad1 db 30 dup (?),'$'\n");
   fprintf(assemblerFile, "&cad2 db 30 dup (?),'$'\n");
   fprintf(assemblerFile, "&inc_aux dd ? \n");
   fprintf(assemblerFile, "&aux_f dd 1000 \n");
   fprintf(assemblerFile, "&aux_prec dd 100000 \n");
   fprintf(assemblerFile, "&auxN dd 0 \n");
   fprintf(assemblerFile, "aux_trunc dd 0 \n");
   fprintf(assemblerFile, "truncn dw 0 \n");
   fprintf(assemblerFile, "truncv dw 0 \n");

   fread(&tsData, sizeof(tsData), 1, tsFile);

   while (feof(tsFile) == 0)
   {
	  //enteros
      if (strcmp(tsData.tipo,"INTEGER")==0)
      {
         strcpy(tipo,"DD");
         fprintf(assemblerFile, "%s \t %s \t ? \n", tsData.nombre,tipo);
      }
      if (strcmp(tsData.tipo,"CTE_ENT")==0)
      {
         strcpy(tipo,"DD");
         fprintf(assemblerFile, "%s \t %s \t %s  \n", tsData.nombre,tipo,tsData.valor);
      }
      //reales
      if (strcmp(tsData.tipo,"FLOAT")==0)
      {
         strcpy(tipo,"DD");
         fprintf(assemblerFile, "%s \t %s \t ? \n", tsData.nombre,tipo);
      }
      if (strcmp(tsData.tipo,"CTE_REAL")==0)
      {
         strcpy(tipo,"DD");
         fprintf(assemblerFile, "%s \t %s \t %s  \n", tsData.nombre,tipo,tsData.valor);
      }
      //strings
      if ((strcmp(tsData.tipo,"CTE_STRING")==0) || (strcmp(tsData.tipo,"STRING")==0))
      {
         strcpy(tipo,"DB");
         if (strncmp(tsData.tipo,"CTE_STRING",3)==0) {
              fprintf(assemblerFile, "%s \t %s \t \"%s\" , '$', %d dup(?) \n", tsData.nombre, tipo, tsData.valor, _MAXLONGITUD - tsData.longitud +1);
         } else {
            fprintf(assemblerFile, "%s \t %s \t %d dup(?), '$'  \n", tsData.nombre, tipo, _MAXLONGITUD);
         }
      }

      fread(&tsData, sizeof(tsData), 1, tsFile);
   }

   //cierre de los archivos
   fclose(tsFile);
   fclose(assemblerFile);
}


void agregarFunciones()
{
     //apertura del archivo
     assemblerFile = openFileTercetos("final.asm","at");

     //zona de codigo
     fprintf(assemblerFile, "\n\n.CODE ; Zona de codigo\n\n");

     fprintf(assemblerFile, "\t;***STRLEN***\n");
     fprintf(assemblerFile, "\tSTRLEN PROC\n");
     fprintf(assemblerFile, "\t             mov bx,0\n");
     fprintf(assemblerFile, "\t      STRL:\n");
     fprintf(assemblerFile, "\t            cmp BYTE PTR [SI+BX],\"$\"\n");
     fprintf(assemblerFile, "\t            je STREND\n");
     fprintf(assemblerFile, "\t            inc BX\n");
     fprintf(assemblerFile, "\t            jmp STRL\n");
     fprintf(assemblerFile, "\t      STREND:\n");
     fprintf(assemblerFile, "\t	    ret\n");
     fprintf(assemblerFile, "\tSTRLEN ENDP \n\n");

     fprintf(assemblerFile, "\t;***COPIAR***\n");
     fprintf(assemblerFile, "\tCOPIAR PROC\n");
     fprintf(assemblerFile, "\t	call STRLEN  ;***STRLEN***\n");
     fprintf(assemblerFile, "\t    	cmp bx,MAXTEXTSIZE\n");
     fprintf(assemblerFile, "\t    	jle COPIARSIZEOK\n");
     fprintf(assemblerFile, "\t    	mov bx,MAXTEXTSIZE\n");
     fprintf(assemblerFile, "\t    COPIARSIZEOK:\n");
     fprintf(assemblerFile, "\t    	mov cx,bx ; la copia se hace de ’CX’ caracteres\n");
     fprintf(assemblerFile, "\t    	cld ; cld es para que la copia se realice hacia adelante\n");
     fprintf(assemblerFile, "\t    	rep movsb ; copia la cadea\n");
     fprintf(assemblerFile, "\t    	mov al,\"$\" ; carácter terminador\n");
     fprintf(assemblerFile, "\t    	mov BYTE PTR [DI],al\n");
     fprintf(assemblerFile, "\t	ret\n");
     fprintf(assemblerFile, "\tCOPIAR ENDP\n\n");

     fprintf(assemblerFile, "\t;***CONCAT***\n");
     fprintf(assemblerFile, "\tCONCAT PROC\n");
     fprintf(assemblerFile, "\t	push ds\n");
     fprintf(assemblerFile, "\t    	push si\n");
     fprintf(assemblerFile, "\t   	call STRLEN  ;***STRLEN***\n");
     fprintf(assemblerFile, "\t	mov dx,bx ; guardo en DX la cantidad de caracteres en el origen.\n");
     fprintf(assemblerFile, "\t	mov si,di\n");
     fprintf(assemblerFile, "\t	push es\n");
     fprintf(assemblerFile, "\t	pop ds\n");
     fprintf(assemblerFile, "\t      	call STRLEN  ;***STRLEN***\n");
     fprintf(assemblerFile, "\t	add di,bx ; DI ya queda apuntando al final delprimer string\n");
     fprintf(assemblerFile, "\t    	add bx,dx ; tamaño total\n");
     fprintf(assemblerFile, "\t    	cmp bx,MAXTEXTSIZE ; excede el tamaño maximo\n");
     fprintf(assemblerFile, "\t    	jg CONCATSIZEMAL\n");
     fprintf(assemblerFile, "\t    CONCATSIZEOK: ; La suma no excede el maximo, copio todos\n");
     fprintf(assemblerFile, "\t    	mov cx,dx ; los caracteres del segundo string.\n");
     fprintf(assemblerFile, "\t    	jmp CONCATSIGO\n");
     fprintf(assemblerFile, "\t    CONCATSIZEMAL: ; La suma de caracteres de los 2 strings exceden el maximo\n");
     fprintf(assemblerFile, "\t    	sub bx,MAXTEXTSIZE\n");
     fprintf(assemblerFile, "\t    	sub dx,bx\n");
     fprintf(assemblerFile, "\t    	mov cx,dx ; copio lo maximo permitido el resto se pierde.\n");
     fprintf(assemblerFile, "\t    CONCATSIGO:\n");
     fprintf(assemblerFile, "\t    	push ds\n");
     fprintf(assemblerFile, "\t    	pop es\n");
     fprintf(assemblerFile, "\t    	pop si\n");
     fprintf(assemblerFile, "\t    	pop ds\n");
     fprintf(assemblerFile, "\t    	cld ; cld es para que la copia se realice hacia adelante\n");
     fprintf(assemblerFile, "\t    	rep movsb ; copia la cadea\n");
     fprintf(assemblerFile, "\t    	mov al,\"$\" ; carácter terminador\n");
     fprintf(assemblerFile, "\t    	mov BYTE PTR [DI],al ; el regis>\n");
     fprintf(assemblerFile, "\t	ret\n");
     fprintf(assemblerFile, "\tCONCAT ENDP\n\n");

   //Muestra los datos que contienen las variables por el momento solo enteros
     fprintf(assemblerFile, "\t;***DisplayINT***\n");
     fprintf(assemblerFile, "\tDisplayINT PROC\n");
     fprintf(assemblerFile, "\t      xor edx,edx\n");
     fprintf(assemblerFile, "\t      div ecx\n");
     fprintf(assemblerFile, "\t      add DL,30H\n");
     fprintf(assemblerFile, "\t      mov [di],dl ;lo pasa a cadena\n");
     fprintf(assemblerFile, "\t      inc di ;inc 1 a cadena\n");
     fprintf(assemblerFile, "\t      add &inc_aux,1\n");
     fprintf(assemblerFile, "\t      ret\n");
     fprintf(assemblerFile, "\tDisplayINT ENDP \n\n");

     fprintf(assemblerFile, "\tE_display PROC \n");
     fprintf(assemblerFile, "\t      call DisplayINT \n");
     fprintf(assemblerFile, "\t      cmp eax,0\n");
     fprintf(assemblerFile, "\t      jne E_display\n");
     fprintf(assemblerFile, "\t      lea di,&cad2 ;cadena fuente\n");
     fprintf(assemblerFile, "\t      mov ebx,&inc_aux\n");
     fprintf(assemblerFile, "\t      mov cx,di\n");
     fprintf(assemblerFile, "\t      add ecx,ebx\n");
     fprintf(assemblerFile, "\t      mov di, cx\n");
     fprintf(assemblerFile, "\t      mov al,'$'\n");
     fprintf(assemblerFile, "\t      mov byte ptr [di],al\n");
     fprintf(assemblerFile, "\t      dec di\n");
     fprintf(assemblerFile, "\t      mov ecx,&inc_aux\n");
     fprintf(assemblerFile, "\t      lea si,&cad1 ;cadena fuente\n");
     fprintf(assemblerFile, "\t      repite:\n");
     fprintf(assemblerFile, "\t             lodsb ;pone en al un caracter\n");
     fprintf(assemblerFile, "\t             mov [di],al ;lo pasa a cad2\n");
     fprintf(assemblerFile, "\t             dec di ;resto 1 a cad2\n");
     fprintf(assemblerFile, "\t      loop repite\n");
     fprintf(assemblerFile, "\t      MOV AX, SEG &cad2 \n");
     fprintf(assemblerFile, "\t      MOV DS, AX \n");
     fprintf(assemblerFile, "\t      MOV DX, OFFSET &cad2\n");
     fprintf(assemblerFile, "\t      MOV AH, 9 \n");
     fprintf(assemblerFile, "\t      INT 21h ; Imprime\n");
     fprintf(assemblerFile, "\t      ret\n");
     fprintf(assemblerFile, "\tE_display ENDP\n");


     fprintf(assemblerFile, "\tFLOAT_INT PROC \n");
     fprintf(assemblerFile, "\t      rcl  eax,1;   //left shift acc to remove the sign\n");
     fprintf(assemblerFile, "\t      mov  ebx,eax; //save the acc\n");
     fprintf(assemblerFile, "\t      mov  edx,4278190080; //clear reg edx;4278190080\n");
     fprintf(assemblerFile, "\t      and  eax,edx; //and acc to retrieve the exponent\n");
     fprintf(assemblerFile, "\t      shr  eax,24;\n");
     fprintf(assemblerFile, "\t      sub  eax,7fh; //subtract 7fh(127) to get the actual power\n");
     fprintf(assemblerFile, "\t      mov  edx,eax; //save acc val power\n");
     fprintf(assemblerFile, "\t      mov  eax,ebx; //retrieve from ebx\n");
     fprintf(assemblerFile, "\t      rcl  eax,8;     //trim the left 8 bits that contain the power\n");
     fprintf(assemblerFile, "\t      mov  ebx,eax; //store\n");
     fprintf(assemblerFile, "\t      mov  ecx, 1fh; //subtract 17 h\n");
     fprintf(assemblerFile, "\t      sub  ecx,edx;\n");
     fprintf(assemblerFile, "\t      mov  edx,00000000h;\n");
     fprintf(assemblerFile, "\t      cmp  ecx,0;\n");
     fprintf(assemblerFile, "\t      je   loop2;\n");
     fprintf(assemblerFile, "\t      shr  eax,1;\n");
     fprintf(assemblerFile, "\t      or   eax,80000000h;\n");
     fprintf(assemblerFile, "\t loop1:   \n");
     fprintf(assemblerFile, "\t      shr  eax,1; //shift (total bits - power bits);\n");
     fprintf(assemblerFile, "\t      sub  ecx,1;\n");
     fprintf(assemblerFile, "\t      add  edx,1;\n");
     fprintf(assemblerFile, "\t      cmp  ecx,0;\n");
     fprintf(assemblerFile, "\t      ja   loop1;\n");
     fprintf(assemblerFile, "\t loop2:  \n");
     fprintf(assemblerFile, "\t      ret\n");
     fprintf(assemblerFile, "\tFLOAT_INT ENDP \n");


     fprintf(assemblerFile, "\tNEGATIVO PROC \n");
     fprintf(assemblerFile, "\t      MOV DX, OFFSET GUION ; Agrega guiom\n");
     fprintf(assemblerFile, "\t      MOV AH,09\n");
     fprintf(assemblerFile, "\t      INT 21h\n");
     fprintf(assemblerFile, "\t      mov eax, &auxN\n");
     fprintf(assemblerFile, "\t      NEG eax \n");
     fprintf(assemblerFile, "\t      ret\n");
     fprintf(assemblerFile, "\tNEGATIVO ENDP \n");

     fprintf(assemblerFile, "\tLIMPIA PROC \n");
     fprintf(assemblerFile, "\t      FFREE ST(0)\n");
     fprintf(assemblerFile, "\t      FFREE ST(1)\n");
     fprintf(assemblerFile, "\t      FFREE ST(2)\n");
     fprintf(assemblerFile, "\t      FFREE ST(3)\n");
     fprintf(assemblerFile, "\t      FFREE ST(4)\n");
     fprintf(assemblerFile, "\t      FFREE ST(5)\n");
     fprintf(assemblerFile, "\t      FFREE ST(6)\n");
     fprintf(assemblerFile, "\t      FFREE ST(7)\n");
     fprintf(assemblerFile, "\t      ret\n");
     fprintf(assemblerFile, "\tLIMPIA ENDP \n");

	   fprintf(assemblerFile, "\n\tROUND PROC \n");
     fprintf(assemblerFile, "\t 	 frndint ;(Redondea y el resultado queda en el tope)\n");
	   fprintf(assemblerFile, "\t      ret\n");
     fprintf(assemblerFile, "\tROUND ENDP \n");

     fprintf(assemblerFile, "\n\tTRUNC PROC \n");
     fprintf(assemblerFile, "\t 		fstp aux_trunc   \n");
     fprintf(assemblerFile, "\t 		xor eax,eax \n");
     fprintf(assemblerFile, "\t 		fstcw truncv \n");
     fprintf(assemblerFile, "\t 		fwait \n");
     fprintf(assemblerFile, "\t 		mov ax,truncv \n");
     fprintf(assemblerFile, "\t 		or ax,0c00h \n");
     fprintf(assemblerFile, "\t 		mov truncn,ax \n");
     fprintf(assemblerFile, "\t 		fldcw truncn \n");
	  fprintf(assemblerFile, "\t 		fld aux_trunc \n");
	  fprintf(assemblerFile, "\t			frndint       \n");
	  fprintf(assemblerFile, "\t     ret\n");
     fprintf(assemblerFile, "\tTRUNC ENDP \n");

     //MAIN DEL ASM
     fprintf(assemblerFile, "\n\nMAIN:\n");
     fprintf(assemblerFile, "\t\t mov ax,@data ; zona de codigo\n");
     fprintf(assemblerFile, "\t\t mov ds,ax; zona de codigo\n\n");
     fprintf(assemblerFile, "\t\t mov es,ax\n");
     fprintf(assemblerFile, "\n\t\t FINIT ;Inicializacion Copo\n");

      //cierre de archivo
     fclose(assemblerFile);
}


void generarASM()
{
  FILE *intermedia = openFileTercetos("intermediaAssembler.dat","rb");

   //lectura de tercetos para generar codigo
   fread(&lectura, sizeof(regTerceto), 1, intermedia);
   while(feof(intermedia) == 0)
   {
      generarCodigo(&lectura);//,vectorTercetos);
      fread(&lectura, sizeof(regTerceto), 1, intermedia);
   }

  //cierre del archivo
   fclose(intermedia);
}


void generarCodigo(regTerceto * terceto)
{
	// calcular nombre de etiqueta
	char etiqueta[10] = "e";
	char indice[4] = "";
	char etiquetaTrue[15] = "";
	char etiquetaFalse[15] = "";
	char etiquetaFin[15] = "";
	strcat(etiqueta, itoa(terceto->indice, indice, 10));
	//apertura del archivo
	assemblerFile = openFileTercetos("final.asm","at");

	  	switch(lectura.tipoTerceto)
		{
	   	case TIPO_TERCETO_1://(operando , , ) solo para id, cte
	   	{
				if(terceto->dato4 == 'I')
				{
					fprintf(assemblerFile, "\t%s:\t FILD %s\n", etiqueta, terceto->dato1);
				}
				else if(terceto->dato4 == 'F')
				{
					fprintf(assemblerFile, "\t%s:\t FLD %s\n", etiqueta, terceto->dato1);
				}
				else  if (terceto->dato4 == 'S')
				{
					fprintf(assemblerFile, "\t%s:\t MOV SI, OFFSET %s \n",etiqueta, terceto->dato1);
					if (bAux1 == 0)
					{
						fprintf(assemblerFile, "\t\t MOV DI, OFFSET &aux1 \n");
						fprintf(assemblerFile, "\t\t CALL COPIAR\n\n");
						bAux1 = 1;
					}
					else
					{
						fprintf(assemblerFile, "\t\t MOV DI, OFFSET &aux2 \n");
						fprintf(assemblerFile, "\t\t CALL COPIAR\n\n");
					}
				}
			}
			break;
       	case TIPO_TERCETO_2://(operador , operando , ) tipo
			{
				if (strcmp(terceto->dato1,"output")==0)
				{
					if (terceto->dato4 == 'S')
					{
						fprintf(assemblerFile, "\t%s:\t MOV AX, SEG %s \n", etiqueta, terceto->dato2);
						fprintf(assemblerFile, "\t\t MOV DS, AX \n");
						fprintf(assemblerFile, "\t\t MOV DX, OFFSET %s \n", terceto->dato2);
						fprintf(assemblerFile, "\t\t MOV AH, 9 \n");
						fprintf(assemblerFile, "\t\t INT 21h ; Imprime \n");
						fprintf(assemblerFile, "\t\t MOV DX, OFFSET NEWLINE ; Agrega newline\n");
						fprintf(assemblerFile, "\t\t MOV AH,09\n");
						fprintf(assemblerFile, "\t\t INT 21h\n\n");
					}
					else if  (terceto->dato4 == 'I')
					{
						fprintf(assemblerFile, "\t%s:\t mov cl,10 \n", etiqueta);
						fprintf(assemblerFile, "\t\t mov eax, %s \n", terceto->dato2);
						fprintf(assemblerFile, "\t\t mov &auxN, eax \n");
						fprintf(assemblerFile, "\t\t NEG eax            ; lo niego. si el numero es negativo la bandera de NEGATIVE estara desactivada\n");
						fprintf(assemblerFile, "\t\t mov eax,  %s \n", terceto->dato2);

						fprintf(assemblerFile, "\t\t lea si,&cad1  \n");
						fprintf(assemblerFile, "\t\t mov di, si\n");
						fprintf(assemblerFile, "\t\t mov &inc_aux,0 \n");
						fprintf(assemblerFile, "\t\t call E_display \n");
						fprintf(assemblerFile, "\t\t MOV DX, OFFSET NEWLINE ; Agrega newline\n");
						fprintf(assemblerFile, "\t\t MOV AH,09\n");
						fprintf(assemblerFile, "\t\t INT 21h\n\n");
					}
					else if  (terceto->dato4 == 'F')
					{
						fprintf(assemblerFile, "\t%s:\t mov  eax,%s \n", etiqueta,terceto->dato2);
						fprintf(assemblerFile, "\t\t CALL FLOAT_INT \n");

						fprintf(assemblerFile, "\t\t mov cl,10 \n");

						fprintf(assemblerFile, "\t\t lea si,&cad1  \n");
						fprintf(assemblerFile, "\t\t mov di, si\n");
						fprintf(assemblerFile, "\t\t mov &inc_aux,0 \n");

						fprintf(assemblerFile, "\t\t call E_display \n");

						fprintf(assemblerFile, "\t\t MOV DX, OFFSET PUNTO \n");
						fprintf(assemblerFile, "\t\t MOV AH,09\n");
						fprintf(assemblerFile, "\t\t INT 21h\n\n");

						fprintf(assemblerFile, "\t\t mov eax, %s \n", terceto->dato2);
						fprintf(assemblerFile, "\t\t CALL FLOAT_INT \n");
						fprintf(assemblerFile, "\t\t mov &aux_f,eax \n");
						fprintf(assemblerFile, "\t\t FLD  %s \n", terceto->dato2);
						fprintf(assemblerFile, "\t\t FILD  &aux_f \n");
						fprintf(assemblerFile, "\t\t FSUB  \n");
						fprintf(assemblerFile, "\t\t FILD &aux_prec \n");
						fprintf(assemblerFile, "\t\t FMUL \n");
						fprintf(assemblerFile, "\t\t FSTP &aux_f \n");
						fprintf(assemblerFile, "\t\t FWAIT \n");
						fprintf(assemblerFile, "\t\t mov eax,&aux_f \n");

						fprintf(assemblerFile, "\t\t FFREE \n");
						fprintf(assemblerFile, "\t\t CALL FLOAT_INT \n");
						fprintf(assemblerFile, "\t\t mov cl,10 \n");
						fprintf(assemblerFile, "\t\t lea si,&cad1  \n");
						fprintf(assemblerFile, "\t\t mov di, si\n");
						fprintf(assemblerFile, "\t\t mov &inc_aux,0 \n");

						fprintf(assemblerFile, "\t\t call E_display \n");

						fprintf(assemblerFile, "\t\t MOV DX, OFFSET NEWLINE ; Agrega newline\n");
						fprintf(assemblerFile, "\t\t MOV AH,09\n");
						fprintf(assemblerFile, "\t\t INT 21h\n\n");
					}
				}
				if (strcmp(terceto->dato1,"END")==0)
				{
					agregarFin(etiqueta);
				}
				if (strcmp(terceto->dato1,"round")==0)
				{
					fprintf(assemblerFile, "\t\t FLD  _%s \n", terceto->dato2);
					fprintf(assemblerFile, "\t\t CALL ROUND\n\n");
				}
				if (strcmp(terceto->dato1,"trunc")==0)
				{
					fprintf(assemblerFile, "\t\t FLD  _%s \n", terceto->dato2);
					fprintf(assemblerFile, "\t\t CALL TRUNC\n\n");
				}
				if (strcmp(terceto->dato1,"free")==0)
				{
					fprintf(assemblerFile, "\t\t CALL LIMPIA\n\n");
				}
    	 	}
			break;
			case TIPO_TERCETO_3://(operador , operando , indice ) tipo
			{
				if (strcmp(lectura.dato1,":=")==0)
				{
					if(terceto->dato4 == 'I')
					{
						fprintf(assemblerFile, "\t%s:\t FIST %s\n", etiqueta, terceto->dato2);
					}
					if(terceto->dato4 == 'F')
					{
						fprintf(assemblerFile, "\t%s:\t FST %s\n", etiqueta, terceto->dato2);
					}
					if (terceto->dato4 == 'S')
					{
						fprintf(assemblerFile, "\t\t MOV SI, OFFSET &aux1 \n");
						fprintf(assemblerFile, "\t%s:\t MOV DI, OFFSET %s \n", etiqueta, terceto->dato2);
						fprintf(assemblerFile, "\t\t CALL COPIAR\n\n");
						bAux1 = 0;
					}
					fprintf(assemblerFile, "\t\t FWAIT \n");
				}
			}
			break;
			case TIPO_TERCETO_4://(operador , indice , indice ) tipo
			{  // operaciones mul, add, div, sub,
				if (esOperacionAritmetica(terceto->dato1))
				{
					if (terceto->dato4 == 'S')
					{ 	//concatenacion de string
					  	fprintf(assemblerFile, "\t%s:\t MOV SI, OFFSET &aux2 \n",etiqueta);
					  	fprintf(assemblerFile, "\t\t MOV DI, OFFSET &aux1 \n");
						fprintf(assemblerFile, "\t\t CALL CONCAT\n\n");
					}
					else
					{
						fprintf(assemblerFile, "\t%s:\t F%s \n", etiqueta, terceto->dato1);
					}
				}
				else if(strcmp(terceto->dato1, "CMP") == 0)
				{
					fprintf(assemblerFile, "\t%s:\t FCOMP \n", etiqueta);
					fprintf(assemblerFile, "\t\t FSTSW AX \n");
					fprintf(assemblerFile, "\t\t FWAIT \n");
					fprintf(assemblerFile, "\t\t SAHF \n");
				}	// and y or
				else if((strcmp(terceto->dato1, "AND") == 0) || strcmp(terceto->dato1, "OR") == 0)
				{
					fprintf(assemblerFile, "\t\t CALL LIMPIA \n");
					fprintf(assemblerFile, "\t\t MOV AX, WORD PTR _&leftCond \n");
					fprintf(assemblerFile, "\t\t %s AX, WORD PTR _&rightCond  \n", terceto->dato1);
					//reseteo los auxiliares que indican que lado de la condicion se  esta armando
					leftCond = 0;
					rightCond = 0;
				}	// operaciones logicas  jne, jnz, jnb, jnbe, jz
				else if(esComparacion(terceto->dato1))
				{	//cargar operación - la operación se recalcula debido al cambio
					//de orden de los operandos en la pila del coprocesador.
					char operacion[5] = "";
					cargarOperacion(operacion, terceto->dato1);
					//el " e-1 " indica que se trata de un salto para un AND/OR
					//printf("---------> %s  , %s \n", terceto->dato1, terceto->dato3);
					if(strcmp(terceto->dato3," e-1 ") == 0)  //ETIQUETAS PARA CALCULAR COMPARACIONES COMPUESTAS
					{
						//calculo de etiquetas para saltos.
						cargarEtiqueta(etiquetaTrue, etiqueta, "True");
						cargarEtiqueta(etiquetaFalse, etiqueta, "False");
						cargarEtiqueta(etiquetaFin, etiqueta, "Fin");

						// escribo el salto a la parte False
						fprintf(assemblerFile, "\t%s:\t %s  %s \n", etiqueta, operacion, etiquetaFalse);
						//escribo la parte TRUE - pongo en uno la variable condición que corresponda
						fprintf(assemblerFile, "\t%s:\t MOV  %s, 1 \n", etiquetaTrue, getCondicion());
						fprintf(assemblerFile, "\t\t JMP %s \n", etiquetaFin);
						//escribo la parte FALSE
						fprintf(assemblerFile, "\t%s:\t MOV %s, 0 \n", etiquetaFalse, getCondicion());
						//etiqueta de fin de de comparación para condición compuesta
						fprintf(assemblerFile, "\t%s: \n", etiquetaFin);
					}
					//condición simple
					else
					{
					  fprintf(assemblerFile, "\t%s:\t %s %s \n", etiqueta, operacion, terceto->dato3);
					  fprintf(assemblerFile, "\t\t CALL LIMPIA \n");
					}
				} 	//salto luego del and/or
				else if((strcmp(terceto->dato1, "JNZ") == 0) || (strcmp(terceto->dato1, "JZ") == 0)) // USADO PARA CONDICIONES AND/OR
				{
					//printf("\t ======ENTRA %s - %s \n", terceto->dato1, terceto->dato2);
					fprintf(assemblerFile, "\t%s:\t %s %s \n", etiqueta, terceto->dato1, terceto->dato3); // terceto->dato3
				}   // etiqueta DO while
	 	  	  	else if(strcmp(terceto->dato1, "DW") == 0) //DO WHILE
	 	  	  	{
					fprintf(assemblerFile, "\t%s:  CALL LIMPIA ;etiqueta DO WHILE\n", etiqueta);
				}
				else if(strcmp(terceto->dato1, "FR") == 0) //FOR
	 	  	  	{
					fprintf(assemblerFile, "\t%s:  ;etiqueta FOR\n", etiqueta);
				}
			}
			break;
			case TIPO_TERCETO_5://(operador , indice , ) tipo
			{
				//jmp
				fprintf(assemblerFile, "\t%s:\t JMP %s \n", etiqueta, terceto->dato2);
			}
			break;
      	case TIPO_TERCETO_6://(operador , operando , 1 ) tipo
			{
				//operaciones inc, dec
				if ((strcmp(terceto->dato1,"ADD")==0) || (strcmp(terceto->dato1,"SUB")==0))
				{
					//fprintf(assemblerFile, "\t%s:\t FFREE \n", etiqueta);
					fprintf(assemblerFile, "\t%s:\t FiLD %s\n", etiqueta, terceto->dato2);
					fprintf(assemblerFile, "\t   \t FiLD &1\n");
					fprintf(assemblerFile, "\t   \t F%s \n", terceto->dato1);
					fprintf(assemblerFile, "\t   \t FiSTP %s\n", terceto->dato2);
					fprintf(assemblerFile, "\t   \t FWAIT \n");
					fprintf(assemblerFile, "\t   \t FFREE \n");
				}
				else if(strcmp(terceto->dato1,":=")==0)
				{
					//operaciones de inicializacion de acumuladores
					fprintf(assemblerFile, "\t%s:\t FLD &0\n", etiqueta);
					fprintf(assemblerFile, "\t   \t FSTP %s\n", terceto->dato2);
					fprintf(assemblerFile, "\t   \t FWAIT \n");
					fprintf(assemblerFile, "\t   \t FFREE \n");
				}
         }
         break;
			case TIPO_TERCETO_7://(operador , operando , operando ) tipo
			{
				if(strcmp(terceto->dato1,":=")==0)
				{
			      // id1 := id2
				   fprintf(assemblerFile, "\t%s:\t FLD %s\n", etiqueta, terceto->dato3);
			    	//para renombrar constantes
			      fprintf(assemblerFile, "\t   \t FSTP %s\n", terceto->dato2);
			      fprintf(assemblerFile, "\t   \t FWAIT \n");
			      fprintf(assemblerFile, "\t   \t FFREE \n");
				}
			}
     }

	//cierre del archivo
   fclose(assemblerFile);
}

int esOperacionAritmetica(char *operacion)
{
 	 if((strcmp(operacion,"MUL")==0) ||
 	 (strcmp(operacion,"DIV")==0) ||
 	 (strcmp(operacion,"ADD")==0) ||
 	 (strcmp(operacion,"SUB")==0) )
	  	 return 1;
 	 else
 	 	 return 0;
}

int esComparacion(char *operacion)
{
 	 if((strcmp(operacion,"JNE")==0) ||
 	 (strcmp(operacion,"JE")==0)  ||
 	 (strcmp(operacion,"JBE")==0) ||
 	 (strcmp(operacion,"JB")==0)  ||
 	 (strcmp(operacion,"JNB")==0) ||
 	 (strcmp(operacion,"JNBE")==0) )
	  	 return 1;
 	 else
 	 	 return 0;
}

void agregarFin(char *etiqueta)
{
	//apertura del archivo
   assemblerFile = openFileTercetos("final.asm","at");

   //grabacion del final del archivo ASM
   fprintf(assemblerFile, "\n\n\t%s:\t MOV DX, OFFSET NEWLINE\n", etiqueta);
   fprintf(assemblerFile, "\t\t MOV ah, 09\n");
   fprintf(assemblerFile, "\t\t int 21h\n");
   fprintf(assemblerFile, "\n");
   //muestra el mensaje por pantalla
   fprintf(assemblerFile, "\t\t MOV dx,OFFSET msgPRESIONE\n");
   fprintf(assemblerFile, "\t\t MOV ah,09\n");
   fprintf(assemblerFile, "\t\t INT 21h\n");
   //espera que se presione una tecla
   fprintf(assemblerFile, "\t\t MOV ah, 01\n");
   fprintf(assemblerFile, "\t\t INT 21h\n");
   fprintf(assemblerFile, "\t\t MOV ax, 4C00h\n");
   fprintf(assemblerFile, "\t\t INT 21h\n");
   //fin del main del ASM
   fprintf(assemblerFile, "END MAIN\n");
   //cierre del archivo
   fclose(assemblerFile);
}

void modificarNombresTS()
{
  tablaDeSimbolo tsData;

  //Abrir archivos
  FILE *tsReadFile = openFileTercetos("TS_bin.txt","rb");
  FILE *tsWriteFile = openFileTercetos("TSAssembler_bin.txt","wb+");

  limpiaAux(&tsData);

	//recorrerlo
	fread(&tsData, sizeof(tablaDeSimbolo), 1, tsReadFile);
	while (feof(tsReadFile) == 0)
	{
		//por cada uno agregar _ al inicio, cambiar "." y " " por _
		//char reNombre[51] = "_";
		renameSimbolo(tsData.nombre);

		fwrite(&tsData, sizeof(tablaDeSimbolo), 1, tsWriteFile);
		fread(&tsData, sizeof(tablaDeSimbolo), 1, tsReadFile);
  	}
	//cerrar archivos
  	fclose(tsReadFile);
  	fclose(tsWriteFile);
}

void modificarNombresIntermedia()
{
  regTerceto tercetoData;

 	 //Abrir archivos
  FILE *tsReadFile = openFileTercetos("intermedia.dat","rb");
  FILE *tsWriteFile = openFileTercetos("intermediaAssembler.dat","wb+");
  limpiaTerceto(&tercetoData);

 	 //recorrerlo
  fread(&tercetoData, sizeof(regTerceto), 1, tsReadFile);

  while (feof(tsReadFile) == 0)
  {
   		switch(tercetoData.tipoTerceto)
		{
		   case TIPO_TERCETO_1://(operando , , ) solo para id, cte
		   		renameSimbolo(tercetoData.dato1);
				break;
		   case TIPO_TERCETO_2://(operador , operando , ) tipo
		   		if (strcmp(tercetoData.dato1,"output")==0)
		  		{
				   renameSimbolo(tercetoData.dato2);
 		   		}
		  		break;
		  case TIPO_TERCETO_3://(operador , operando , indice ) tipo
		  	    if (strcmp(tercetoData.dato1,":=")==0)
			  	{
			       renameSimbolo(tercetoData.dato2);
			    }
		  	  	break;
		  case TIPO_TERCETO_6://(operador , operando , 1 ) tipo
			   //operaciones inc, dec
			   if ((strcmp(tercetoData.dato1,"ADD")==0) || (strcmp(tercetoData.dato1,"SUB")==0) || (strcmp(tercetoData.dato1,":=")==0))
			   {
			 		 renameSimbolo(tercetoData.dato2);
			   }
			   break;
		  case TIPO_TERCETO_7://(operador , operando , operando ) tipo
			   if(strcmp(tercetoData.dato1,":=")==0)
			   {
					renameSimbolo(tercetoData.dato2);
					renameSimbolo(tercetoData.dato3);
			   }
     }

		fwrite(&tercetoData, sizeof(regTerceto), 1, tsWriteFile);
    	fread(&tercetoData, sizeof(regTerceto), 1, tsReadFile);
  }
 	 //cerrar archivos
  fclose(tsReadFile);
  fclose(tsWriteFile);
}

void renameSimbolo(char *simbolo)
{
 	 //por cada uno agregar _ al inicio, cambiar "." y " " por _
 	 char reNombre[51] = "_";
 	 //Se sacan los . del nombre de la constante porque da error un nombre de va con .
	 int cantCaracteres = strlen(simbolo);
	 int i;
	 for(i = 0; i < cantCaracteres ; i++)
	 {
	  	   if(simbolo[i] == '.' || simbolo[i] == ' ')
		  	{
  		  	   reNombre[i + 1] = '_';
		    } else
		    {
		  	   reNombre[i + 1] = simbolo[i];
	  	    }
	 }
	 strcpy(simbolo, reNombre);
}

void cargarOperacion(char *operacion, char *opTerceto) //AFECTA LAS CONDICIONES: NO TOCAR!
{

	 if((strcmp(opTerceto, "JNE") == 0) || (strcmp(opTerceto, "JE") == 0))
	 {
	  	 strcpy(operacion, opTerceto);
	 }
	 else {
         //strcpy(operacion, negarSalto(opTerceto));
         strcpy(operacion, opTerceto);
	 }
}

void cargarEtiqueta(char *etiquetaNueva, char *etiqueta, char *append)
{
 	strcpy(etiquetaNueva, etiqueta);
	strcat(etiquetaNueva, append);
}

char *getCondicion()
{
 	 //pasa 2 veces por cada condicion, una para armar la parte de TRUe
	  //y otra para armar la parte de FAlse
   if(leftCond < 2)
   {
  	   leftCond = leftCond + 1;
	   return "_&leftCond";
   }
   else
   {
		rightCond = rightCond + 1;
		return "_&rightCond";
   }
}
