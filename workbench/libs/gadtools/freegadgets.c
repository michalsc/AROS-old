/*
    (C) 1997 AROS - The Amiga Replacement OS
    $Id$

    Desc:
    Lang: english
*/
#include <proto/exec.h>
#include "gadtools_intern.h"

/*********************************************************************

    NAME */
#include <proto/gadtools.h>
#include <intuition/intuition.h>

	AROS_LH1(VOID, FreeGadgets,

/*  SYNOPSIS */
	AROS_LHA(struct Gadget *, glist, A0),

/*  LOCATION */
	struct Library *, GadToolsBase, 6, GadTools)

/*  FUNCTION
	Frees all gadtools gadgets in a linked list of gadgets.

    INPUTS
	glist - pointer to the first gadget to be freed, may be NULL.

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
	CreateGadgetA()

    INTERNALS

    HISTORY

***************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct GadToolsBase *,GadToolsBase)

    struct Gadget *lastgad = NULL, *nextgad = NULL;

    if (!glist)
	return;

    for (;nextgad;glist=nextgad)
    {
	nextgad = glist->NextGadget;
	if ((glist->Flags & GTYP_GADTOOLS))
	{
            freeitext((struct GadToolsBase_intern *)GadToolsBase, glist->GadgetText);
            glist->GadgetText = NULL;
	    DisposeObject(glist);
	}
	else
	{
	    if (lastgad != NULL)
		lastgad->NextGadget = glist;
	    lastgad = glist;
	}
    }

    if (lastgad != NULL)
	lastgad->NextGadget = NULL;

    AROS_LIBFUNC_EXIT
} /* FreeGadgets */
