/*
    Copyright � 2002-2010, The AROS Development Team. All rights reserved.
    Copyright � 2001-2003, The MorphOS Development Team. All Rights Reserved.
    $Id$
*/

#include <aros/debug.h>
#include <aros/macros.h>
#include <exec/memory.h>
#include <graphics/rastport.h>
#include <intuition/pointerclass.h>
#include <prefs/pointer.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include "intuition_intern.h"
#include "monitorclass_private.h"

/* This function does not use MoveSprite() any more because it
   wants to work on empty displays too. Additionally it uses
   physical display coordinates instead of logical screen ones */
void MySetPointerPos(struct IntuitionBase *IntuitionBase, WORD x, WORD y)
{
    Object *mon = GetPrivIBase(IntuitionBase)->ActiveMonitor;
    struct IntScreen *scr = GetPrivScreen(IntuitionBase->ActiveScreen);
    struct SharedPointer *pointer = NULL;

    if (scr)
	pointer = scr->Pointer;
    else {
        Object *obj = GetPrivIBase(IntuitionBase)->DefaultPointer;

	if (obj)
	    GetAttr(POINTERA_SharedPointer, obj, (IPTR *)&pointer);
    }
    if (pointer) {
	DB2(bug("Move pointer to (%d, %d), monitor 0x%p\n", x, y, mon));
	/* Take hotspot into account */
	x += pointer->xoffset;
        y += pointer->yoffset;
	/* Update sprite position, just for backwards compatibility */
	pointer->sprite->es_SimpleSprite.x = x;
	pointer->sprite->es_SimpleSprite.y = y;
    }

    if (mon)
	DoMethod(mon, MM_SetPointerPos, x, y);
}

void ActivateMonitor(Object *newmonitor, struct IntuitionBase *IntuitionBase)
{
    Object *oldmonitor = GetPrivIBase(IntuitionBase)->ActiveMonitor;

    D(bug("ActivateMonitor(0x%p), old monitor 0x%p\n", newmonitor, oldmonitor));
    /* Do not bother if switching to the same monitor */
    if (newmonitor == oldmonitor)
	return;

    if (oldmonitor)
	SetAttrs(oldmonitor, MA_PointerVisible, FALSE, TAG_DONE);

    GetPrivIBase(IntuitionBase)->ActiveMonitor = newmonitor;
    if (newmonitor) {
	SetAttrs(newmonitor, MA_PointerVisible, TRUE, TAG_DONE);
	/* TODO: Set pointer position */
    }
}

struct Screen *FindFirstScreen(Object *monitor, struct IntuitionBase *IntuitionBase)
{
    struct Screen *scr;

    for (scr = IntuitionBase->FirstScreen; scr; scr = scr->NextScreen) {
	if (GetPrivScreen(scr)->MonitorObject == monitor)
	    break;
    }
    return scr;
}

#ifdef __MORPHOS__

/* TODO: there are no such functions in MorphOS/AmigaOS, may be we should make them private too? */

struct RastPort *MyCreateRastPort(struct IntuitionBase *IntuitionBase)
{
    struct RastPort *newrp = AllocMem(sizeof(*newrp), MEMF_PUBLIC);
    
    if (newrp)
    {
        InitRastPort(newrp);
    }
    
    return newrp;
}

struct RastPort *MyCloneRastPort(struct IntuitionBase *IntuitionBase, struct RastPort *rp)
{
    struct RastPort *newrp = NULL;

    if (rp)
    {
        newrp = AllocMem(sizeof(*newrp), MEMF_PUBLIC);
        if (newrp)
        {
            // *newrp = *rp;

            memcpy(newrp,rp,sizeof(struct RastPort));
        }
    }

    return newrp;
}

void MyFreeRastPort(struct IntuitionBase *IntuitionBase, struct RastPort *rp)
{
    FreeMem(rp, sizeof(*rp));
}

BOOL IsLayerHiddenBySibling(struct Layer *layer, BOOL xx)
{
    struct Window *win = layer->Window;
    
    /* skip requesters attached to the same window. */
    while (layer->front && layer->front->Window == win)
    {
        layer = layer->front;
    }

    /* jDc: we need to care for layers that are on
    ** front of our layer, but don't cover it*/

    if (layer->front)
    {
        struct Layer *lay;
	
        for (lay = layer->front; lay; lay = lay->front)
        {
            struct Window *lwin = lay->Window;
	    
            if (lwin && win)
            {
                if (lwin->LeftEdge > win->LeftEdge + win->Width - 1) continue;
                if (lwin->LeftEdge + lwin->Width - 1 < win->LeftEdge) continue;
                if (lwin->TopEdge > win->TopEdge + win->Height - 1) continue;
                if (lwin->TopEdge + lwin->Height - 1 < win->TopEdge) continue;
                return TRUE;
            }
        }
        return NULL;
	
    } else return NULL;
}

#endif

struct TextFont *SafeReopenFont(struct IntuitionBase *IntuitionBase,
                                struct TextFont **fontptr)
{
    struct TextFont *ret = NULL, *font;

    /* Atomically lock the font before, so it can't go away
     */
    Forbid();

    font = *fontptr;
    if (font)
    {
        struct TextAttr ta;

        font->tf_Accessors++;
        Permit();

        /* Now really open it
         */
        ta.ta_Name  = font->tf_Message.mn_Node.ln_Name;
        ta.ta_YSize = font->tf_YSize;
        ta.ta_Style = font->tf_Style;
        ta.ta_Flags = font->tf_Flags;

        ret = OpenFont(&ta);

        /* Unlock it
         */
        Forbid();
        font->tf_Accessors--;
    }

