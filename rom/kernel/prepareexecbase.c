/*
    Copyright � 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    ExecBase initialization, no-MMU version.
*/

#include <aros/asmcall.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <exec/resident.h>
#include <proto/exec.h>

#include <kernel_base.h>
#include <kernel_debug.h>
#include <kernel_romtags.h>

struct ExecBase *krnPrepareExecBase(UWORD *ranges[], struct MemHeader *mh, struct TagItem *bootMsg)
{
    struct Resident *exec; 
    struct ExecBase *sysBase;
    struct Resident **resList = krnRomTagScanner(mh, ranges);
bug("1 ranges=%p, mh=%p\n", ranges, mh);
    if (!resList)
    {
        krnPanic(NULL, "Failed to create initial resident list\n"
        	       "Not enough memory space provided");
        return NULL;
    }
bug("2 resList=%p\n", resList);
    exec = krnFindResident(resList, "exec.library");
    if (!exec)
    {
	krnPanic(NULL, "Failed to create ExecBase\n"
		       "exec.library is not found");
    	return NULL;
    }
bug("3 exec=%p\n", exec);
bug("4 rt_Init=%p\n", exec->rt_Init);
    /* Magic. Described in rom/exec/exec_init.c. */
    sysBase = krnInitExecBase(exec, mh, bootMsg);
bug("4");
    if (!sysBase)
    {
	krnPanic(NULL, "Failed to create ExecBase\n"
		       "\n"
		       "MemHeader 0x%p, First chunk 0x%p, %u bytes free",
		       mh, mh->mh_First, mh->mh_Free);

	return NULL;
    }
bug("5");
    sysBase->ResModules = resList;
bug("6");
#ifndef NO_RUNTIME_DEBUG
    /* Print out modules list if requested by the user */
    if (SysBase->ex_DebugFlags & EXECDEBUGF_INITCODE)
    {
	ULONG i;

	nbug("Resident modules (addr: pri flags version name):\n");

	for (i = 0; resList[i]; i++)
	{
	    nbug("+ %p: %4d %02x %3d \"%s\"\n", resList[i], resList[i]->rt_Pri,
		 resList[i]->rt_Flags, resList[i]->rt_Version, resList[i]->rt_Name);
	}
    }
#endif
    return sysBase;
}
