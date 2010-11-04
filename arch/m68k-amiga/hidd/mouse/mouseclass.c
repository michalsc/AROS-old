/*
    Copyright � 1995-2006, The AROS Development Team. All rights reserved.
    $Id$

    Desc: The main mouse class.
    Lang: English.
*/

#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/oop.h>
#include <oop/oop.h>
#include <proto/potgo.h>

#include <exec/alerts.h>
#include <exec/memory.h>

#include <hidd/hidd.h>
#include <hidd/mouse.h>

#include <hardware/custom.h>
#include <hardware/cia.h>
#include <hardware/intbits.h>

#include <devices/inputevent.h>
#include <string.h>

#include <aros/symbolsets.h>

#include "mouse.h"

#include LC_LIBDEFS_FILE

#define DEBUG 1
#include <aros/debug.h>

#ifdef HiddMouseAB
#undef HiddMouseAB
#endif
#define HiddMouseAB	(MSD(cl)->hiddMouseAB)

/* defines for buttonstate */

#define LEFT_BUTTON 	1
#define RIGHT_BUTTON 	2
#define MIDDLE_BUTTON	4

static AROS_UFH4(ULONG, mouse_vblank,
    AROS_UFHA(ULONG, dummy, A0),
    AROS_UFHA(void *, data, A1),
    AROS_UFHA(ULONG, dummy2, A5),
    AROS_UFHA(struct ExecBase *, mySysBase, A6))
{ 
    AROS_USERFUNC_INIT

	struct mouse_data *mousedata = (struct mouse_data*)data;
	volatile struct Custom *custom = (struct Custom*)0xdff000;
    volatile struct CIA *cia = (struct CIA*)0xbfe001;
    struct pHidd_Mouse_Event *e = &mousedata->event;
    UWORD potinp = custom->potinp;
    UWORD joydat = mousedata->port ? custom->joy1dat : custom->joy0dat;
    UWORD buttons = 0;
    BYTE x, y;

	x = (BYTE)(joydat >> 8);
	x -= (BYTE)(mousedata->joydat >> 8);
	y = (BYTE)(joydat & 0xff);
	y -= (BYTE)(mousedata->joydat & 0xff);
	
	mousedata->joydat = joydat;

	e->x = x;
	e->y = y;
	if (e->x || e->y) {
		e->button = vHidd_Mouse_NoButton;
		e->type = vHidd_Mouse_Motion;
		mousedata->mouse_callback(mousedata->callbackdata, e);
	}	

    if ((cia->ciapra & (0x40 << mousedata->port)) == 0)
    	buttons |= LEFT_BUTTON;
    if ((potinp & (0x0400 << (mousedata->port * 4))) == 0)
    	buttons |= RIGHT_BUTTON;
    if ((potinp & (0x0100 << (mousedata->port * 4))) == 0)
    	buttons |= MIDDLE_BUTTON;
    	
	if (buttons != mousedata->buttons) {
		int i;
		for (i = 0; i < 3; i++) {
			if ((buttons & (1 << i)) != (mousedata->buttons & (1 << i))) {
                e->button = vHidd_Mouse_Button1 + i;
                e->type = (buttons & (1 << i)) ? vHidd_Mouse_Press : vHidd_Mouse_Release;
                mousedata->mouse_callback(mousedata->callbackdata, e);
            }
        }
        mousedata->buttons = buttons;
    }

	return 0;
	
	AROS_USERFUNC_EXIT
}

