/*
    (C) 1995-99 AROS - The Amiga Research OS
    $Id$

    Desc: ResetMenuStrip()
    Lang: english
*/
#include "intuition_intern.h"

/*****************************************************************************

    NAME */
#include <proto/intuition.h>

	AROS_LH2(BOOL, ResetMenuStrip,

/*  SYNOPSIS */
	AROS_LHA(struct Window *, window, A0),
	AROS_LHA(struct Menu *, menu, A1),

/*  LOCATION */
	struct IntuitionBase *, IntuitionBase, 117, Intuition)

/*  FUNCTION
	Works like a "fast" SetMenuStrip() as it doesn't check Menu or
	calculate internal values before attaching the Menu to the Window.
	Use this function only if the Menu has been added before by
	SetMenuStrip() and you changed nothing in the struct except
	CHECKED and ITEMENABLED flags.

    INPUTS
	window - The window to add the MenuStrip to
	menu   - The menu to be added to the window above.

    RESULT
	Always TRUE.

    NOTES
        Yes, I do repeat it again:
	Use this function only if the Menu has been added before by
	SetMenuStrip() and you changed nothing in the struct except
	CHECKED and ITEMENABLED flags.

    EXAMPLE

    BUGS

    SEE ALSO
	SetMenuStrip(), ClearMenuStrip()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct IntuitionBase *,IntuitionBase)

   window->MenuStrip = menu;

return TRUE;

    AROS_LIBFUNC_EXIT
} /* ResetMenuStrip */
