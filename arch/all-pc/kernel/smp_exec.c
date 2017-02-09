/*
    Copyright � 2017, The AROS Development Team. All rights reserved.
    $Id$
*/

#define DEBUG 0

#include <aros/config.h>

#if defined(__AROSEXEC_SMP__)
#include <proto/exec.h>
#define __KERNEL_NOLIBBASE__
#include <proto/kernel.h>

#include <exec/rawfmt.h>

#include "kernel_base.h"
#include "kernel_intern.h"
#include "kernel_debug.h"
#include "apic.h"

#define __AROS_KERNEL__

#include "exec_intern.h"

#include "etask.h"

extern BOOL Exec_InitETask(struct Task *, struct Task *, struct ExecBase *);

struct Task *cpu_InitBootStrap(apicid_t cpuNo)
{
    struct ExceptionContext *bsctx;
    struct MemList *ml;
#define bstask          ((struct Task *)(ml->ml_ME[0].me_Addr))
#define bstaskmlsize    (sizeof(struct MemList))

    /* Build bootstraps memory list */
    if ((ml = AllocMem(bstaskmlsize, MEMF_PUBLIC|MEMF_CLEAR)) == NULL)
    {
        bug("[Kernel:%03u] FATAL : Failed to allocate memory for bootstrap task", cpuNo);
        return NULL;
    }

    ml->ml_NumEntries      = 1;

    ml->ml_ME[0].me_Length = sizeof(struct Task);
    if ((ml->ml_ME[0].me_Addr = AllocMem(sizeof(struct Task),    MEMF_PUBLIC|MEMF_CLEAR)) == NULL)
    {
        bug("[Kernel:%03u] FATAL : Failed to allocate task for bootstrap", cpuNo);
        FreeMem(ml, bstaskmlsize);
        return NULL;
    }

    AddHead(&bstask->tc_MemEntry, &ml->ml_Node);

    D(bug("[Kernel:%03u] %s: Bootstrap task @ 0x%p\n", cpuNo, __func__, bstask));

    if ((bsctx = KrnCreateContext()) == NULL)
    {
        bug("[Kernel:%03u] FATAL : Failed to create the bootstrap Task context\n", cpuNo);
        FreeMem(ml->ml_ME[0].me_Addr, ml->ml_ME[0].me_Length);
        FreeMem(ml, bstaskmlsize);
        return NULL;
    }

    D(bug("[Kernel:%03u] %s: CPU Ctx @ 0x%p\n", cpuNo, __func__, bsctx));

    NEWLIST(&bstask->tc_MemEntry);

    if ((bstask->tc_Node.ln_Name = AllocVec(20, MEMF_CLEAR)) != NULL)
    {
        IPTR bstNameArg[] = 
        {
            cpuNo
        };
        RawDoFmt("CPU #%03u Bootstrap", (RAWARG)bstNameArg, RAWFMTFUNC_STRING, bstask->tc_Node.ln_Name);
    }
    bstask->tc_Node.ln_Type = NT_TASK;
    bstask->tc_Node.ln_Pri  = 0;
    bstask->tc_State        = TS_READY;
    bstask->tc_SigAlloc     = 0xFFFF;

    /* Create a ETask structure and attach CPU context */
    if (!Exec_InitETask(bstask, NULL, SysBase))
    {
        bug("[Kernel:%03u] FATAL : Failed to initialize bootstrap ETask\n", cpuNo);
        FreeVec(bstask->tc_Node.ln_Name);
        FreeMem(ml->ml_ME[0].me_Addr, ml->ml_ME[0].me_Length);
        FreeMem(ml, bstaskmlsize);
        return NULL;
    }
    bstask->tc_UnionETask.tc_ETask->et_RegFrame = bsctx;

    /* the bootstrap can only run on this CPU */
    IntETask(bstask->tc_UnionETask.tc_ETask)->iet_CpuNumber = cpuNo;
    IntETask(bstask->tc_UnionETask.tc_ETask)->iet_CpuAffinity = KrnGetCPUMask(cpuNo);

    bsctx->Flags = 0;

    return bstask;
#undef  bstask
}

void cpu_BootStrap(struct Task *bstask)
{
    D(
        apicid_t cpuNo = KrnGetCPUNumber();
    
        bug("[Kernel:%03u] %s()\n", cpuNo, __func__);
        
        if (IntETask(bstask->tc_UnionETask.tc_ETask)->iet_CpuNumber != cpuNo)
            bug("[Kernel:%03u] %s: bstask running on wrong CPU? (task cpu = %03u)\n", cpuNo, __func__, IntETask(bstask->tc_UnionETask.tc_ETask)->iet_CpuNumber);
    )

    bstask->tc_State = TS_RUN;
    SET_THIS_TASK(bstask);

    D(bug("[Kernel:%03u] %s: Leaving supervisor mode\n", cpuNo, __func__));

    krnLeaveSupervisorRing(FLAGS_INTENABLED);

    D(
        bug("[Kernel:%03u] %s: Initialising Scheduler...\n", cpuNo, __func__);
        bug("[Kernel:%03u] %s:        Enabling Exec Interrupts...\n", cpuNo, __func__);
    )

    /* We now start up the interrupts */
    Permit();
    Enable();

    D(bug("[Kernel:%03u] %s:        Creating Idle Task ...\n", cpuNo, __func__));

    Exec_X86CreateIdleTask(SysBase);

    D(bug("[Kernel:%03u] %s: Done\n", cpuNo, __func__));
}
#endif
