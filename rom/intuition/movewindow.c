/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$

    Desc: Move a window around on the screen
    Lang: english
*/
#include "intuition_intern.h"

/*****************************************************************************

    NAME */
	#include <clib/intuition_protos.h>

	AROS_LH3(void, MoveWindow,

/*  SYNOPSIS */
	AROS_LHA(struct Window *, window, A0),
	AROS_LHA(LONG           , dx, D0),
	AROS_LHA(LONG           , dy, D1),

/*  LOCATION */
	struct IntuitionBase *, IntuitionBase, 28, Intuition)

/*  FUNCTION
	Change the position of a window on the screen.

    INPUTS
	window - Move this window
	dx, dy - Move it that many pixels along the axis (right, down)

    RESULT
	The window will move when the next input event will be received.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
	SizeWindow()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct IntuitionBase *,IntuitionBase)

    intui_MoveWindow (window, dx, dy);

    AROS_LIBFUNC_EXIT
} /* MoveWindow */
