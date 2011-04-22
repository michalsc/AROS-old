/*
    Copyright � 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Common IORequest processing routines
    Lang: english
*/

#include <aros/debug.h>
#include <aros/symbolsets.h>
#include <devices/newstyle.h>
#include <exec/errors.h>
#include <exec/initializers.h>
#include <hardware/intbits.h>
#include <proto/exec.h>
#include <proto/timer.h>

#include "timer_intern.h"
#include "timer_macros.h"

/****************************************************************************************/

#define NEWSTYLE_DEVICE 1

#define ioStd(x)  	((struct IOStdReq *)x)

/****************************************************************************************/

#if NEWSTYLE_DEVICE

static const UWORD SupportedCommands[] =
{
    TR_GETSYSTIME,
    TR_SETSYSTIME,
    TR_ADDREQUEST,
    NSCMD_DEVICEQUERY,
    0
};

#endif

static void addToWaitList(struct MinList *list, struct timerequest *iotr, struct ExecBase *SysBase)
{
    /* We are disabled, so we should take as little time as possible. */
    struct timerequest *tr;
    BOOL added = FALSE;

    ForeachNode(list, tr)
    {
    	/* If the time in the new request is less than the next request */
    	if (CMPTIME(&tr->tr_time, &iotr->tr_time) < 0)
    	{
    	    /* Add the node before the next request */
    	    Insert((struct List *)list, &iotr->tr_node.io_Message.mn_Node, tr->tr_node.io_Message.mn_Node.ln_Pred);

    	    added = TRUE;
    	    break;
    	}
    }

    /*
     * This will catch the case of either an empty list, or request is
     * for after all other requests
     */
    if(!added)
    	ADDTAIL(list, iotr);

#if PRINT_LIST
    bug("Current list contents:\n");

    ForeachNode(list, tr)
    {
	 bug("%u.%u\n", tr->tr_time.tv_secs, tr->tr_time.tv_micro);
    }
#endif
}

BOOL common_BeginIO(struct timerequest *timereq, struct TimerBase *TimerBase)
{
    ULONG unitNum = (IPTR)timereq->tr_node.io_Unit;
    BOOL replyit = FALSE;
    BOOL addedhead = FALSE;

    timereq->tr_node.io_Message.mn_Node.ln_Type = NT_MESSAGE;
    timereq->tr_node.io_Error = 0;

    switch(timereq->tr_node.io_Command)
    {
#if NEWSTYLE_DEVICE
    case NSCMD_DEVICEQUERY:

	/*
	 * CHECKME: In timer.device this is maybe a bit problematic, as the
	 * timerequest structure does not have io_Data and io_Length members
	 */
	if (timereq->tr_node.io_Message.mn_Length < sizeof(struct IOStdReq))
	{
	    timereq->tr_node.io_Error = IOERR_BADLENGTH;
	}
	else if(ioStd(timereq)->io_Length < ((LONG)OFFSET(NSDeviceQueryResult, SupportedCommands)) + sizeof(UWORD *))
	{
	    timereq->tr_node.io_Error = IOERR_BADLENGTH;
	}
	else
	{
	    struct NSDeviceQueryResult *d = (struct NSDeviceQueryResult *)ioStd(timereq)->io_Data;

	    d->DevQueryFormat 	 = 0;
	    d->SizeAvailable 	 = sizeof(struct NSDeviceQueryResult);
	    d->DeviceType 	 = NSDEVTYPE_TIMER;
	    d->DeviceSubType 	 = 0;
	    d->SupportedCommands = (UWORD *)SupportedCommands;

	    ioStd(timereq)->io_Actual = sizeof(struct NSDeviceQueryResult);
	}
	break;
#endif

    case TR_GETSYSTIME:
	GetSysTime(&timereq->tr_time);

	if (!(timereq->tr_node.io_Flags & IOF_QUICK))
	    ReplyMsg(&timereq->tr_node.io_Message);

	replyit = FALSE; /* Because replyit will clear the timeval */
	break;

    case TR_SETSYSTIME:
	Disable();

	/* Set current time value */
	TimerBase->tb_CurrentTime.tv_secs  = timereq->tr_time.tv_secs;
	TimerBase->tb_CurrentTime.tv_micro = timereq->tr_time.tv_micro;
	/* Update hardware */
	EClockSet(TimerBase);

        Enable();
	replyit = TRUE;
	break;

    case TR_ADDREQUEST:
	switch(unitNum)
	{
	case UNIT_WAITUNTIL:
	    Disable();

	    if (CMPTIME(&TimerBase->tb_CurrentTime, &timereq->tr_time) <= 0)
	    {
		timereq->tr_time.tv_secs = timereq->tr_time.tv_micro = 0;
		timereq->tr_node.io_Error = 0;
		replyit = TRUE;
	    }
	    else
	    {
		/* Ok, we add this to the list. UNIT_WAITUNTIL is VBlank-based. */
		addToWaitList(&TimerBase->tb_Lists[UNIT_VBLANK], timereq, SysBase);

		/*
		 * If our request was added to the head of the list, we may need to
		 * readjust our hardware interrupt (reset elapsed time).
		 * This routine returns TRUE in order to indicate this.
		 */
		if (TimerBase->tb_Lists[UNIT_VBLANK].mlh_Head == (struct MinNode *)timereq)
		    addedhead = TRUE;

		replyit = FALSE;
		timereq->tr_node.io_Flags &= ~IOF_QUICK;
	    }
	    
	    Enable();
	    break;

	case UNIT_VBLANK:
	case UNIT_MICROHZ:
	    Disable();

	    /*
	     * Adjust the time request to be relative to the
	     * the elapsed time counter that we keep.
	    */
	    ADDTIME(&timereq->tr_time, &TimerBase->tb_Elapsed);		    

	    /* Slot it into the list */
	    addToWaitList(&TimerBase->tb_Lists[unitNum], timereq, SysBase);

	    /* Indicate if HW need to be reprogrammed */
	    if (TimerBase->tb_Lists[unitNum].mlh_Head == (struct MinNode *)timereq)
		addedhead = TRUE;

	    Enable();
	    timereq->tr_node.io_Flags &= ~IOF_QUICK;
	    replyit = FALSE;
	    break;

	case UNIT_ECLOCK:
	case UNIT_WAITECLOCK:
	    /* TODO: implement these (backport from m68k-Amiga) */
	default:
	    replyit = FALSE;
	    timereq->tr_node.io_Error = IOERR_NOCMD;
	    break;

	} /* switch(unitNum) */
	break;

    case CMD_CLEAR:
    case CMD_FLUSH:
    case CMD_INVALID:
    case CMD_READ:
    case CMD_RESET:
    case CMD_START:
    case CMD_STOP:
    case CMD_UPDATE:
    case CMD_WRITE:
    default:
	replyit = TRUE;
	timereq->tr_node.io_Error = IOERR_NOCMD;
	break;
	    
    } /* switch(command) */

    if (replyit)
    {
	timereq->tr_time.tv_secs  = 0;
	timereq->tr_time.tv_micro = 0;

	if (!(timereq->tr_node.io_Flags & IOF_QUICK))
	    ReplyMsg(&timereq->tr_node.io_Message);
    }

    return addedhead;
}

