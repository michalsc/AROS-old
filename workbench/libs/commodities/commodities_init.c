/*
    (C) 1997-99 AROS - The Amiga Research OS
    $Id$

    Desc: Commodities initialization code.
    Lang: English.
*/

#define  AROS_ALMOST_COMPATIBLE

#ifndef  DEBUG
#define  DEBUG 1
#endif

#include <aros/debug.h>

#include <utility/utility.h>
#include "cxintern.h"	/* Must be included after utility.h */ 

#include <exec/types.h>
#include <exec/resident.h>
#include <proto/exec.h>
#include <devices/timer.h>
#include <aros/libcall.h>

#include "initstruct.h"
#include <stddef.h>

#include <exec/libraries.h>
#include <exec/alerts.h>
#include <libraries/commodities.h>
#include <proto/commodities.h>
#include "libdefs.h"

#define INIT	AROS_SLIB_ENTRY(init, Commodities)

struct inittable;
extern const char name[];
extern const char version[];
extern const APTR inittabl[4];
extern void *const LIBFUNCTABLE[];
extern const struct inittable datatable;
extern struct CommoditiesBase *INIT();
extern struct CommoditiesBase *AROS_SLIB_ENTRY(open, Commodities)();
extern BPTR AROS_SLIB_ENTRY(close, Commodities)();
extern BPTR AROS_SLIB_ENTRY(expunge, Commodities)();
extern int AROS_SLIB_ENTRY(null, Commodities)();
extern const char LIBEND;

int entry(void)
{
    /* If the library was executed by accident return error code. */
    return -1;
}

const struct Resident resident=
{
    RTC_MATCHWORD,
    (struct Resident *)&resident,
    (APTR)&LIBEND,
    RTF_AUTOINIT,
    VERSION_NUMBER,
    NT_LIBRARY,
    0,
    (char *)name,
    (char *)&version[6],
    (ULONG *)inittabl
};

const char name[] = NAME_STRING;

const char version[] = VERSION_STRING;

const APTR inittabl[4]=
{
    (APTR)sizeof(struct CommoditiesBase),
    (APTR)LIBFUNCTABLE,
    (APTR)&datatable,
    &INIT
};

struct inittable
{
    S_CPYO(1,1,B);
    S_CPYO(2,1,L);
    S_CPYO(3,1,B);
    S_CPYO(4,1,W);
    S_CPYO(5,1,W);
    S_CPYO(6,1,L);
    S_END (LIBEND);
};

#define O(n) offsetof(struct CommoditiesBase, n)

const struct inittable datatable=
{
    { { I_CPYO(1,B,O(cx_LibNode.lib_Node.ln_Type)), { NT_LIBRARY } } },
    { { I_CPYO(1,L,O(cx_LibNode.lib_Node.ln_Name)), { (IPTR)name } } },
    { { I_CPYO(1,B,O(cx_LibNode.lib_Flags       )), { LIBF_SUMUSED|LIBF_CHANGED } } },
    { { I_CPYO(1,W,O(cx_LibNode.lib_Version     )), { 1 } } },
    { { I_CPYO(1,W,O(cx_LibNode.lib_Revision    )), { 0 } } },
    { { I_CPYO(1,L,O(cx_LibNode.lib_IdString    )), { (IPTR)&version[6] } } },
	I_END ()
};

#undef O

BOOL InitCx(struct CommoditiesBase *CxBase);
VOID ShutDownCx(struct CommoditiesBase *CxBase);

AROS_LH2(struct CommoditiesBase *, init,
 AROS_LHA(struct CommoditiesBase *, CxBase, D0),
 AROS_LHA(BPTR,               segList,   A0),
	   struct ExecBase *, sysBase, 0, Commodities)
{
    AROS_LIBFUNC_INIT
    /* This function is single-threaded by exec by calling Forbid. */

    /* Store arguments */
    CxBase->cx_SysBase = sysBase;
    CxBase->cx_SegList = segList;

    return CxBase;

    AROS_LIBFUNC_EXIT
}


AROS_LH1(struct CommoditiesBase *, open,
 AROS_LHA(ULONG, version, D0),
	   struct CommoditiesBase *, CxBase, 1, Commodities)
{
    AROS_LIBFUNC_INIT

    BOOL ok = TRUE;
    
    /*
      This function is single-threaded by exec by calling Forbid.
      If you break the Forbid() another task may enter this function
      at the same time. Take care.
    */
    
    /* Keep compiler happy */
    version = 0;
    
    D(bug("commodities_open: Entering...\n"));
    
    /* I have one more opener. */
    CxBase->cx_LibNode.lib_OpenCnt++;
    CxBase->cx_LibNode.lib_Flags &= ~LIBF_DELEXP;
    
    if(CxBase->cx_UtilityBase == NULL)
	CxBase->cx_UtilityBase = OpenLibrary(UTILITYNAME, 37);
    if(CxBase->cx_UtilityBase == NULL)
	return NULL;
    
