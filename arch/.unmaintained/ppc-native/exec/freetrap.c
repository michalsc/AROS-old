/*
    (C) 1995 AROS - The Amiga Research OS
    $Id$

    Desc: Free a trap.
    Lang: english
*/
#include "exec_intern.h"
#include <proto/exec.h>

/*****************************************************************************

    NAME */

	AROS_LH1(void, FreeTrap,

/*  SYNOPSIS */
	AROS_LHA(long, trapNum, D0),

/*  LOCATION */
	struct ExecBase *, SysBase, 58, Exec)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	29-10-95    digulla automatically created from
			    exec_lib.fd and clib/exec_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct ExecBase *,SysBase)

    AROS_LIBFUNC_EXIT
} /* FreeTrap */
