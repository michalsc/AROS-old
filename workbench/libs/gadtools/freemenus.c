/*
    (C) 1999 AROS - The Amiga Research OS
    $Id$

    Desc: Free memory allocated by CreateMenusA()
    Lang: english
*/
#include <proto/exec.h>
#include "gadtools_intern.h"

/*********************************************************************

    NAME */
#include <proto/gadtools.h>
#include <intuition/intuition.h>

	AROS_LH1(VOID, FreeMenus,

/*  SYNOPSIS */
	AROS_LHA(struct Menu *, menu, A0),

/*  LOCATION */
	struct Library *, GadToolsBase, 9, GadTools)

/*  FUNCTION
	Frees the menus allocated by CreateMenusA().

    INPUTS
	menu - pointer to the menu (or first MenuItem) to be freed, may be NULL.

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
	CreateMenusA()

    INTERNALS

    HISTORY

***************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct GadToolsBase *,GadToolsBase)

#warning TODO: Write gadtools/FreeMenus()
    aros_print_not_implemented ("FreeMenus");

    AROS_LIBFUNC_EXIT
} /* FreeMenus */