/* Check a single unit and reply completed requests */
void checkUnit(struct TimerBase *TimerBase, struct MinList *unit, struct ExecBase *SysBase)
{
    struct timerequest *tr, *next;

    /*
     * Go through the list and return requests that have completed.
     * A completed request is one whose time is less than that of the elapsed time.
    */
    ForeachNodeSafe(unit, tr, next)
    {
	if (CMPTIME(&TimerBase->tb_Elapsed, &tr->tr_time) <= 0)
	{
	    /* This request has finished */
	    REMOVE(tr);

#ifdef USE_VBLANK_EMU
	    if (tr == &TimerBase->tb_vblank_timerequest)
	    {
	    	struct IntVector *iv = &SysBase->IntVects[INTB_VERTB];

	    	/* VBlank Emu */
		if (iv->iv_Code)
		{
		    AROS_UFC5(void, iv->iv_Code,
			AROS_UFCA(ULONG, 0, D1),
			AROS_UFCA(ULONG, 0, A0),
			AROS_UFCA(APTR, iv->iv_Data, A1),
			AROS_UFCA(APTR, iv->iv_Code, A5),
			AROS_UFCA(struct ExecBase *, SysBase, A6)
		    );
		}

		/*
		 * Process VBlank unit.
		 * The philosophy behind is that only software which needs to measure
		 * exact intervals uses MICROHZ unit. Others use VBLANK one. As a result,
		 * VBLANK queue is generally more populated than MICROHZ one.
		 * VBLANK queue is checked more rarely than MICROHZ, this helps to decrease
		 * CPU usage.
		 */
		checkUnit(TimerBase, &TimerBase->tb_Lists[UNIT_VBLANK], SysBase);

		/*
		 * Automatically requeue/reactivate request.
		 * Feature: get value every time from SysBase. This means
		 * that the user can change our VBlank rate in runtime by modifying
		 * this field.
		 */
    		tr->tr_time.tv_secs  = 0;
		tr->tr_time.tv_micro = 1000000 / SysBase->VBlankFrequency;
                ADDTIME(&tr->tr_time, &TimerBase->tb_Elapsed);
		addToWaitList(unit, tr, SysBase);

		continue;
	    }
#endif

	    tr->tr_time.tv_secs  = 0;
	    tr->tr_time.tv_micro = 0;
	    tr->tr_node.io_Error = 0;

	    ReplyMsg(&tr->tr_node.io_Message);
	}
	else
	{
	    /*
		The first request hasn't finished, as all requests are in
		order, we don't bother searching through the remaining
	    */
	    break;
	}
    }
}

/****************************************************************************************/

static int Timer_Open(struct TimerBase *LIBBASE, struct timerequest *tr, ULONG unitNum, ULONG flags)
{
    /*
     * Normally, we should check the length of the message and other
     * such things, however the RKM documents an example where the
     * length of the timerrequest isn't set, so we must not check
     * this.
     * This fixes bug SF# 741580
     */

    if (unitNum > UNIT_WAITECLOCK)
	tr->tr_node.io_Error = IOERR_OPENFAIL;
    else
    {
	tr->tr_node.io_Error  = 0;
	tr->tr_node.io_Unit   = (NULL + unitNum);
	tr->tr_node.io_Device = &LIBBASE->tb_Device;
    }

    return TRUE;
}

ADD2OPENDEV(Timer_Open, 0);
