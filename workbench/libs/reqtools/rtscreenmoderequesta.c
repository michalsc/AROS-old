
/*
    (C) 1999 - 2000 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: English
*/

#include <exec/types.h>
#include <proto/exec.h>
#include <proto/reqtools.h>
#include <proto/intuition.h>
#include <exec/libraries.h>
#include <exec/memory.h>
#include <aros/libcall.h>

#include "reqtools_intern.h"

/*****************************************************************************

    NAME */

    AROS_LH3(ULONG, rtScreenModeRequestA,

/*  SYNOPSIS */

	AROS_LHA(struct rtScreenModeRequester *, screenmodereq, A1),
	AROS_LHA(char *, title, A3),
	AROS_LHA(struct TagItem *, taglist, A0),

/*  LOCATION */

	struct ReqToolsBase *, ReqToolsBase, 24, ReqTools)

/*  FUNCTION
   
    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    return (ULONG)FileRequestA((struct RealFileRequester *)screenmodereq, NULL, title, taglist); /* in filereq.c */
    
    AROS_LIBFUNC_EXIT
    
} /* rtScreenModeRequestA */
