/*
    (C) 1995-2000 AROS - The Amiga Research OS
    $Id$

    Desc: OOP function OOP_ReleaseAttrBases
    Lang: english
*/

#include "intern.h"
#include "hash.h"
/*****************************************************************************

    NAME */
#include <proto/exec.h>
#include <exec/memory.h>
#include <aros/libcall.h>

#include <aros/debug.h>

	AROS_LH1(VOID, OOP_ReleaseAttrBases,

/*  SYNOPSIS */
	AROS_LHA(struct OOP_ABDescr *, abd, A0),

/*  LOCATION */
	struct Library *, OOPBase, 19, OOP)

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
    AROS_LIBBASE_EXT_DECL(struct Library*,OOPBase)
    
    
    for (; abd->interfaceID; abd ++)
    {
        if ( *abd->attrBase != 0 )
	{
	    OOP_ReleaseAttrBase(abd->interfaceID);
	    *abd->attrBase = 0;
	}
    }
    
    
    AROS_LIBFUNC_EXIT

} /* OOP_ReleaseAttrBases  */

