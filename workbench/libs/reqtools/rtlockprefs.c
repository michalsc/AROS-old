
/*
    (C) 1999 AROS - The Amiga Research OS
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

    AROS_LH0(struct ReqToolsPrefs *, rtLockPrefs,

/*  SYNOPSIS */

/*  LOCATION */

	struct ReqToolsBase *, ReqToolsBase, 28, ReqTools)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    rtUnlockPrefs()

    INTERNALS

    HISTORY

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    ObtainSemaphore(&GPB(ReqToolsBase)->rt.ReqToolsPrefs.PrefsSemaphore);

    return &GPB(ReqToolsBase)->rt.ReqToolsPrefs;

    AROS_LIBFUNC_EXIT
} /* rtLockPrefs */
