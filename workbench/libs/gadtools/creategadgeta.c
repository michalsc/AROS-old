/*
    (C) 1997 AROS - The Amiga Replacement OS
    $Id$

    Desc: gadtools.library function CreateGadgetA()
    Lang: english
*/
#include <proto/exec.h>
#include <exec/memory.h>
#include <proto/intuition.h>
#include <intuition/gadgetclass.h>
#include <proto/utility.h>
#include "gadtools_intern.h"

/*********************************************************************

    NAME */
#include <proto/gadtools.h>
#include <exec/types.h>
#include <intuition/intuition.h>
#include <utility/tagitem.h>
#include <libraries/gadtools.h>

        AROS_LH4(struct Gadget *, CreateGadgetA,

/*  SYNOPSIS */
	AROS_LHA(ULONG, kind, D0),
	AROS_LHA(struct Gadget *, previous, A0),
	AROS_LHA(struct NewGadget *, ng, A1),
	AROS_LHA(struct TagItem *, taglist, A2),

/*  LOCATION */
	struct Library *, GadToolsBase, 5, GadTools)

/*  FUNCTION
        Creates a gadtools gadget.

    INPUTS

        kind -     Kind of gadget. See <libraries/gadtools.h> for a list of
                   all possible kinds.
	previous - Pointer to the previous gadget in gadget-list. Create the
	           first "gadget" with CreateContext().
        ng -       Pointer to struct NewGadget. See <libraries/gadtools.h>.
        taglist -  Additional tags. See <libraries/gadtools.h>.

    RESULT
        A pointer to a gadget or NULL to indicate an error.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        CreateContext(), FreeGadgets(), <libraries/gadtools.h>

    INTERNALS

    HISTORY

***************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct GadToolsBase *,GadToolsBase)

    struct Gadget *gad = NULL;
    struct TagItem stdgadtags[] = {
        {GA_Left, 0L},
	{GA_Top, 0L},
	{GA_Width, 0L},
	{GA_Height, 0L},
	{GA_IntuiText, (IPTR)NULL},
        {GA_LabelPlace, (IPTR)GV_LabelPlace_In},
	{GA_Previous, (IPTR)previous},
	{GA_ID, 0L},
	{GA_DrawInfo, (IPTR)NULL},
	{GA_UserData, (IPTR)NULL},
	{TAG_END, 0L}
    };

    if (previous == NULL || ng == NULL || ng->ng_VisualInfo == NULL)
	return (NULL);

    stdgadtags[TAG_Left].ti_Data = ng->ng_LeftEdge;
    stdgadtags[TAG_Top].ti_Data = ng->ng_TopEdge;
    stdgadtags[TAG_Width].ti_Data = ng->ng_Width;
    stdgadtags[TAG_Height].ti_Data = ng->ng_Height;
    stdgadtags[TAG_IText].ti_Data = (IPTR)makeitext((struct GadToolsBase_intern *)GadToolsBase, ng);
    if (stdgadtags[TAG_IText].ti_Data)
    {
        stdgadtags[TAG_Previous].ti_Data = (IPTR)previous;
        stdgadtags[TAG_ID].ti_Data = ng->ng_GadgetID;
        stdgadtags[TAG_DrawInfo].ti_Data = (IPTR)(((struct VisualInfo *)(ng->ng_VisualInfo))->vi_dri);
        stdgadtags[TAG_UserData].ti_Data = (IPTR)ng->ng_UserData;

        /* Calculate label placement.*/
        if ((ng->ng_Flags & PLACETEXT_LEFT))
            stdgadtags[TAG_LabelPlace].ti_Data = GV_LabelPlace_Left;
        else if ((ng->ng_Flags & PLACETEXT_RIGHT))
            stdgadtags[TAG_LabelPlace].ti_Data = GV_LabelPlace_Right;
        else if ((ng->ng_Flags & PLACETEXT_ABOVE))
            stdgadtags[TAG_LabelPlace].ti_Data = GV_LabelPlace_Above;
        else if ((ng->ng_Flags & PLACETEXT_BELOW))
            stdgadtags[TAG_LabelPlace].ti_Data = GV_LabelPlace_Below;

        switch(kind)
        {
        case BUTTON_KIND:
            gad = makebutton((struct GadToolsBase_intern *)GadToolsBase, 
                             stdgadtags,
                             (struct VisualInfo *)ng->ng_VisualInfo,
                             taglist);
            break;
        case CHECKBOX_KIND:
            gad = makecheckbox((struct GadToolsBase_intern *)GadToolsBase,
                               stdgadtags,
                               (struct VisualInfo *)ng->ng_VisualInfo,
                               taglist);
            break;
        case CYCLE_KIND:
            gad = makecycle((struct GadToolsBase_intern *)GadToolsBase,
                            stdgadtags,
                            (struct VisualInfo *)ng->ng_VisualInfo,
                            taglist);
            break;
        case MX_KIND:
            gad = makemx((struct GadToolsBase_intern *)GadToolsBase,
                         stdgadtags,
                         (struct VisualInfo *)ng->ng_VisualInfo,
                         taglist);
            break;
        case PALETTE_KIND:
            gad = makepalette((struct GadToolsBase_intern *)GadToolsBase,
                         stdgadtags,
                         (struct VisualInfo *)ng->ng_VisualInfo,
                         taglist);
            break;
        case TEXT_KIND:

            gad = maketext((struct GadToolsBase_intern *)GadToolsBase,
                         stdgadtags,
                         (struct VisualInfo *)ng->ng_VisualInfo,
                         ng->ng_TextAttr,
                         taglist);
            break;
        case NUMBER_KIND:
            gad = makenumber((struct GadToolsBase_intern *)GadToolsBase,
                         stdgadtags,
                         (struct VisualInfo *)ng->ng_VisualInfo,
                         ng->ng_TextAttr,
                         taglist);
            break;
        case SLIDER_KIND:
            gad = makeslider((struct GadToolsBase_intern *)GadToolsBase,
                         stdgadtags,
                         (struct VisualInfo *)ng->ng_VisualInfo,
                         ng->ng_TextAttr,
                         taglist);

            break;

        case SCROLLER_KIND:
            gad = makescroller((struct GadToolsBase_intern *)GadToolsBase,
                         stdgadtags,
                         (struct VisualInfo *)ng->ng_VisualInfo,
                         taglist);
	break;
        }
    }

    if (gad)
	gad->GadgetType |= GTYP_GADTOOLS;
    else
        FreeVec((APTR)stdgadtags[TAG_IText].ti_Data);

    return (gad);
    AROS_LIBFUNC_EXIT
} /* CreateGadgetA */
