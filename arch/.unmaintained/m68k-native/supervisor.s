#    (C) 1995-96 AROS - The Amiga Replacement OS
#    $Id$
#    $Log$
#    Revision 1.4  1996/10/24 15:51:32  aros
#    Use the official AROS macros over the __AROS versions.
#
#    Revision 1.3  1996/10/21 21:08:59  aros
#    Changed AROS_LA to AROS_LHA
#
#    Revision 1.2  1996/08/01 17:41:37  digulla
#    Added standard header for all files
#
#    Desc:
#    Lang:

#*****************************************************************************
#
#   NAME
#
#	AROS_LH1(ULONG, Supervisor,
#
#   SYNOPSIS
#	AROS_LHA(ULONG_FUNC, userFunction, A5),
#
#   LOCATION
#	struct ExecBase *, SysBase, 5, Exec)
#
#   FUNCTION
#	Call a routine in supervisor mode. This routine runs on the
#	supervisor stack and must end with a 'rte'. No registers are spilled,
#	i.e. Supervisor() effectively works like a function call.
#
#   INPUTS
#	userFunction - address of the function to be called.
#
#   RESULT
#	whatever the function left in the registers
#
#   NOTES
#	This function is CPU dependant.
#
#   EXAMPLE
#
#   BUGS
#	Context switches that happen during the duration of this call are lost.
#
#   SEE ALSO
#
#   INTERNALS
#
#   HISTORY
#
#******************************************************************************

	ThisTask    =	0x114
	tc_TrapCode =	0x32

	.globl	_Exec_Supervisor
_Exec_Supervisor:
	| a privileged do-nothing instruction
	orw	#0x0000,sr
	| No trap? Then this was called from supervisor mode. Prepare a rte.
	movew	sr,sp@-
	jmp	a5@

	| CPU privilege violation vector points to here
	.globl	_TrapLevel8
_TrapLevel8:

	| There's only one legal location which may do a privilege
	| violation - and that's the instruction above.
	cmpl	#_Exec_Supervisor,sp@(2:W)
	jne	pv
	| All OK. Prepare returnaddress and go to the right direction.
	movel	#end,sp@(2:W)
	jmp	a5@
end:	rts

	| Store trap number
pv:	movel	#8,sp@-
	jra	_TrapEntry

	| And handle the trap
_TrapEntry:
	| Simple disable
	orw	#0x0700,sr

	| get some room for destination address
	subqw	#4,sp

	| calculate destination address without clobbering any registers
	movel	a0,sp@-
	movel	4,a0
	movel	a0@(ThisTask),a0
	movel	a0@(tc_TrapCode),sp@(4)
	movel	sp@+,a0

	| and jump
	rts
