/*
    (C) 1997 AROS - The Amiga Research OS
    $Id$

    Desc: kbd HIDD for AROS.
    Lang: English.
*/

#include <proto/oop.h>
#include <proto/exec.h>
#include <exec/memory.h>

#include "kbd.h"

#undef OOPBase

/* Support functions */

VOID releaseattrbases(struct abdescr *abd, struct Library * OOPBase)
{
    for (; abd->interfaceid; abd ++)
    {
        if ( *(abd->attrbase) != 0 )
	{
	    OOP_ReleaseAttrBase(abd->interfaceid);
	    *(abd->attrbase) = 0;
	}
    }
    return;
}

BOOL obtainattrbases(struct abdescr *abd, struct Library *OOPBase)
{
    struct abdescr *d;
    for (d = abd; d->interfaceid; d ++)
    {
        *(d->attrbase) = OOP_ObtainAttrBase(abd->interfaceid);
	if ( *(d->attrbase) == 0 )
	{
	    releaseattrbases(abd, OOPBase);
	    return FALSE;
	}   
    }
    return TRUE;
    
}

#undef OOPBase
#define OOPBase ((struct Library *)OOP_OCLASS(OOP_OCLASS(o))->UserData)

VOID Hidd_Kbd_HandleEvent(OOP_Object *o, ULONG event)
{
    struct pHidd_Kbd_HandleEvent msg;
    static OOP_MethodID mid = 0;

    if (!mid)
    	mid = OOP_GetMethodID(IID_Hidd_HwKbd, moHidd_Kbd_HandleEvent);

    msg.mID = mid;
    msg.event = event;

    OOP_DoMethod(o, (OOP_Msg) &msg);
}
