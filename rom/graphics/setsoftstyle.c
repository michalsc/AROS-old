/*
    (C) 1998-99 AROS - The Amiga Replacement OS
    $Id$

    Desc: Graphics function SetSoftStyle()
    Lang: English
*/

/*****************************************************************************

    NAME */
#include <graphics/rastport.h>
#include <proto/graphics.h>

	AROS_LH3I(ULONG, SetSoftStyle,

/*  SYNOPSIS */
	AROS_LHA(struct RastPort *, rp    , A1),
	AROS_LHA(ULONG            , style , D0),
	AROS_LHA(ULONG            , enable, D1),

/*  LOCATION */
	struct GfxBase *, GfxBase, 15, Graphics)

/*  FUNCTION

    Set the style of the current font. Only those bits set in 'enable' are
    affected.

    INPUTS

    rp     --  pointer to rastport
    style  --  the style the font should have
    enable --  mask for style bits

    RESULT

    The style bits used hereinafter (the font may not support all the styles
    you wish to set). Note that this is possibly more style bits than you
    affected by calling SetSoftStyle() as a font may have intrinsic style
    bits set.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    AskSoftStyle(), graphics/text.h

    INTERNALS

    HISTORY

    24.7.98  SDuvan  implemented

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct GfxBase *, GfxBase)

    ULONG  realEnable = enable & AskSoftStyle(rp);

    rp->AlgoStyle = ((~realEnable & rp->AlgoStyle) | (realEnable & style));

    return rp->AlgoStyle | rp->Font->tf_Style;
    
    AROS_LIBFUNC_EXIT
} /* SetSoftStyle */