    Permit();

    return ret;
}

Object *MakePointerFromData(struct IntuitionBase *IntuitionBase,
                            UWORD *source, int xOffset, int yOffset, int width, int height)
{
    struct TagItem pointertags[] = {
        {POINTERA_BitMap      , (IPTR)source},
        {POINTERA_XOffset     , xOffset      },
        {POINTERA_YOffset     , yOffset      },
	{SPRITEA_OldDataFormat, TRUE	     },
	{SPRITEA_Width	      , width	     },
	{SPRITEA_OutputHeight , height	     },
        {TAG_DONE                            }
    };

    return NewObjectA(GetPrivIBase(IntuitionBase)->pointerclass, NULL, pointertags);
}

Object *MakePointerFromPrefs(struct IntuitionBase *IntuitionBase, struct Preferences *prefs)
{
    SetPointerColors(IntuitionBase);
    
    return MakePointerFromData(IntuitionBase, prefs->PointerMatrix, prefs->XOffset, prefs->YOffset, 16, 16);
}

void InstallPointer(struct IntuitionBase *IntuitionBase, UWORD which, Object **old, Object *pointer)
{
    struct IntScreen 	*scr;
    struct Window   	*win;
    struct SharedPointer *oldpointer;
    struct SharedPointer *newpointer;
    
    ULONG lock = LockIBase(0);

    GetAttr(POINTERA_SharedPointer, *old, (IPTR *)&oldpointer);
    GetAttr(POINTERA_SharedPointer, pointer, (IPTR *)&newpointer);

    for (scr = GetPrivScreen(IntuitionBase->FirstScreen); scr; scr = GetPrivScreen(scr->Screen.NextScreen))
    {
        for (win = scr->Screen.FirstWindow; win; win = win->NextWindow)
        {
            if (((struct IntWindow *)win)->pointer == *old)
            {
                win->XOffset = newpointer->xoffset;
                win->YOffset = newpointer->yoffset;
            }
        }

        if (scr->Pointer == oldpointer)
        {
            DEBUG_POINTER(dprintf("InstallPointer: scr 0x%lx pointer 0x%lx sprite 0x%lx\n",
                                  scr, pointer, newpointer->sprite));
            if (ChangeExtSprite(&scr->Screen.ViewPort,
                                oldpointer->sprite, newpointer->sprite,
				POINTERA_XOffset, newpointer->xoffset,
				POINTERA_YOffset, newpointer->yoffset,
				TAG_DONE))
            {
                ObtainSharedPointer(newpointer, IntuitionBase);
                ReleaseSharedPointer(oldpointer, IntuitionBase);
                scr->Pointer = newpointer;
            }
            else
            {
                DEBUG_POINTER(dprintf("InstallPointer: can't change pointer.\n"));
            }
        }
    }

    /* Normal pointer image is also set on all empty displays (with zero ViewPorts) */
    if (which == WBP_NORMAL)
	ChangeExtSprite(NULL,
			oldpointer->sprite, newpointer->sprite,
			POINTERA_XOffset, newpointer->xoffset,
			POINTERA_YOffset, newpointer->yoffset,
			TAG_DONE);

    DisposeObject(*old);
    *old = pointer;

    UnlockIBase(lock);
}

void SetPointerColors(struct IntuitionBase *IntuitionBase)
{
    struct Color32 *p;
    int     	   k;
    ULONG   	   lock = LockIBase(0);
    /* Probably this should apply to Workbench screen and not to currently active one? */
    struct Screen *scr = IntuitionBase->ActiveScreen;

    DEBUG_POINTER(dprintf("SetPointerColors()\n");)

    p = GetPrivIBase(IntuitionBase)->Colors;

    if (scr)
    {
#ifndef ALWAYS_ALLOCATE_SPRITE_COLORS
        if (GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH) < 9)
#endif
	{
	    UWORD firstcol = scr->ViewPort.ColorMap->SpriteBase_Even;
	    
	    /* Translate bank number and offset to color number - see graphics/getcolormap.c */
	    firstcol = (firstcol << 4) | (firstcol >> 8);
            for (k = 1; k < 4; ++k, ++p) {
	        DEBUG_POINTER(dprintf("Color %u: R %08lx G %08lx B %08lx\n", p[k+7].red, p[k+7].green, p[k+7].blue);)
		SetRGB32(&scr->ViewPort, k + firstcol, p[k+7].red, p[k+7].green, p[k+7].blue);
	    }
        }
    }

    UnlockIBase(lock);

    DEBUG_POINTER(dprintf("SetPointerColors() done\n");)
}


struct SharedPointer *CreateSharedPointer(struct ExtSprite *sprite, int x, int y,
                    struct IntuitionBase *IntuitionBase)
{
    struct SharedPointer *pointer;

    pointer = AllocMem(sizeof(*pointer), MEMF_PUBLIC);
    if (pointer)
    {
        pointer->sprite = sprite;
        pointer->xoffset = x;
        pointer->yoffset = y;
        pointer->ref_count = 1;
    }

    return pointer;
}

void ObtainSharedPointer(struct SharedPointer *pointer,
                         struct IntuitionBase *IntuitionBase)
{
    ULONG lock = LockIBase(0);
    ++pointer->ref_count;
    UnlockIBase(lock);
}

void ReleaseSharedPointer(struct SharedPointer *pointer,
                          struct IntuitionBase *IntuitionBase)
{
    ULONG lock = LockIBase(0);
    if (--pointer->ref_count == 0)
    {
        FreeSpriteData(pointer->sprite);
        FreeMem(pointer, sizeof(*pointer));
    }
    UnlockIBase(lock);
}
