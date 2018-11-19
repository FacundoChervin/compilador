include macros2.asm
include number.asm

.MODEL SMALL
.386
.STACK 200h

.DATA

	_a	dd	?
	_b	dd	?
	_c	dd	?
	_d	dd	?
	_f	dd	?
	_g	dd	?
	_final	dd	?
	_const_string_0	dd	"hola"
	_const_string_1	dd	"12345"
	_const_string_2	dd	"1234"
	_const_string_3	dd	"123"
	_const_string_4	dd	"12"
	_const_string_5	dd	"1"
	_const_string_6	dd	""

.CODE

START:
ET_0:

	CMP _a, _b
	JG ET_59


	mov _a, 0.010000

	mov _b, -1.213000

	mov _b, 0.123130

	mov _d, _const_string_0

	getString _d

	CMP 2, _final
	JL ET_37


	CMP 1, _final
	JG ET_37


	mov _final, 1

	JMP ET_40


ET_37:

	mov _final, 0

ET_40:

	CMP 1, 2
	JL ET_57


	displayString _const_string_1

	displayString _const_string_2

	displayString _const_string_3

	displayString _const_string_4

	displayString _const_string_5

	displayString _const_string_6

ET_57:

	JMP ET_0


ET_59:

	mov R1, 111
	sum R1, 222
	mov @aux1, R1

	mov R1, 111
	sum R1, 100
	mov @aux2, R1

	mov R1, @aux2
	sum R1, 333
	mov @aux3, R1

	mov R1, @aux1
	sum R1, @aux3
	mov @aux4, R1

	mov R1, @aux4
	div R1, 3
	mov @aux5, R1

	mov R1, 25
	mul R1, 1
	mov @aux6, R1

	mov R1, 123
	sub R1, 1
	mov @aux7, R1

	mov R1, @aux6
	sum R1, @aux7
	mov @aux8, R1

	mov _final, @aux8

END START
