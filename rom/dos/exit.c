/*
    (C) 1995-2000 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: English
*/
#include "dos_intern.h"

/*****************************************************************************

    NAME */
#include <proto/dos.h>

	AROS_LH1(void, Exit,

/*  SYNOPSIS */
	AROS_LHA(LONG, returnCode, D1),

/*  LOCATION */
	struct DosLibrary *, DOSBase, 24, Dos)

/*  FUNCTION

    Obsolete function, the purpose of which was to exit from a BCPL program.
    DO NOT CALL THIS FUNCTION!

    INPUTS

    RESULT

    NOTES

    This function is obsolete and doens't do anything. Therefore, do not
    call it!

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct DosLibrary *,DOSBase)

	/* Poof, program lost in BCPL space */
    
	/* Maybe we should somehow notify the user that this is not
	   a function to use in a direct manner? */

    AROS_LIBFUNC_EXIT
} /* Exit */
