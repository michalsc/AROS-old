/*
    (C) 1995 AROS - The Amiga Replacement OS
    $Id$
    $Log$
    Revision 1.4  1996/10/24 15:51:36  aros
    Use the official AROS macros over the __AROS versions.

    Revision 1.3  1996/09/12 14:52:47  digulla
    Better way to separate public and private parts of the library base

    Revision 1.2  1996/08/31 12:58:12  aros
    Merged in/modified for FreeBSD.

    Desc:
    Lang: english
*/
#include "utility_intern.h"

/*****************************************************************************

    NAME */
	#include <clib/utility_protos.h>

	AROS_LH0(ULONG, GetUniqueID,

/*  SYNOPSIS */
	/* void */

/*  LOCATION */
	struct UtilityBase *, UtilityBase, 45, Utility)

/*  FUNCTION
	Returns a unique id that is different from any other id that is
	obtained from this function call.

    INPUTS

    RESULT
	an unsigned long id

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS
	Calls Disable()/Enable() to guarentee uniqueness.

    HISTORY
	29-10-95    digulla automatically created from
			    utility_lib.fd and clib/utility_protos.h
	17-08-96    iaint   Reimplemented. CVS lost my old one. Well I did.

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    ULONG ret;

    Disable();

    ret = ++(GetIntUtilityBase(UtilityBase)->ub_LastID);

    Enable();

    return ret;

    AROS_LIBFUNC_EXIT
} /* GetUniqueID */
