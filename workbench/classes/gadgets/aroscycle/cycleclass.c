/*
    (C) 1997-98 AROS - The Amiga Research OS
    $Id$

    Desc: AROS specific cycle class implementation.
    Lang: english
*/
#include <exec/libraries.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <proto/graphics.h>
#include <graphics/rastport.h>
#include <graphics/text.h>
#include <proto/utility.h>
#include <utility/tagitem.h>
#include <devices/inputevent.h>
#include <gadgets/aroscycle.h>
#include <proto/alib.h>

#ifndef DEBUG
#   define DEBUG 0
#endif
#include <aros/debug.h>

#include "aroscycle_intern.h"


#define IM(o) ((struct Image *)(o))
#define EG(o) ((struct Gadget *)(o))


#undef AROSCycleBase
#define AROSCycleBase ((struct CycleBase_intern *)(cl->cl_UserData))



Object *cycle_new(Class *cl, Class *rootcl, struct opSet *msg)
{
    struct CycleData *data;
    Object *o;
    struct TagItem imgtags[] = {
        { IA_Width, 0 },
        { IA_Height, 0 },
        { IA_EdgesOnly, TRUE },
        { TAG_DONE, 0UL }
    };
    STRPTR *labels;

    o = (Object *)DoSuperMethodA(cl, (Object *)rootcl, (Msg)msg);
    if (!o)
        return NULL;

    data = INST_DATA(cl, o);
    data->active = GetTagData(AROSCYCLE_Active, 0, msg->ops_AttrList);
    data->labels = (STRPTR *)GetTagData(AROSCYCLE_Labels, NULL, msg->ops_AttrList);
    data->numlabels = 0;
    labels = data->labels;
    if (labels)
    {
        while (labels[0])
        {
            data->numlabels++;
            labels++;
        }
    }

    imgtags[0].ti_Data = (IPTR)EG(o)->Width;
    imgtags[1].ti_Data = (IPTR)EG(o)->Height;
    EG(o)->GadgetRender = NewObjectA(NULL, FRAMEICLASS, imgtags);
    if (!EG(o)->GadgetRender)
    {
        IPTR methodid = OM_DISPOSE;
        CoerceMethodA(cl, o, (Msg)&methodid);
        return NULL;
    }

    return o;
}


VOID cycle_dispose(Class *cl, Object *o, Msg msg)
{
    if (EG(o)->GadgetRender)
        DisposeObject(EG(o)->GadgetRender);
    DoSuperMethodA(cl,o,msg);
}


IPTR cycle_set(Class *cl, Object *o, struct opSet *msg)
{
    struct TagItem *tag, *taglist = msg->ops_AttrList;
    BOOL rerender = FALSE;
    IPTR result;
    struct CycleData *data = INST_DATA(cl, o);

    result = DoSuperMethodA(cl, o, (Msg)msg);

    while ((tag = NextTagItem(&taglist)))
    {
        switch(tag->ti_Tag)
        {
        case AROSCYCLE_Labels:{
            STRPTR *mylabels;

            data->labels = (STRPTR *)tag->ti_Data;
            data->numlabels = 0;
            mylabels = data->labels;
            if (mylabels)
            {
                while (mylabels[0])
                {
                    data->numlabels++;
                    mylabels++;
                }
            }
            rerender = TRUE;
            break;}
        case AROSCYCLE_Active:
            data->active = tag->ti_Data;
            rerender = TRUE;
            break;
        }
    }


    if ((rerender) && (cl == OCLASS(o)))
    {
        struct RastPort *rport;

        if (data->active > data->numlabels-1)
            data->active = 0;

        rport = ObtainGIRPort(msg->ops_GInfo);
        if (rport)
        {
            DoMethod(o, GM_RENDER, msg->ops_GInfo, rport, GREDRAW_UPDATE);
            ReleaseGIRPort(rport);
            result = FALSE;
        }
    }
    return result;
}


VOID cycle_render(Class *cl, Object *o, struct gpRender *msg)
{
    struct CycleData *data = INST_DATA(cl, o);

    /* Full redraw: clear and draw border */
    SetAPen(msg->gpr_RPort, msg->gpr_GInfo->gi_DrInfo->dri_Pens[EG(o)->Flags&GFLG_SELECTED?FILLPEN:BACKGROUNDPEN]);
    SetDrMd(msg->gpr_RPort, JAM1);
    RectFill(msg->gpr_RPort,
             EG(o)->LeftEdge, EG(o)->TopEdge,
             EG(o)->LeftEdge+EG(o)->Width-1, EG(o)->TopEdge+EG(o)->Height-1);
    DrawImageState(msg->gpr_RPort,IM(EG(o)->GadgetRender),
                   EG(o)->LeftEdge, EG(o)->TopEdge,
                   EG(o)->Flags&GFLG_SELECTED?IDS_SELECTED:IDS_NORMAL,
                   msg->gpr_GInfo->gi_DrInfo);

    if (data->labels)
        renderlabel(AROSCycleBase, EG(o),
                    data->labels[data->active],
                    msg->gpr_RPort, msg->gpr_GInfo);
		    
    /* Draw disabled pattern */
    if (G(o)->Flags & GFLG_DISABLED)
        drawdisabledpattern(AROSCycleBase, msg->gpr_RPort,
                            msg->gpr_GInfo->gi_DrInfo->dri_Pens[SHADOWPEN],
                            G(o)->LeftEdge, G(o)->TopEdge,
                            G(o)->Width, G(o)->Height);

}


