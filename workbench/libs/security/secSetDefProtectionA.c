/*
    Copyright � 2002-2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <stdio.h>

#include "security_intern.h"

#define DEBUG 1
#include <aros/debug.h>

/*****************************************************************************

    NAME */
	AROS_LH1(BOOL, secSetDefProtectionA,

/*  SYNOPSIS */
	/* (taglist) */
	AROS_LHA(struct TagItem *, taglist, A0),

/*  LOCATION */
	struct Library *, SecurityBase, 3, Security)

/*  FUNCTION

    INPUTS


    RESULT


    NOTES


    EXAMPLE

    BUGS

    SEE ALSO


    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *, SecurityBase)

    D(bug( DEBUG_NAME_STR "secSetDefProtectionA()\n") );;

    return NULL;

    AROS_LIBFUNC_EXIT

} /* secSetDefProtectionA */

