;* Assembly includefile generated by RevUp 1.5 *

VERSION		EQU	1
REVISION	EQU	1
DATE	MACRO
		dc.b	'7.10.96'
	ENDM
VERS	MACRO
		dc.b	'protracker 1.1'
	ENDM
VSTRING	MACRO
		dc.b	'protracker 1.1 (7.10.96)',13,10,0
	ENDM
VERSTAG	MACRO
		dc.b	0,'$VER: protracker 1.1 (7.10.96)',0
	ENDM
