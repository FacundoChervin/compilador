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
	_g,fin	dd	?
	_final	dd	?
	_0.010000	dd	?
	_-1.213000	dd	?
	_0.123130	dd	?
	_hola	dd	?
	_1	dd	?
	_2	dd	?
	_25	dd	?
	_123	dd	?

.CODE

START:
	mov _a, 0.010000
	mov _b, -1.213000
	mov _b, 0.123130
	mov _d, hola
	getString d
	mov _final, 1
	mov _final, 0
	displayString "12345"
	displayString "1234"
	displayString "123"
	displayString "12"
	displayString "1"
	displayString ""
	mov R1, _25
	mul R1, _1
	mov @aux1, R1
	mov R1, _123
	sub R1, _1
	mov @aux2, R1
	mov R1, _@aux1
	sum R1, _@aux2
	mov @aux3, R1
	mov _final, @aux3
END START
