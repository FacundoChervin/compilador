.MODEL LARGE ; Modelo de memoria. 
.386 ; Tipo de procesador. 
.STACK 200h ;Bytes en el Stack. 


.DATA
MAXTEXTSIZE EQU 50
msgPRESIONE DB 0DH,0AH, " PROGRAMA FINALIZADO: presione una tecla para continuar..." ,'$'
NEWLINE    DB 0Dh,0Ah,'$'
PUNTO       DB ".$"
GUION       DB "-" ,'$'
&1    dd   1
&0    dd   0
&aux1    db    MAXTEXTSIZE dup(?) , '$' 
&aux2    db    MAXTEXTSIZE dup(?) , '$' 
_&leftCond     dd    0 
_&rightCond    dd    0 
&cad1 db 30 dup (?),'$'
&cad2 db 30 dup (?),'$'
&inc_aux dd ? 
&aux_f dd 1000 
&aux_prec dd 100000 
&auxN dd 0 
aux_trunc dd 0 
truncn dw 0 
truncv dw 0 
_a 	 DD 	 ? 
_b 	 DD 	 ? 
_i 	 DD 	 ? 
_c 	 DB 	 50 dup(?), '$'  
_d 	 DB 	 50 dup(?), '$'  
_pivote 	 DD 	 ? 
__30 	 DD 	 30  
_cadena 	 DB 	 50 dup(?), '$'  
__Compi 	 DB 	 "Compi" , '$', 46 dup(?) 
__2 	 DD 	 2  
_auto_patente 	 DB 	 50 dup(?), '$'  
_auto_modelo 	 DD 	 ? 
__3 	 DD 	 3  
__4 	 DD 	 4  
__AnalisisLexico 	 DB 	 "AnalisisLexico" , '$', 37 dup(?) 
__AnalisisSintactico 	 DB 	 "AnalisisSintactico" , '$', 33 dup(?) 
__20 	 DD 	 20  
__1 	 DD 	 1  
__5 	 DD 	 5  
__10 	 DD 	 10  


.CODE ; Zona de codigo

	;***STRLEN***
	STRLEN PROC
	             mov bx,0
	      STRL:
	            cmp BYTE PTR [SI+BX],"$"
	            je STREND
	            inc BX
	            jmp STRL
	      STREND:
		    ret
	STRLEN ENDP 

	;***COPIAR***
	COPIAR PROC
		call STRLEN  ;***STRLEN***
	    	cmp bx,MAXTEXTSIZE
	    	jle COPIARSIZEOK
	    	mov bx,MAXTEXTSIZE
	    COPIARSIZEOK:
	    	mov cx,bx ; la copia se hace de ’CX’ caracteres
	    	cld ; cld es para que la copia se realice hacia adelante
	    	rep movsb ; copia la cadea
	    	mov al,"$" ; carácter terminador
	    	mov BYTE PTR [DI],al
		ret
	COPIAR ENDP

	;***CONCAT***
	CONCAT PROC
		push ds
	    	push si
	   	call STRLEN  ;***STRLEN***
		mov dx,bx ; guardo en DX la cantidad de caracteres en el origen.
		mov si,di
		push es
		pop ds
	      	call STRLEN  ;***STRLEN***
		add di,bx ; DI ya queda apuntando al final delprimer string
	    	add bx,dx ; tamaño total
	    	cmp bx,MAXTEXTSIZE ; excede el tamaño maximo
	    	jg CONCATSIZEMAL
	    CONCATSIZEOK: ; La suma no excede el maximo, copio todos
	    	mov cx,dx ; los caracteres del segundo string.
	    	jmp CONCATSIGO
	    CONCATSIZEMAL: ; La suma de caracteres de los 2 strings exceden el maximo
	    	sub bx,MAXTEXTSIZE
	    	sub dx,bx
	    	mov cx,dx ; copio lo maximo permitido el resto se pierde.
	    CONCATSIGO:
	    	push ds
	    	pop es
	    	pop si
	    	pop ds
	    	cld ; cld es para que la copia se realice hacia adelante
	    	rep movsb ; copia la cadea
	    	mov al,"$" ; carácter terminador
	    	mov BYTE PTR [DI],al ; el regis>
		ret
	CONCAT ENDP

	;***DisplayINT***
	DisplayINT PROC
	      xor edx,edx
	      div ecx
	      add DL,30H
	      mov [di],dl ;lo pasa a cadena
	      inc di ;inc 1 a cadena
	      add &inc_aux,1
	      ret
	DisplayINT ENDP 

	E_display PROC 
	      call DisplayINT 
	      cmp eax,0
	      jne E_display
	      lea di,&cad2 ;cadena fuente
	      mov ebx,&inc_aux
	      mov cx,di
	      add ecx,ebx
	      mov di, cx
	      mov al,'$'
	      mov byte ptr [di],al
	      dec di
	      mov ecx,&inc_aux
	      lea si,&cad1 ;cadena fuente
	      repite:
	             lodsb ;pone en al un caracter
	             mov [di],al ;lo pasa a cad2
	             dec di ;resto 1 a cad2
	      loop repite
	      MOV AX, SEG &cad2 
	      MOV DS, AX 
	      MOV DX, OFFSET &cad2
	      MOV AH, 9 
	      INT 21h ; Imprime
	      ret
	E_display ENDP
	FLOAT_INT PROC 
	      rcl  eax,1;   //left shift acc to remove the sign
	      mov  ebx,eax; //save the acc
	      mov  edx,4278190080; //clear reg edx;4278190080
	      and  eax,edx; //and acc to retrieve the exponent
	      shr  eax,24;
	      sub  eax,7fh; //subtract 7fh(127) to get the actual power
	      mov  edx,eax; //save acc val power
	      mov  eax,ebx; //retrieve from ebx
	      rcl  eax,8;     //trim the left 8 bits that contain the power
	      mov  ebx,eax; //store
	      mov  ecx, 1fh; //subtract 17 h
	      sub  ecx,edx;
	      mov  edx,00000000h;
	      cmp  ecx,0;
	      je   loop2;
	      shr  eax,1;
	      or   eax,80000000h;
	 loop1:   
	      shr  eax,1; //shift (total bits - power bits);
	      sub  ecx,1;
	      add  edx,1;
	      cmp  ecx,0;
	      ja   loop1;
	 loop2:  
	      ret
	FLOAT_INT ENDP 
	NEGATIVO PROC 
	      MOV DX, OFFSET GUION ; Agrega guiom
	      MOV AH,09
	      INT 21h
	      mov eax, &auxN
	      NEG eax 
	      ret
	NEGATIVO ENDP 
	LIMPIA PROC 
	      FFREE ST(0)
	      FFREE ST(1)
	      FFREE ST(2)
	      FFREE ST(3)
	      FFREE ST(4)
	      FFREE ST(5)
	      FFREE ST(6)
	      FFREE ST(7)
	      ret
	LIMPIA ENDP 

	ROUND PROC 
	 	 frndint ;(Redondea y el resultado queda en el tope)
	      ret
	ROUND ENDP 

	TRUNC PROC 
	 		fstp aux_trunc   
	 		xor eax,eax 
	 		fstcw truncv 
	 		fwait 
	 		mov ax,truncv 
	 		or ax,0c00h 
	 		mov truncn,ax 
	 		fldcw truncn 
	 		fld aux_trunc 
				frndint       
	     ret
	TRUNC ENDP 


