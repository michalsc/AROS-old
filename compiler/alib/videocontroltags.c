/*
    (C) 2000 AROS - The Amiga Research OS
    $Id$

    Desc: Varargs version of graphics.library/VideoControl()
    Lang: english
*/
#define AROS_TAGRETURNTYPE ULONG
#include <graphics/view.h>
#include <utility/tagitem.h>

/*****************************************************************************

    NAME */
#define NO_INLINE_STDARG /* turn off inline def */
#include <proto/graphics.h>

	ULONG VideoControlTags (

/*  SYNOPSIS */
	struct ColorMap * cm,
	Tag tag1,
	...)

/*  FUNCTION
        This is the varargs version of graphics.library/VideoControl().
        For information see graphics.library/VideoControl().

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        graphics/VideoControl()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_SLOWSTACKTAGS_PRE(tag1)
    VideoControl (cm, AROS_SLOWSTACKTAGS_ARG(tag1));
    AROS_SLOWSTACKTAGS_POST
} /* VideoControlTags */
