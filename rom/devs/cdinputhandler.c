/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$
    $Log$
    Revision 1.1  1996/08/23 17:32:23  digulla
    Implementation of the console.device


    Desc:
    Lang: english
*/

/*****************************************************************************

    NAME */
	#include <exec/libraries.h>
	#include <devices/inputevent.h>
	#include <clib/console_protos.h>

	__AROS_LH2(struct InputEvent *, CDInputHandler,

/*  SYNOPSIS */
	__AROS_LHA(struct InputEvent *, events, A0),
	__AROS_LHA(struct Library    *, consoleDevice, A1),

/*  LOCATION */
	struct Library *, ConsoleDevice, 7, Console)

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
			    console_lib.fd and clib/console_protos.h

*****************************************************************************/
{
    __AROS_FUNC_INIT
    __AROS_BASE_EXT_DECL(struct Library *,ConsoleDevice)

    return NULL;

    __AROS_FUNC_EXIT
} /* CDInputHandler */