IPTR cycle_handleinput(Class *cl, Object *o, struct gpInput *msg)
{
    IPTR retval = GMR_MEACTIVE;
    struct RastPort *rport;
    struct CycleData *data;

    data = INST_DATA(cl, o);
    
    if (msg->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE)
    {
        if (msg->gpi_IEvent->ie_Code == SELECTUP)
        {
            if (G(o)->Flags & GFLG_SELECTED)
            {
                /* mouse is over gadget */
		
        	data->active++;
        	if (data->active == data->numlabels)
                    data->active = 0;

		
                *msg->gpi_Termination = data->active;
                retval = GMR_NOREUSE | GMR_VERIFY;
            } else
                /* mouse is not over gadget */
                retval = GMR_NOREUSE;
		
            G(o)->Flags &= ~GFLG_SELECTED;
	    
            rport = ObtainGIRPort(msg->gpi_GInfo);
            if (rport)
            {
        	struct gpRender rmsg =
                    { GM_RENDER, msg->gpi_GInfo, rport, GREDRAW_UPDATE };
        	DoMethodA(o, (Msg)&rmsg);
        	ReleaseGIRPort(rport);
            }

        } else if (msg->gpi_IEvent->ie_Code == IECODE_NOBUTTON)
        {
            struct gpHitTest htmsg =
                { GM_HITTEST, msg->gpi_GInfo,
                  { msg->gpi_Mouse.X, msg->gpi_Mouse.Y },
                };
            if (DoMethodA(o, (Msg)&htmsg) != GMR_GADGETHIT)
            {
                if (EG(o)->Flags & GFLG_SELECTED)
                {
                    G(o)->Flags &= ~GFLG_SELECTED;
                    rport = ObtainGIRPort(msg->gpi_GInfo);
                    if (rport)
                    {
                        struct gpRender rmsg =
                            { GM_RENDER, msg->gpi_GInfo, rport, GREDRAW_UPDATE };
                        DoMethodA(o, (Msg)&rmsg);
                        ReleaseGIRPort(rport);
                    }
                }
            } else
            {
                if (!(EG(o)->Flags & GFLG_SELECTED))
                {
                    EG(o)->Flags |= GFLG_SELECTED;
                    rport = ObtainGIRPort(msg->gpi_GInfo);
                    if (rport)
                    {
                        struct gpRender rmsg =
                            { GM_RENDER, msg->gpi_GInfo, rport, GREDRAW_UPDATE };
                        DoMethodA(o, (Msg)&rmsg);
                        ReleaseGIRPort(rport);
                    }
                }
            }
        } else if (msg->gpi_IEvent->ie_Code == MENUDOWN)
            retval = GMR_REUSE;
    }
    return retval;
}



AROS_UFH3S(IPTR, dispatch_cycleclass,
	  AROS_UFHA(Class *, cl, A0),
	  AROS_UFHA(Object *, o, A2),
	  AROS_UFHA(Msg, msg, A1)
)
{
    IPTR retval = 0UL;
    struct CycleData *data;

    switch (msg->MethodID) {
    case OM_NEW:
	retval = (IPTR)cycle_new(cl, (Class *)o, (struct opSet *)msg);
	break;

    case OM_DISPOSE:
        cycle_dispose(cl, o, msg);
        break;

    case OM_SET:
        retval = cycle_set(cl, o, (struct opSet *)msg);
        break;

    case GM_RENDER:
        cycle_render(cl, o, (struct gpRender *)msg);
        break;

#define GPGA(x) ((struct gpInput *)(x))
    case GM_GOACTIVE:{
        struct RastPort *rport;

        data = INST_DATA(cl, o);
        EG(o)->Flags = GFLG_SELECTED;
        rport = ObtainGIRPort(GPGA(msg)->gpi_GInfo);
        if (rport)
          {
            struct gpRender rmsg =
                { GM_RENDER, GPGA(msg)->gpi_GInfo, rport, GREDRAW_UPDATE };
            DoMethodA(o, (Msg)&rmsg);
            ReleaseGIRPort(rport);
            retval = GMR_MEACTIVE;
        } else
            retval = GMR_NOREUSE;
        break;}

#if 0
/* stegerg: now done in GM_HANDLEINPUT */

#define GPGI(x) ((struct gpGoInactive *)(x))
    case GM_GOINACTIVE:{
        struct RastPort *rport;

        data = INST_DATA(cl, o);
        if (EG(o)->Flags & GFLG_SELECTED)
        {
            data->active++;
            if (data->active == data->numlabels)
                data->active = 0;
        }
        EG(o)->Flags &= ~GFLG_SELECTED;
        rport = ObtainGIRPort(GPGI(msg)->gpgi_GInfo);
        if (rport)
        {
            struct gpRender rmsg =
                { GM_RENDER, GPGI(msg)->gpgi_GInfo, rport, GREDRAW_UPDATE };
            DoMethodA(o, (Msg)&rmsg);
            ReleaseGIRPort(rport);
        }
        break;}
#endif

    case GM_HANDLEINPUT:
        retval = cycle_handleinput(cl, o, (struct gpInput *)msg);
        break;

    default:
	retval = DoSuperMethodA(cl, o, msg);
	break;
    }

    return retval;
}

/*************************** Classes *****************************/

#undef AROSCycleBase

struct IClass *InitCycleClass (struct CycleBase_intern * AROSCycleBase)
{
    Class *cl = NULL;

    cl = MakeClass(AROSCYCLECLASS, GADGETCLASS, NULL, sizeof(struct CycleData), 0);
    if (cl) {
	cl->cl_Dispatcher.h_Entry    = (APTR)AROS_ASMSYMNAME(dispatch_cycleclass);
	cl->cl_Dispatcher.h_SubEntry = NULL;
	cl->cl_UserData 	     = (IPTR)AROSCycleBase;

	AddClass (cl);
    }

    return (cl);
}
