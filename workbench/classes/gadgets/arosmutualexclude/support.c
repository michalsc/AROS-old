/*
    (C) 1997 AROS - The Amiga Replacement OS
    $Id$

    Desc: Support functions for AROSMutualExcludeClass.
    Lang: english
*/
#include <strings.h>
#include <exec/types.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <graphics/rastport.h>
#include <graphics/text.h>
#include <libraries/gadtools.h>

#include "arosmutualexclude_intern.h"


UWORD disabledpattern[2] = {0x4444, 0x1111};

/* draws a disabled pattern */
void drawdisabledpattern(struct MXBase_intern *AROSMutualExcludeBase,
			 struct RastPort *rport, UWORD pen,
			 WORD left, WORD top, UWORD width, UWORD height)
{
    SetABPenDrMd(rport, pen, 0, JAM1);
    rport->AreaPtrn = disabledpattern;
    rport->AreaPtSz = 1;
    RectFill(rport, left, top, left+width-1, top+height-1);
}



struct TextFont *preparefont(struct MXBase_intern *AROSMutualExcludeBase,
			     struct RastPort *rport, struct IntuiText *itext,
			     struct TextFont **oldfont)
{
    struct TextFont *font;

    if (itext->ITextFont)
    {
	*oldfont = rport->Font;
	font = OpenFont(itext->ITextFont);
	if (font)
	{
	    SetFont(rport, font);
	    /* SetSoftStyle(rport, itext->ITextFont->ta_Style, 0xffffffff) !!! */
	} else
	    font = rport->Font;
    } else
    {
	*oldfont = NULL;
	font = rport->Font;
    }
    SetABPenDrMd(rport, itext->FrontPen, itext->BackPen, itext->DrawMode);

    return font;
}

void closefont(struct MXBase_intern *AROSMutualExcludeBase,
	       struct RastPort *rport,
	       struct TextFont *font, struct TextFont *oldfont)
{
    if (oldfont)
    {
	SetFont(rport, oldfont);
	CloseFont(font);
    }
}

BOOL renderlabel(struct MXBase_intern *AROSMutualExcludeBase,
		 struct Gadget *gad, struct RastPort *rport, LONG labelplace)
{
    struct TextFont *font, *oldfont;
    struct TextExtent te;
    STRPTR text;
    int len, x, y;
    UWORD width, height;

    if (gad->GadgetText)
    {
        /* Calculate offsets. */
        if ((gad->Flags & GFLG_LABELSTRING))
            text = (STRPTR)gad->GadgetText;
        else if ((gad->Flags & GFLG_LABELIMAGE))
            text = NULL;
        else
        {
            /* GFLG_LABELITEXT */
            text = gad->GadgetText->IText;
            font = preparefont(AROSMutualExcludeBase,
                               rport, gad->GadgetText, &oldfont);
            if (!font)
                return FALSE;
        }

        if (text)
        {
            len = strlen(text);
            TextExtent(rport, text, len, &te);
            width  = te.te_Width;
            height = te.te_Height;
        } else
        {
            width  = ((struct Image *)gad->GadgetText)->Width;
            height = ((struct Image *)gad->GadgetText)->Height;
        }
        if ((labelplace & GV_LabelPlace_Right))
        {
            x = gad->LeftEdge + gad->Width + 5;
            y = gad->TopEdge + ((gad->Height - height) / 2) + 1;
        } else if ((labelplace & GV_LabelPlace_Above))
        {
            x = gad->LeftEdge - ((width - gad->Width) / 2);
            y = gad->TopEdge - height - 2;
        } else if ((labelplace & GV_LabelPlace_Below))
        {
            x = gad->LeftEdge - ((width - gad->Width) / 2);
            y = gad->TopEdge + gad->Height + 3;
        } else if ((labelplace & GV_LabelPlace_In))
        {
            x = gad->LeftEdge - ((width - gad->Width) / 2);
            y = gad->TopEdge + ((gad->Height - height) / 2) + 1;
        } else /* GV_LabelPlace_Left */
        {
            x = gad->LeftEdge - width - 4;
            y = gad->TopEdge + ((gad->Height - height) / 2) + 1;
        }

        y += rport->Font->tf_Baseline;
        if ((gad->Flags & GFLG_LABELSTRING))
        {
            SetABPenDrMd(rport, 1, 0, JAM1);
            Move(rport, x, y);
            Text(rport, text, len);
        } else if ((gad->Flags & GFLG_LABELIMAGE))
            DrawImage(rport, (struct Image *)gad->GadgetText, x, y);
        else
        {
            PrintIText(rport, gad->GadgetText, x, y);
            closefont(AROSMutualExcludeBase, rport, font, oldfont);
        }
    }
    return TRUE;
}
