/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$

    Desc:
    Lang:
*/

/******************************************************************************

    NAME
	AROS_LH0(void, UserState,

    LOCATION
	struct ExecBase *, SysBase, 26, Exec)

    FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

******************************************************************************/

	#include "machine.i"

	.text
	.balign 16
	.globl	AROS_SLIB_ENTRY(UserState,Exec)
	.type	AROS_SLIB_ENTRY(UserState,Exec),@function
AROS_SLIB_ENTRY(UserState,Exec):
	/* Dummy */
	ret

