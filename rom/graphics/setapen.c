/*
    (C) 1995 AROS - The Amiga Replacement OS
    $Id$
    $Log$
    Revision 1.5  1996/10/24 15:51:06  aros
    Use the official AROS macros over the __AROS versions.

    Revision 1.4  1996/10/02 16:41:27  digulla
    Always pass GfxBase to driver

    Revision 1.3  1996/08/23 17:09:44  digulla
    Moved the modification of the rastport behind the call of the driver to
    allow the driver to examine the old value

    Revision 1.2  1996/08/13 13:59:02  digulla
    Added call to driver
    Replaced AROS_LA by AROS_LHA
    Added include

    Revision 1.1  1996/08/12 14:28:29  digulla
    Change forground color of Rastport

    Desc:
    Lang: english
*/
#include "graphics_intern.h"
#include <graphics/rastport.h>

extern void driver_SetAPen (struct RastPort *, ULONG, struct GfxBase *);

/*****************************************************************************

    NAME */
	#include <graphics/rastport.h>
	#include <clib/graphics_protos.h>

	AROS_LH2(void, SetAPen,

/*  SYNOPSIS */
	AROS_LHA(struct RastPort *, rp, A1),
	AROS_LHA(unsigned long    , pen, D0),

/*  LOCATION */
	struct GfxBase *, GfxBase, 57, Graphics)

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
			    graphics_lib.fd and clib/graphics_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct GfxBase *,GfxBase)

    driver_SetAPen (rp, pen, GfxBase);

    rp->FgPen = pen;

    AROS_LIBFUNC_EXIT
} /* SetAPen */
