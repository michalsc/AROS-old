/*
    (C) 1995-96 AROS - The Amiga Research OS
    $Id$

    Desc: Routines for BOOL Gadgets
    Lang: english
*/

#include <proto/graphics.h>
#include <proto/intuition.h>
#include <intuition/cghooks.h>
#include <intuition/imageclass.h>
#include "intuition_intern.h"
#include <graphics/gfxmacros.h>
#include "gadgets.h"

void RefreshBoolGadget (struct Gadget * gadget, struct Window * window,
	    struct IntuitionBase * IntuitionBase)
{
    struct GadgetInfo 	gi;
    struct RastPort 	*rp;
    struct DrawInfo 	*dri;
    struct BBox 	bbox;
    ULONG		state;
    APTR  		render;


#define RENDERGADGET(win,gad,rend)              \
	if (rend)                               \
	{					\
	    if (gad->Flags & GFLG_GADGIMAGE)    \
	    {					\
		DrawImageState (rp              \
		    , (struct Image *)rend      \
		    , bbox.Left 		\
		    , bbox.Top			\
		    , state			\
		    , dri			\
		);				\
	    }					\
	    else				\
	    {					\
		DrawBorder (rp                  \
		    , (struct Border *)rend     \
		    , bbox.Left 		\
		    , bbox.Top			\
		);				\
	    }					\
	}
#define GETRENDER(gad)  (gad->Flags & GFLG_SELECTED) ? \
			    gad->SelectRender : gad->GadgetRender;


    CalcBBox (window, gadget, &bbox);

    state = GetGadgetState(window, gadget);
        
    SET_GI_RPORT(&gi, window, gadget);
    gi.gi_Layer = gi.gi_RastPort->Layer;
    
    rp = ObtainGIRPort(&gi);
    if (!rp) return;
    
    dri = GetScreenDrawInfo(window->WScreen);
    
    SetDrMd (rp, JAM1);

    switch (gadget->Flags & GFLG_GADGHIGHBITS)
    {
	case GFLG_GADGHIMAGE:
	    render = GETRENDER(gadget);
	    RENDERGADGET(window,gadget,render);
	    break;

	case GFLG_GADGHNONE:
	    render = gadget->GadgetRender;
	    RENDERGADGET(window,gadget,render);
	    break;

    } /* switch GadgetHighlightMethod */

    if (gadget->GadgetText)
    {
	switch (gadget->Flags & GFLG_LABELMASK)
	{

	case GFLG_LABELITEXT:
	    PrintIText (rp
		, gadget->GadgetText
		, bbox.Left
		, bbox.Top
	    );
	    break;

	case GFLG_LABELSTRING: {
	    STRPTR text = (STRPTR) gadget->GadgetText;
	    int len, labelwidth, labelheight;

	    len = strlen (text);

	    labelwidth = LabelWidth (rp, text, len, IntuitionBase);
	    labelheight = rp->Font->tf_YSize;

	    SetAPen (rp, 1);
	    SetDrMd (rp, JAM1);

	    Move (rp
		, bbox.Left + bbox.Width/2 - labelwidth/2
		, bbox.Top + bbox.Height/2 - labelheight/2
		    + rp->Font->tf_Baseline
	    );
	    RenderLabel (rp, text, len, IntuitionBase);

	    break; }

	case GFLG_LABELIMAGE:
	    DrawImageState (rp
		, (struct Image *)gadget->GadgetText
		, bbox.Left
		, bbox.Top
		, IDS_NORMAL
		, dri
	    );
	    break;
	}
    } /* GadgetText */

    if ((bbox.Width >= 1) && (bbox.Height >= 1))
    {
	switch (gadget->Flags & GFLG_GADGHIGHBITS)
	{
	case GFLG_GADGHCOMP:
	    render = gadget->GadgetRender;
	    RENDERGADGET(window,gadget,render);

	    if (gadget->Flags & GFLG_SELECTED)
	    {
		SetDrMd (rp, COMPLEMENT);

		RectFill (rp
		    , bbox.Left
		    , bbox.Top
		    , bbox.Left + bbox.Width - 1
		    , bbox.Top + bbox.Height - 1
		);
	    }

	    break;

	case GFLG_GADGHBOX:
	    render = gadget->GadgetRender;
	    RENDERGADGET(window,gadget,render);

	    if (gadget->Flags & GFLG_SELECTED)
	    {
		SetDrMd (rp, COMPLEMENT);

    #define BOXWIDTH 5
		RectFill (rp
		    , bbox.Left
		    , bbox.Top
		    , bbox.Left + bbox.Width - 1
		    , bbox.Top + bbox.Height - 1
		);

		if (bbox.Width > 2*BOXWIDTH && bbox.Height > 2*BOXWIDTH)
		{
		    RectFill (rp
			, bbox.Left + BOXWIDTH
			, bbox.Top + BOXWIDTH
			, bbox.Left + bbox.Width - BOXWIDTH - 1
			, bbox.Top + bbox.Height - BOXWIDTH - 1
		    );
		}
	    }

	    break;

	} /* Highlight after contents have been drawn */

	if ( gadget->Flags & GFLG_DISABLED )
	{
            RenderDisabledPattern(rp, dri, bbox.Left,
					   bbox.Top,
					   bbox.Left + bbox.Width - 1,
	    				   bbox.Top + bbox.Height - 1,
					   IntuitionBase );
	}
	
    } /* if ((bbox.Width >= 1) && (bbox.Height >= 1)) */
    
    FreeScreenDrawInfo(window->WScreen, dri);
    
    ReleaseGIRPort(rp);
    
} /* RefreshBoolGadget */
