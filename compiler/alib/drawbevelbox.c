/*
    Copyright (C) 1997-2000 AROS - The Amiga Research OS
    $Id$

    Desc: Draw a bevel box
    Lang: english
*/


#define AROS_TAGRETURNTYPE void
#define AROS_TAGRETURNTYPEVOID

#include "alib_intern.h"

extern struct Library * GadToolsBase;

/*****************************************************************************

    NAME */
#include <graphics/rastport.h>
#define NO_INLINE_STDARG /* turn off inline def */
#include <proto/gadtools.h>

#include <utility/tagitem.h>

	void DrawBevelBox (

/*  SYNOPSIS */
        struct RastPort * rp,
        WORD              left,
        WORD              top,
        WORD              width,
        WORD              height,
	ULONG             tag1,
	...		)

/*  FUNCTION
        Varargs version of gadtools.library/DrawBevelBoxA().

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
    AROS_SLOWSTACKTAGS_PRE(tag1)
    DrawBevelBoxA (rp, left, top, width, height, AROS_SLOWSTACKTAGS_ARG(tag1));
    AROS_SLOWSTACKTAGS_POST
} /* DrawBevelBox */


