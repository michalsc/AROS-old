/*
    Copyright � 2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc: ColdReboot() - Reboot the computer.
    Lang: english
*/

#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/tasks.h>
#include <aros/libcall.h>

#include <proto/exec.h>

#include "exec_intern.h"
#include "exec_util.h"

/*****************************************************************************

    NAME */

	AROS_LH0(void, ColdReboot,

/*  LOCATION */
	struct ExecBase *, SysBase, 121, Exec)

/*  FUNCTION
	This function will reboot the computer.

    INPUTS
	None.

    RESULT
	This function does not return.

    NOTES
	It can be quite harmful to call this function. It may be possible that
	you will lose data from other tasks not having saved, or disk buffers
	not being flushed. Plus you could annoy the (other) users.

    EXAMPLE

    BUGS

    SEE ALSO

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    Exec_DoResetCallbacks((struct IntExecBase *)SysBase, SD_ACTION_WARMREBOOT);

    asm volatile("mov pc,#0;");

    AROS_LIBFUNC_EXIT
}
