/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$    $Log

    Desc: Graphics function SetRast()
    Lang: english
*/
#include "graphics_intern.h"
#include <graphics/rastport.h>

/*****************************************************************************

    NAME */
#include <graphics/rastport.h>
#include <proto/graphics.h>

	AROS_LH2(void, SetRast,

/*  SYNOPSIS */
	AROS_LHA(struct RastPort *, rp, A1),
	AROS_LHA(ULONG            , pen, D0),

/*  LOCATION */
	struct GfxBase *, GfxBase, 39, Graphics)

/*  FUNCTION

    Set the entire contents of a specified RastPort to a specific colour.

    INPUTS

    rp   --  pointer to the RastPort in question
    pen  --  pen number to set the bitmap pixels to

    RESULT

    All pixels are set to the colour corresponding to the specified pen
    number.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    RectFill(), <graphics/rastport.h>

    INTERNALS

    HISTORY
	29-10-95    digulla automatically created from
			    graphics_lib.fd and clib/graphics_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct GfxBase *,GfxBase)

    driver_SetRast (rp, pen, GfxBase);

    AROS_LIBFUNC_EXIT
} /* SetRast */
