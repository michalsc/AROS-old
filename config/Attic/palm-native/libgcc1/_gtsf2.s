	.text
	.even
	.globl __gtsf2

__gtsf2:
	moveml	%sp@(4),%d0-%d1	| get u and v
	tstl	%d1		| check sign bit
	bpl	L_1
	negl	%d1		| negate
	eorl	#0x80000000,%d1	| toggle sign bit
L_1:
	tstl	%d0		| check sign bit
	bpl	L_2
	negl	%d0		| negate
	eorl	#0x80000000,%d0	| toggle sign bit
L_2:
	cmpl	%d1,%d0
	sgt	%d0
	andl	#1,%d0
	rts
