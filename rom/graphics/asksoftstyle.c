/*
    (C) 1998-99 AROS - The Amiga Replacement OS
    $Id$

    Desc: Graphics function AskSoftStyle()
    Lang: English
*/

/*****************************************************************************

    NAME */
#include <graphics/rastport.h>
#include <proto/graphics.h>

	AROS_LH1I(ULONG, AskSoftStyle,

/*  SYNOPSIS */
	AROS_LHA(struct RastPort *, rp, A1),

/*  LOCATION */
	struct GfxBase *, GfxBase, 14, Graphics)

/*  FUNCTION

    Query algorithmically generated style attributes. These are the bits
    valid to set via SetSoftStyle().

    INPUTS

    pr   --  pointer to rastport

    RESULT

    Algorithmically generated style bits (bits not defined are also set).

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    SetSoftStyle(), graphics/text.h

    INTERNALS

    HISTORY

    24.7.98  SDuvan  implemented

*****************************************************************************/


{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct GfxBase *, GfxBase)
      
    if(rp->Font == NULL)
	return 0;
    
    return ~rp->Font->tf_Style;
    
    AROS_LIBFUNC_EXIT
} /* AskSoftStyle */
