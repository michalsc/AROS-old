/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: english
*/
#include "intuition_intern.h"
#include "propgadgets.h"

/*****************************************************************************

    NAME */
#include <intuition/intuition.h>
#include <proto/intuition.h>

	AROS_LH9(void, NewModifyProp,

/*  SYNOPSIS */
	AROS_LHA(struct Gadget    *, gadget, A0),
	AROS_LHA(struct Window    *, window, A1),
	AROS_LHA(struct Requester *, requester, A2),
	AROS_LHA(ULONG             , flags, D0),
	AROS_LHA(ULONG             , horizPot, D1),
	AROS_LHA(ULONG             , vertPot, D2),
	AROS_LHA(ULONG             , horizBody, D3),
	AROS_LHA(ULONG             , vertBody, D4),
	AROS_LHA(LONG              , numGad, D5),

/*  LOCATION */
	struct IntuitionBase *, IntuitionBase, 78, Intuition)

/*  FUNCTION
	Changes the values in the PropInfo-structure of a proportional
	gadget and refreshes the specified number of gadgets beginning
	at the proportional gadget. If numGad is 0 (zero), then no
	refreshing is done.

    INPUTS
	gadget - Must be a PROPGADGET
	window - The window which contains the gadget
	requester - If the gadget has GTYP_REQGADGET set, this must be
		non-NULL.
	flags - New flags
	horizPot - New value for the HorizPot field of the PropInfo
	vertPot - New value for the VertPot field of the PropInfo
	horizBody - New value for the HorizBody field of the PropInfo
	vertBody - New value for the VertBody field of the PropInfo
	numGad - How many gadgets to refresh. 0 means none (not even
		the current gadget) and -1 means all of them.

    RESULT
	None.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
	NewModifyProp(), RefreshGadgets(), RefreshGList()

    INTERNALS

    HISTORY
	29-10-95    digulla automatically created from
			    intuition_lib.fd and clib/intuition_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct IntuitionBase *,IntuitionBase)

    struct PropInfo 	*pi;
    struct BBox 	old, new;
    BOOL		knobok1, knobok2;
    
    if ((gadget->GadgetType & GTYP_GTYPEMASK) != GTYP_PROPGADGET
	|| !gadget->SpecialInfo
    )
	return;

    pi = gadget->SpecialInfo;

    CalcBBox (window, gadget, &old);

    new = old;

    knobok1 = CalcKnobSize (gadget, &old);

    pi->Flags = flags;
    pi->HorizPot = horizPot;
    pi->VertPot = vertPot;
    pi->HorizBody = horizBody;
    pi->VertBody = vertBody;

    knobok2 = CalcKnobSize (gadget, &new);

    if (knobok2)
        RefreshPropGadgetKnob (gadget, knobok1 ? &old : 0, &new, window, IntuitionBase);

    if (numGad > 1 && gadget->NextGadget)
	RefreshGList (gadget->NextGadget, window, requester, numGad - 1);

    AROS_LIBFUNC_EXIT
    
} /* NewModifyProp */
