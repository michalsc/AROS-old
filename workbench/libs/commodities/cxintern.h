/*
    (C) 1997-99 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: English
*/


#ifndef COMMODITIES_BASE_H
#define COMMODITIES_BASE_H

#define COF_ACTIVE (1<<1)	/* Object is active */
#define COF_VALID  (1<<0)	/* Object is inserted in a commodity list */

#define cxm_MaxLevel 32		/* Maximum routing level */

enum { CX_OBJECT, CX_MESSAGE, CX_INPUTEVENT };
enum { CXM_SINGLE, CXM_DOUBLE };

#define  AROS_ALMOST_COMPATIBLE
#include <exec/lists.h>

#include <exec/types.h>
#include <exec/io.h>
#include <exec/nodes.h>

#include <exec/ports.h>
#include <exec/libraries.h>
#include <exec/interrupts.h>
#include <exec/semaphores.h>
#include <devices/inputevent.h>
#include <devices/timer.h>
#include <libraries/commodities.h>
#include <dos/dos.h>


typedef struct cx_Object
{
    struct   Node     co_Node;
    UBYTE             co_Flags;
    UBYTE             co_Error;
    struct   MinList  co_ObjList;
    
    union
    {
	ULONG                co_DebugID;
	ULONG                co_TypeFilter;
	struct InputEvent   *co_IE;		/* Translate */
	IX		    *co_FilterIX;
	struct BrokerExt    *co_BExt;
	struct SendExt      *co_SendExt;
	struct SignalExt    *co_SignalExt;
	struct CustomExt    *co_CustomExt;
    } co_Ext;
} CxObj;

struct BrokerExt
{
    char            bext_Name[CBD_NAMELEN];
    char            bext_Title[CBD_TITLELEN];
    char            bext_Descr[CBD_DESCRLEN];
    struct Task    *bext_Task;
    struct MsgPort *bext_MsgPort;
};

struct SendExt
{
    struct MsgPort *sext_MsgPort;
    ULONG           sext_ID;
};

struct SignalExt
{
    struct Task *sixt_Task;
    UBYTE        sixt_SigBit;
};

struct CustomExt
{
    VOID  (*cext_Action)();
    ULONG   cext_ID;
};


typedef struct cx_Message
{
    struct Message     cxm_Message;
    CxObj             *cxm_Routing;           /* Next destination */
    LONG	       cxm_ID;
    UBYTE	       cxm_Type;
    UBYTE	       cxm_Level;
    CxObj             *cxm_retObj[cxm_MaxLevel];
    struct InputEvent *cxm_Data;
} CxMsg;


struct CommoditiesBase
{
    struct Library          cx_LibNode;
    
    BPTR                    cx_SegList;
    
    struct Library         *cx_KeyMapBase;
    struct Library         *cx_UtilityBase;
    struct ExecBase        *cx_SysBase;
    struct Library         *cx_TimerBase;
    
    struct IOStdReq         cx_IORequest;     /* To set up input handler */
    struct Interrupt        cx_Interrupt;     /* Input handler */
    struct MsgPort          cx_InputMP;       /* Reply port for input.device */
    struct List             cx_BrokerList;
    struct List             cx_MessageList;
    struct SignalSemaphore  cx_SignalSemaphore;
    struct InputEvent      *cx_IEvents;
    struct InputEvent     **cx_EventExtra;    /* Only for bookkeeping
						 purposes */
    struct MsgPort          cx_MsgPort;
    BOOL                    cx_Running;       /* Is the input handler
						   installed? */
    struct timerequest      cx_TimerIO;	      /* For timer.device... */
    struct MsgPort          cx_TimerMP;	      /* ... communication */
};


/* Extra prototypes */

BOOL  SetupIHandler(struct CommoditiesBase *CxBase);
VOID  FreeCxStructure(APTR obj, int type, struct Library *CxBase);
APTR  AllocCxStructure(LONG type, LONG objtype, struct Library *CxBase);
ULONG CheckStatus(CxObj *broker, ULONG command, struct Library *CxBase);


/* Locate library bases */

#ifdef SysBase
#undef SysBase
#endif
#ifdef KeyMapBase
#undef KeyMapBase
#endif
#ifdef UtilityBase
#undef UtilityBase
#endif
#ifdef TimerBase
#undef TimerBase
#endif

#define SysBase ((struct CommoditiesBase *)CxBase)->cx_SysBase
#define KeymapBase ((struct CommoditiesBase *)CxBase)->cx_KeyMapBase
#define UtilityBase ((struct CommoditiesBase *)CxBase)->cx_UtilityBase
#define TimerBase ((struct CommoditiesBase *)CxBase)->cx_TimerBase

#define ROUTECxMsg(msg, obj)    msg->cxm_Routing = obj

#define GPB(x) ((struct CommoditiesBase *)x)

#define expunge() \
AROS_LC0(BPTR, expunge, struct CommoditiesBase *, CxBase, 3, Commodities)

#endif /* COMMODITIES_BASE_H */
