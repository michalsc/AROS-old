/*
    (C) 1997 AROS - The Amiga Replacement OS
    $Id$

    Desc: Intuition function FreeSysRequest()
    Lang: english
*/

#include "intuition_intern.h"
#include <proto/exec.h>

/*****************************************************************************

    NAME */
#include <proto/intuition.h>
#include <exec/types.h>
#include <intuition/intuition.h>

	AROS_LH1(void, FreeSysRequest,

/*  SYNOPSIS */
	AROS_LHA(struct Window *, window, A0),

/*  LOCATION */
	struct IntuitionBase *, IntuitionBase, 62, Intuition)

/*  FUNCTION
	Frees a requester made with BuildSysRequest() or
	BuildEasyRequestArgs().

    INPUTS
	Window - The requester to be freed. May be NULL or 1.

    RESULT

    NOTES

    EXAMPLE

    BUGS
	BuildSysRequest() requesters not supported, yet.

    SEE ALSO
	BuildSysRequest(), BuildEasyRequestArgs()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct IntuitionBase *,IntuitionBase)

    struct Screen *scr;
    struct Gadget *gadgets;
    STRPTR *gadgetlabels;

    if ((window == NULL) || (window == (void *)1L))
	return;

    scr = window->WScreen;
    gadgets = window->FirstGadget;
    gadgetlabels = ((struct EasyRequestUserData *)window->UserData)->GadgetLabels;

    FreeVec(window->UserData);
    CloseWindow(window);
    easyrequest_freegadgets(gadgets);
    easyrequest_freelabels(gadgetlabels);
    UnlockPubScreen(NULL, scr);

    return;
    AROS_LIBFUNC_EXIT
} /* FreeSysRequest */
