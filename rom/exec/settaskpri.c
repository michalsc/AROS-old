/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$
    $Log$
    Revision 1.5  1996/10/24 15:50:58  aros
    Use the official AROS macros over the __AROS versions.

    Revision 1.4  1996/08/13 13:56:08  digulla
    Replaced AROS_LA by AROS_LHA
    Replaced some AROS_LH*I by AROS_LH*
    Sorted and added includes

    Revision 1.3  1996/08/01 17:41:20  digulla
    Added standard header for all files

    Desc:
    Lang: english
*/
#include <exec/execbase.h>
#include <aros/libcall.h>

/*****************************************************************************

    NAME */
	#include <clib/exec_protos.h>

	AROS_LH2(BYTE, SetTaskPri,

/*  SYNOPSIS */
	AROS_LHA(struct Task *, task,      A1),
	AROS_LHA(LONG,          priority,  D0),

/*  LOCATION */
	struct ExecBase *, SysBase, 50, Exec)

/*  FUNCTION
	Change the priority of a given task. As a general rule the higher
	the priority the more CPU time a task gets. Useful values are within
	-127 to 5.

    INPUTS
	task	 - Pointer to task structure.
	priority - New priority of the task.

    RESULT
	Old task priority.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    BYTE old;

    /* Always Disable() when doing something with task lists. */
    Disable();

    /* Get returncode */
    old=task->tc_Node.ln_Pri;

    /* Set new value. */
    task->tc_Node.ln_Pri=priority;

    /* Check if the task is willing to run. */
    if(task->tc_State!=TS_WAIT)
    {
	/* If it is in the ready list remove and reinsert it. */
	if(task->tc_State==TS_READY)
	{
	    Remove(&task->tc_Node);
	    Enqueue(&SysBase->TaskReady,&task->tc_Node);
	}

	/*
	    I could check the task priorities here to determine if
	    the following is really necessary, but OTOH priority
	    changes are rare and the hassle isn't really worth it.
	*/

	/* Are taskswitches allowed? */
	if(SysBase->TDNestCnt>=0||SysBase->IDNestCnt>0)
	    /* No. Store it for later. */
	    SysBase->AttnResched|=0x80;
	else
	{
	    /* Switches are allowed. Move the current task away. */
	    SysBase->ThisTask->tc_State=TS_READY;
	    Enqueue(&SysBase->TaskReady,&SysBase->ThisTask->tc_Node);

	    /* And force a rescedule. */
	    Switch();
	}
    }

    /* All done. */
    Enable();
    return old;
    AROS_LIBFUNC_EXIT
} /* SetTaskPri */