MAIN:
		 mov ax,@data ; zona de codigo
		 mov ds,ax; zona de codigo

		 mov es,ax

		 FINIT ;Inicializacion Copo
	e1:	 FILD __30
	e2:	 FIST _pivote
		 FWAIT 
	e3:	 MOV SI, OFFSET __Compi 
		 MOV DI, OFFSET &aux1 
		 CALL COPIAR

		 MOV SI, OFFSET &aux1 
	e4:	 MOV DI, OFFSET _cadena 
		 CALL COPIAR

		 FWAIT 
	e5:	 FILD __3
	e6:	 FIST _a
		 FWAIT 
	e7:	 FILD __4
	e8:	 FIST _b
		 FWAIT 
	e9:	 MOV SI, OFFSET __AnalisisLexico 
		 MOV DI, OFFSET &aux1 
		 CALL COPIAR

		 MOV SI, OFFSET &aux1 
	e10:	 MOV DI, OFFSET _c 
		 CALL COPIAR

		 FWAIT 
	e11:	 MOV SI, OFFSET __AnalisisSintactico 
		 MOV DI, OFFSET &aux1 
		 CALL COPIAR

		 MOV SI, OFFSET &aux1 
	e12:	 MOV DI, OFFSET _d 
		 CALL COPIAR

		 FWAIT 
	e13:	 FILD __20
	e14:	 FIST _auto_modelo
		 FWAIT 
	e15:	 MOV SI, OFFSET _c 
		 MOV DI, OFFSET &aux1 
		 CALL COPIAR

	e16:	 MOV SI, OFFSET _cadena 
		 MOV DI, OFFSET &aux2 
		 CALL COPIAR

	e17:	 MOV SI, OFFSET &aux2 
		 MOV DI, OFFSET &aux1 
		 CALL CONCAT

		 MOV SI, OFFSET &aux1 
	e18:	 MOV DI, OFFSET _c 
		 CALL COPIAR

		 FWAIT 
	e19:	 MOV AX, SEG _c 
		 MOV DS, AX 
		 MOV DX, OFFSET _c 
		 MOV AH, 9 
		 INT 21h ; Imprime 
		 MOV DX, OFFSET NEWLINE ; Agrega newline
		 MOV AH,09
		 INT 21h

	e20:	 FILD __3
	e21:	 FILD __1
	e22:	 FIST _i
		 FWAIT 
	e23:	 FiLD _i
	   	 FiLD &1
	   	 FSUB 
	   	 FiSTP _i
	   	 FWAIT 
	   	 FFREE 
		 FWAIT 
	e25:  ;etiqueta FOR
	e26:	 FiLD _i
	   	 FiLD &1
	   	 FADD 
	   	 FiSTP _i
	   	 FWAIT 
	   	 FFREE 
		 FWAIT 
	e28:	 FILD _i
	e29:	 FILD __3
	e30:	 FCOMP 
		 FSTSW AX 
		 FWAIT 
		 SAHF 
	e31:	 JB  e38  
		 CALL LIMPIA 
	e32:	 FILD _i
	e33:	 FILD __2
	e34:	 FCOMP 
		 FSTSW AX 
		 FWAIT 
		 SAHF 
	e35:	 JNB  e37  
		 CALL LIMPIA 
	e36:	 mov cl,10 
		 mov eax, _i 
		 mov &auxN, eax 
		 NEG eax            ; lo niego. si el numero es negativo la bandera de NEGATIVE estara desactivada
		 mov eax,  _i 
		 lea si,&cad1  
		 mov di, si
		 mov &inc_aux,0 
		 call E_display 
		 MOV DX, OFFSET NEWLINE ; Agrega newline
		 MOV AH,09
		 INT 21h

	e37:	 JMP  e25  
	e38:	 FILD __5
	e39:	 FILD __10
	e40:	 FCOMP 
		 FSTSW AX 
		 FWAIT 
		 SAHF 
	e41:	 JNB  e41False 
	e41True:	 MOV  _&leftCond, 1 
		 JMP e41Fin 
	e41False:	 MOV _&leftCond, 0 
	e41Fin: 
	e42:	 FILD __5
	e43:	 FILD __20
	e44:	 FCOMP 
		 FSTSW AX 
		 FWAIT 
		 SAHF 
	e45:	 JNB  e45False 
	e45True:	 MOV  _&rightCond, 1 
		 JMP e45Fin 
	e45False:	 MOV _&rightCond, 0 
	e45Fin: 
		 CALL LIMPIA 
		 MOV AX, WORD PTR _&leftCond 
		 AND AX, WORD PTR _&rightCond  
	e47:	 JNZ  e49  
	e48:	 mov cl,10 
		 mov eax, _a 
		 mov &auxN, eax 
		 NEG eax            ; lo niego. si el numero es negativo la bandera de NEGATIVE estara desactivada
		 mov eax,  _a 
		 lea si,&cad1  
		 mov di, si
		 mov &inc_aux,0 
		 call E_display 
		 MOV DX, OFFSET NEWLINE ; Agrega newline
		 MOV AH,09
		 INT 21h

	e49:	 FILD _b
	e50:	 FILD _pivote
	e51:	 FADD 
	e52:	 FILD _auto_modelo
	e53:	 FSUB 
	e54:	 FIST _a
		 FWAIT 
	e55:	 mov cl,10 
		 mov eax, _a 
		 mov &auxN, eax 
		 NEG eax            ; lo niego. si el numero es negativo la bandera de NEGATIVE estara desactivada
		 mov eax,  _a 
		 lea si,&cad1  
		 mov di, si
		 mov &inc_aux,0 
		 call E_display 
		 MOV DX, OFFSET NEWLINE ; Agrega newline
		 MOV AH,09
		 INT 21h

	e56:  CALL LIMPIA ;etiqueta DO WHILE
	e57:	 mov cl,10 
		 mov eax, _b 
		 mov &auxN, eax 
		 NEG eax            ; lo niego. si el numero es negativo la bandera de NEGATIVE estara desactivada
		 mov eax,  _b 
		 lea si,&cad1  
		 mov di, si
		 mov &inc_aux,0 
		 call E_display 
		 MOV DX, OFFSET NEWLINE ; Agrega newline
		 MOV AH,09
		 INT 21h

	e58:	 FILD _b
	e59:	 FILD __1
	e60:	 FADD 
	e61:	 FIST _b
		 FWAIT 
	e62:	 FILD _b
	e63:	 FILD __5
	e64:	 FCOMP 
		 FSTSW AX 
		 FWAIT 
		 SAHF 
	e65:	 JNBE  e56  
		 CALL LIMPIA 


	e66:	 MOV DX, OFFSET NEWLINE
		 MOV ah, 09
		 int 21h

		 MOV dx,OFFSET msgPRESIONE
		 MOV ah,09
		 INT 21h
		 MOV ah, 01
		 INT 21h
		 MOV ax, 4C00h
		 INT 21h
END MAIN
