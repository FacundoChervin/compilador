include macros2.asm
include number.asm

.MODEL LARGE
.386
.STACK 200h

.DATA
	@aux0	dd	?
	@aux1	dd	?
	@aux2	dd	?
	@aux3	dd	?
	@aux4	dd	?
	@aux5	dd	?
	@aux6	dd	?
	@aux7	dd	?
	@aux8	dd	?

	_a	dd	?
	_b	dd	?
	_c	dd	?
	_d	dd	?
	_f	dd	?
	_g	dd	?
	_final	dd	?
	@1_000000	dd	1.000000
	@2_000000	dd	2.000000
	@0_010000	dd	0.010000
	@N1_213000	dd	-1.213000
	@0_123130	dd	0.123130
	_const_string_0	db	"12345"
	_const_string_1	db	"1234"
	_const_string_2	db	"123"
	_const_string_3	db	"12"
	_const_string_4	db	"1"
	_const_string_5	db	""

.CODE

MOV AX,@DATA
MOV DS,AX
FINIT;

START:
	fld @1_000000
	fstp _a

	fld @2_000000
	fstp _b

ET_6:

	mov ecx, [_a]
	CMP ecx, [_b]
	JG ET_62


	fld @0_010000
	fstp _a

	fld @N1_213000
	fstp _b

	fld @0_123130
	fstp _b

	getString _d

	mov ecx, 2
	CMP ecx, [_final]
	JL ET_40


	mov ecx, 1
	CMP ecx, [_final]
	JG ET_40


	mov _final, 1

	JMP ET_43


ET_40:

	mov _final, 0

ET_43:

	mov ecx, 1
	CMP ecx, 2
	JL ET_60


	displayString _const_string_0

	displayString _const_string_1

	displayString _const_string_2

	displayString _const_string_3

	displayString _const_string_4

	displayString _const_string_5

ET_60:

	JMP ET_6


ET_62:

	mov eax, 111
	add eax, 222
	mov @aux1, eax

	mov eax, 111
	add eax, 100
	mov @aux2, eax

	mov eax, [@aux2]
	add eax, 333
	mov @aux3, eax

	mov eax, [@aux1]
	add eax, [@aux3]
	mov @aux4, eax

	mov eax, [@aux4]
	mov ebx, 3
	div ebx
	mov @aux5, eax

	mov eax, 25
	mov ebx, 1
	mul ebx
	mov @aux6, eax

	mov eax, 123
	sub eax, 1
	mov @aux7, eax

	mov eax, [@aux6]
	add eax, [@aux7]
	mov @aux8, eax

	mov eax, [@aux8]
	mov _final, eax

	mov ah, 1 ; pausa, espera que oprima una tecla
	int 21h ; AH=1 es el servicio de lectura
	MOV AX, 4C00h ; Sale del Dos
	INT 21h       ; Enviamos la interrupcion 21h

END START
