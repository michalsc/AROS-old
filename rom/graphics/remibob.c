/*
    (C) 1995-99 AROS - The Amiga Research OS
    $Id$

    Desc: Remove a Bob from the gel list an the RastPort
    Lang: english
*/
#include "graphics_intern.h"

/*****************************************************************************

    NAME */
	#include <clib/graphics_protos.h>

	AROS_LH3(void, RemIBob,

/*  SYNOPSIS */
	AROS_LHA(struct Bob *,      bob, A0),
	AROS_LHA(struct RastPort *, rp , A1),
	AROS_LHA(struct ViewPort *, vp , A2),

/*  LOCATION */
	struct GfxBase *, GfxBase, 22, Graphics)

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
    AROS_LIBBASE_EXT_DECL(struct GfxBase *,GfxBase)


#warning TODO: Write graphics/RemIBob()
    aros_print_not_implemented ("RemIBob");

    AROS_LIBFUNC_EXIT
} /* RemIBob */
