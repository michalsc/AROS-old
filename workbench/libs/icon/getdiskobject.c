/*
    Copyright � 1995-2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#define DEBUG 1
#include <aros/debug.h>

#include <proto/arossupport.h>
#include <proto/dos.h>
#include <workbench/icon.h>
#include <dos/dos.h>

#include <string.h>

#include "icon_intern.h"

/*****************************************************************************

    NAME */
#include <clib/icon_protos.h>
#include <exec/types.h>

	AROS_LH1(struct DiskObject *, GetDiskObject,

/*  SYNOPSIS */
	AROS_LHA(CONST_STRPTR, name, A0),

/*  LOCATION */
	struct IconBase *, IconBase, 13, Icon)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct IconBase *, IconBase)
    
    return GetIconTagList(name, NULL);
    
    AROS_LIBFUNC_EXIT
} /* GetDiskObject() */