    if(CxBase->cx_KeyMapBase == NULL)
	CxBase->cx_KeyMapBase = OpenLibrary("keymap.library", 37);
    if(CxBase->cx_KeyMapBase == NULL)
	return NULL;
    
    
    if(CxBase->cx_TimerBase == NULL)
    {
	CxBase->cx_TimerMP.mp_Node.ln_Type = NT_MSGPORT;
	CxBase->cx_TimerMP.mp_Flags = PA_IGNORE;
	NEWLIST(&CxBase->cx_TimerMP.mp_MsgList);

	CxBase->cx_TimerIO.tr_node.io_Message.mn_ReplyPort = &CxBase->cx_TimerMP;

	if(OpenDevice(TIMERNAME, UNIT_VBLANK,
		      (struct IORequest *)&CxBase->cx_TimerIO, 0) == 0)
	    CxBase->cx_TimerBase = 
		(struct Library *)(CxBase->cx_TimerIO.tr_node.io_Device);
    }
    if(CxBase->cx_TimerBase == NULL)
	return NULL;

    D(bug("commodities_open: Setting up Zero object.\n"));
    
    if(CxBase->cx_LibNode.lib_OpenCnt == 1)
	ok = InitCx((struct CommoditiesBase *)CxBase);
    
    if(!ok)
    {
        D(bug("Error: Failed to initialize commodities.library.\n"));
	ShutDownCx((struct CommoditiesBase *)CxBase);
	return NULL;
    }
    
    D(bug("commodities_open: Library correctly opened.\n"));
    
    return CxBase;

    AROS_LIBFUNC_EXIT
}


BOOL InitCx(struct CommoditiesBase *CxBase)
{
    CxObj *zero;

    InitSemaphore(&CxBase->cx_SignalSemaphore);
    NEWLIST(&CxBase->cx_BrokerList);
    NEWLIST(&CxBase->cx_MessageList);

    CxBase->cx_MsgPort.mp_Node.ln_Type = NT_MSGPORT;
    CxBase->cx_MsgPort.mp_Flags = PA_IGNORE;
    NEWLIST(&CxBase->cx_MsgPort.mp_MsgList);
    
    zero = CreateCxObj(CX_ZERO, (IPTR)NULL, (IPTR)NULL);
    
    if(zero == NULL)
	return FALSE;

    /* Make sure this object goes LAST in the list */
    ((struct Node *)zero)->ln_Pri = -128;
    AddHead(&CxBase->cx_BrokerList, (struct Node *)zero);
    return TRUE;
}


VOID ShutDownCx(struct CommoditiesBase *CxBase)
{
    struct InputEvent *temp;
    CxMsg *msg;
 
    /* Free messages */
    while((msg = (CxMsg *)GetMsg(&CxBase->cx_MsgPort)) != NULL)
	FreeCxStructure(msg, CX_MESSAGE, (struct Library *)CxBase);
    
    /* Free input events */
    while(CxBase->cx_IEvents != NULL)
    {
	temp = CxBase->cx_IEvents->ie_NextEvent;
	FreeCxStructure(CxBase->cx_IEvents, CX_INPUTEVENT,
			(struct Library *)CxBase);
	CxBase->cx_IEvents = temp;
    }

    CxBase->cx_IEvents = NULL;
    
    /* Remove the ZERO object, in case it exists. */
    DeleteCxObj((CxObj *)RemHead(&CxBase->cx_BrokerList));
}


AROS_LH0(BPTR, close, struct CommoditiesBase *, CxBase, 2, Commodities)
{
    AROS_LIBFUNC_INIT
    /*
	This function is single-threaded by exec by calling Forbid.
	If you break the Forbid() another task may enter this function
	at the same time. Take care.
    */

    /* I have one fewer opener. */
    if(--(CxBase->cx_LibNode.lib_OpenCnt) == 0)
	ShutDownCx(CxBase);
    
    if((CxBase->cx_LibNode.lib_Flags & LIBF_DELEXP) != 0)
    {
	if(CxBase->cx_LibNode.lib_OpenCnt == 0)
	    return expunge();
	
	CxBase->cx_LibNode.lib_Flags &= ~LIBF_DELEXP;
	return NULL;
    }

    return NULL;

    AROS_LIBFUNC_EXIT
}


AROS_LH0(BPTR, expunge, struct CommoditiesBase *, CxBase, 3, Commodities)
{
    AROS_LIBFUNC_INIT

    BPTR ret;
    /*
	This function is single-threaded by exec by calling Forbid.
	Never break the Forbid() or strange things might happen.
    */

    /* Test for openers. */
    if(CxBase->cx_LibNode.lib_OpenCnt != 0)
    {
	/* Set the delayed expunge flag and return. */
	CxBase->cx_LibNode.lib_Flags |= LIBF_DELEXP;
	return 0;
    }

    /* Get rid of the library. Remove it from the list. */
    Remove(&CxBase->cx_LibNode.lib_Node);

    /* Get returncode here - FreeMem() will destroy the field. */
    ret = CxBase->cx_SegList;

    CloseLibrary(CxBase->cx_KeyMapBase);
    CloseLibrary(CxBase->cx_UtilityBase);

    /* Free the memory. */
    FreeMem((char *)CxBase-CxBase->cx_LibNode.lib_NegSize,
	    CxBase->cx_LibNode.lib_NegSize + CxBase->cx_LibNode.lib_PosSize);

    return ret;
    AROS_LIBFUNC_EXIT
}


AROS_LH0I(int, null, struct CommoditiesBase *, CxBase, 4, Commodities)
{
    AROS_LIBFUNC_INIT
    return 0;
    AROS_LIBFUNC_EXIT
}