/***** Mouse::New()  ***************************************/
OOP_Object * AmigaMouse__Root__New(OOP_Class *cl, OOP_Object *o, struct pRoot_New *msg)
{
    BOOL has_mouse_hidd = FALSE;

    EnterFunc(bug("_Mouse::New()\n"));

    ObtainSemaphoreShared( &MSD(cl)->sema);

    if (MSD(cl)->mousehidd)
        has_mouse_hidd = TRUE;

    ReleaseSemaphore( &MSD(cl)->sema);

    if (has_mouse_hidd) /* Cannot open twice */
        ReturnPtr("_Mouse::New", OOP_Object *, NULL); /* Should have some error code here */

    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
    if (o)
    {
        struct mouse_data   *data = OOP_INST_DATA(cl, o);
        struct TagItem      *tag, *tstate;
        struct Interrupt *inter;
        volatile struct Custom *custom = (struct Custom*)0xdff00;
        volatile struct CIA *cia = (struct CIA*)0xbfe001;
        struct PotgoBase *PotgoBase;
        UWORD potgobits;

        tstate = msg->attrList;

        /* Search for all mouse attrs */

        while ((tag = NextTagItem((const struct TagItem **)&tstate)))
        {
            ULONG idx;

            if (IS_HIDDMOUSE_ATTR(tag->ti_Tag, idx))
            {
                switch (idx)
                {
                    case aoHidd_Mouse_IrqHandler:
                        data->mouse_callback = (APTR)tag->ti_Data;
                        break;

                    case aoHidd_Mouse_IrqHandlerData:
                        data->callbackdata = (APTR)tag->ti_Data;
                        break;
                }
            }

        } /* while (tags to process) */



		PotgoBase = OpenResource("potgo.resource");
		if (!PotgoBase)
			Alert(AT_DeadEnd | AG_OpenRes | AN_Unknown);

		MSD(cl)->potgo = PotgoBase;

		data->port = 0;
		potgobits = 0x0f00 << (data->port * 4);
		if (AllocPotBits(potgobits) != potgobits)
			Alert(AT_DeadEnd | AG_NoMemory | AN_Unknown);
			
		WritePotgo(potgobits, potgobits);
		cia->ciaddra |= 0x40 << data->port; // left button line = output
		custom->potgo = 0xff00; // other buttons = output
		data->joydat = data->port ? custom->joy1dat : custom->joy0dat;
		data->potinp = custom->potinp;
		data->ciapra = cia->ciapra;
   
   		MSD(cl)->potgobits = potgobits;
  		inter = &MSD(cl)->mouseint;
 
       	inter->is_Code         = (APTR)mouse_vblank;
    	inter->is_Data         = data;
    	inter->is_Node.ln_Name = "Mouse VBlank server";
    	inter->is_Node.ln_Pri  = 10;
    	inter->is_Node.ln_Type = NT_INTERRUPT;
	
 		AddIntServer(INTB_VERTB, inter);
   
       	ObtainSemaphore( &MSD(cl)->sema);
        MSD(cl)->mousehidd = o;
        ReleaseSemaphore( &MSD(cl)->sema);
    }

    return o;
}

VOID AmigaMouse__Root__Dispose(OOP_Class *cl, OOP_Object *o, OOP_Msg msg)
{
    ObtainSemaphore(&MSD(cl)->sema);
    MSD(cl)->mousehidd = NULL;
    if (MSD(cl)->potgo) {
    	struct PotgoBase *PotgoBase = MSD(cl)->potgo;
	    RemIntServer(INTB_VERTB, &MSD(cl)->mouseint);
	    FreePotBits(MSD(cl)->potgobits);
	}
    ReleaseSemaphore( &MSD(cl)->sema);

    OOP_DoSuperMethod(cl, o, msg);
}

/***** Mouse::Get()  ***************************************/
VOID AmigaMouse__Root__Get(OOP_Class *cl, OOP_Object *o, struct pRoot_Get *msg)
{
    struct mouse_data *data = OOP_INST_DATA(cl, o);
    ULONG   	       idx;

    if (IS_HIDDMOUSE_ATTR(msg->attrID, idx))
    {
	switch (idx)
	{
	    case aoHidd_Mouse_IrqHandler:
		*msg->storage = (IPTR)data->mouse_callback;
		return;

	    case aoHidd_Mouse_IrqHandlerData:
		*msg->storage = (IPTR)data->callbackdata;
		return;

	    case aoHidd_Mouse_State:
		return;

    	case aoHidd_Mouse_RelativeCoords:
	    *msg->storage = TRUE;
	    return;
	}

    }

    OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
}

/***** Mouse::HandleEvent()  ***************************************/

VOID AmigaMouse__Hidd_Mouse__HandleEvent(OOP_Class *cl, OOP_Object *o, struct pHidd_Mouse_HandleEvent *msg)
{
    struct mouse_data * data;

    EnterFunc(bug("_mouse_handleevent()\n"));

    data = OOP_INST_DATA(cl, o);

    /* Nothing done yet */

    ReturnVoid("_Mouse::HandleEvent");
}

/********************  init_kbdclass()  *********************************/

static int AmigaMouse_InitAttrs(LIBBASETYPEPTR LIBBASE)
{
    struct OOP_ABDescr attrbases[] =
    {
        { IID_Hidd_Mouse, &LIBBASE->msd.hiddMouseAB },
        { NULL	    	, NULL      	    }
    };

    EnterFunc(bug("AmigaMouse_InitAttrs\n"));

    ReturnInt("AmigaMouse_InitAttr", ULONG, OOP_ObtainAttrBases(attrbases));
}

/*************** free_kbdclass()  **********************************/
static int AmigaMouse_ExpungeAttrs(LIBBASETYPEPTR LIBBASE)
{
    struct OOP_ABDescr attrbases[] =
    {
        { IID_Hidd_Mouse, &LIBBASE->msd.hiddMouseAB },
        { NULL	    	, NULL      	    }
    };

    EnterFunc(bug("AmigaMouse_InitClass\n"));

    OOP_ReleaseAttrBases(attrbases);

    return TRUE;
}

ADD2INITLIB(AmigaMouse_InitAttrs, 0)
ADD2EXPUNGELIB(AmigaMouse_ExpungeAttrs, 0)
