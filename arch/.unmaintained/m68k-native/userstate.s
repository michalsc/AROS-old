#    (C) 1995-96 AROS - The Amiga Replacement OS
#    $Id$
#    $Log$
#    Revision 1.4  1996/10/24 15:51:33  aros
#    Use the official AROS macros over the __AROS versions.
#
#    Revision 1.3  1996/10/21 21:08:59  aros
#    Changed AROS_LA to AROS_LHA
#
#    Revision 1.2  1996/08/01 17:41:38  digulla
#    Added standard header for all files
#
#    Desc:
#    Lang:

#*****************************************************************************
#
#   NAME
#
#	AROS_LH1(void, UserState,
#
#   SYNOPSIS
#	AROS_LHA(APTR, sysStack, D0),
#
#   LOCATION
#	struct ExecBase *, SysBase, 26, Exec)
#
#   FUNCTION
#	Return to user mode after a call to SuperState().
#
#   INPUTS
#	sysStack - The returncode from SuperState().
#
#   RESULT
#
#   NOTES
#
#   EXAMPLE
#
#   BUGS
#
#   SEE ALSO
#	SuperState(), Supervisor()
#
#   INTERNALS
#
#   HISTORY
#
#******************************************************************************

	.globl	_Exec_UserState
_Exec_UserState:
	| simply return if argument is NULL
	tstl	d0
	jne	nonzero
	rts
nonzero:
	| Transfer sp
	movel	sp,usp

	| Set old supervisor sp
	movel	d0,sp

	| And return. This jumps directly to a rts.
	rte

