/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$
    $Log$
    Revision 1.4  1996/10/24 15:51:18  aros
    Use the official AROS macros over the __AROS versions.

    Revision 1.3  1996/10/02 18:10:47  digulla
    Fixed a bug (coordinates are relative to offset and not to last point)

    Revision 1.2  1996/08/29 13:33:30  digulla
    Moved common code from driver to Intuition
    More docs

    Revision 1.1  1996/08/23 17:28:18  digulla
    Several new functions; some still empty.


    Desc:
    Lang: english
*/
#include "intuition_intern.h"
#include <clib/graphics_protos.h>

/*****************************************************************************

    NAME */
	#include <graphics/rastport.h>
	#include <intuition/intuition.h>
	#include <clib/intuition_protos.h>

	AROS_LH4(void, DrawBorder,

/*  SYNOPSIS */
	AROS_LHA(struct RastPort *, rp, A0),
	AROS_LHA(struct Border   *, border, A1),
	AROS_LHA(long             , leftOffset, D0),
	AROS_LHA(long             , topOffset, D1),

/*  LOCATION */
	struct IntuitionBase *, IntuitionBase, 18, Intuition)

/*  FUNCTION
	Draws one or more borders in the specified RastPort. Rendering
	will start at the position which you get when you add the offsets
	leftOffset and topOffset to the LeftEdge and TopEdge specified
	in the Border structure. All coordinates are relative to that point.

    INPUTS
	rp - The RastPort to render into
	border - Information what and how to render
	leftOffset, topOffset - Initial starting position

    RESULT
	None.

    NOTES

    EXAMPLE
	// Draw a house with one stroke
	// The drawing starts at the lower left edge
	WORD XY[] =
	{
	    10, -10,
	    10,   0,
	     0, -10,
	    10, -10,
	     5, -15,
	     0, -10,
	     0,   0,
	    10,   0,
	};
	struct Border demo =
	{
	    100, 100,	// Position
	    1, 2,	// Pens
	    JAM1,	// Drawmode
	    8,		// Number of pairs in XY
	    XY, 	// Vector offsets
	    NULL	// No next border
	};

	// Render the house with the bottom left edge at 150, 50
	DrawBorder (rp, &demo, 50, -50);

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	29-10-95    digulla automatically created from
			    intuition_lib.fd and clib/intuition_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct IntuitionBase *,IntuitionBase)
    ULONG  apen;
    ULONG  bpen;
    ULONG  drmd;
    WORD * ptr;
    WORD   x, y;
    WORD   xoff, yoff;
    int    t;

    /* Store important variables of the RastPort */
    apen = GetAPen (rp);
    bpen = GetBPen (rp);
    drmd = GetDrMd (rp);

    /* For all borders... */
    for ( ; border; border=border->NextBorder)
    {
	/* Change RastPort to the colors/mode specified */
	SetAPen (rp, border->FrontPen);
	SetBPen (rp, border->BackPen);
	SetDrMd (rp, border->DrawMode);

	/* Move to initial position */
	Move (rp
	    , x = border->LeftEdge + leftOffset
	    , y = border->TopEdge + topOffset
	);

	/* Start of vector offsets */
	ptr = border->XY;

	for (t=0; t<border->Count; t++)
	{
	    /* Add vector offset to current position */
	    xoff = *ptr ++;
	    yoff = *ptr ++;

	    /* Stroke */
	    Draw (rp, x + xoff, y + yoff);
	}
    }

    /* Restore RastPort */
    SetAPen (rp, apen);
    SetBPen (rp, bpen);
    SetDrMd (rp, drmd);

    AROS_LIBFUNC_EXIT
} /* DrawBorder */
