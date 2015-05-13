/*
    Copyright � 2015, The AROS Development Team. All rights reserved.
    $Id$

*/

#include <exec/alerts.h>
#include <exec/execbase.h>
#include <exec/lists.h>
#include <proto/exec.h>
#include <proto/kernel.h>

//#include <kernel_base.h>
#include <kernel_debug.h>
#include <kernel_scheduler.h>

#include "kernel_cpu.h"

#include <exec_platform.h>

#include <aros/types/spinlock_s.h>

#include <etask.h>

#include "exec_intern.h"

#define DSCHED(x)

/* Check if the currently running task on this cpu should be rescheduled.. */
BOOL core_Schedule(void)
{
    struct Task *task = GET_THIS_TASK;
    BOOL corereschedule = TRUE;

    DSCHED(bug("[KRN:BCM2708] core_Schedule()\n"));

    SysBase->AttnResched &= ~ARF_AttnSwitch;

    /* If task has pending exception, reschedule it so that the dispatcher may handle the exception */
    if (!(task->tc_Flags & TF_EXCEPT))
    {
#if defined(__AROSEXEC_SMP__)
        KrnSpinLock(&PrivExecBase(SysBase)->TaskReadySpinLock, NULL,
                    SPINLOCK_MODE_READ);
#endif
        /* Is the TaskReady empty? If yes, then the running task is the only one. Let it work */
        if (IsListEmpty(&SysBase->TaskReady))
            corereschedule = FALSE;
        else
        {
            struct Task *nexttask;
#if defined(__AROSEXEC_SMP__)
            int cpunum = GetCPUNumber();
            uint32_t cpumask = (1 << cpunum);
#endif
            /*
                    If there are tasks ready for this cpu that have equal or lower priority,
                    and the current task has used its alloted time - reschedule so they can run
                */
            for (nexttask = (struct Task *)GetHead(&SysBase->TaskReady); nexttask != NULL; nexttask = (struct Task *)GetSucc(nexttask))
            {
#if defined(__AROSEXEC_SMP__)
                if ((GetIntETask(nexttask)->iet_CpuAffinity  & cpumask) == cpumask)
                {
#endif
                    if (nexttask->tc_Node.ln_Pri <= task->tc_Node.ln_Pri)
                    {
                        /* If the running task did not used it's whole quantum yet, let it work */
                        if (!(SysBase->SysFlags & SFF_QuantumOver))
                            corereschedule = FALSE;
                    }
                    break;
#if defined(__AROSEXEC_SMP__)
                }
#endif
            }
        }
#if defined(__AROSEXEC_SMP__)
        KrnSpinUnLock(&PrivExecBase(SysBase)->TaskReadySpinLock);
#endif
    }

    DSCHED
        (
            if (corereschedule)
                bug("[KRN:BCM2708] '%s' @ 0x%p needs rescheduled ..\n", task->tc_Node.ln_Name, task);
        )

    return corereschedule;
}

/* Switch the currently running task on this cpu to ready state */
void core_Switch(void)
{
    struct Task *task = GET_THIS_TASK;

    DSCHED(bug("[KRN:BCM2708] core_Switch(): Switching away from '%s' @ 0x%p\n", task->tc_Node.ln_Name, task));

    if (task->tc_State == TS_RUN)
    {
#if defined(__AROSEXEC_SMP__)
        KrnSpinLock(&PrivExecBase(SysBase)->TaskRunningSpinLock, NULL,
                    SPINLOCK_MODE_WRITE);
#endif
        Remove(&task->tc_Node);
#if defined(__AROSEXEC_SMP__)
        KrnSpinUnLock(&PrivExecBase(SysBase)->TaskRunningSpinLock);
#endif
        task->tc_State = TS_READY;
    }

    /* if the current task has gone out of stack bounds, suspend it to prevent further damage to the system */
    if (task->tc_SPReg <= task->tc_SPLower || task->tc_SPReg > task->tc_SPUpper)
    {
        bug("[KRN:BCM2708] '%s' @ 0x%p went out of stack limits\n", task->tc_Node.ln_Name, task);
        bug("[KRN:BCM2708]   Lower 0x%p, upper 0x%p, SP 0x%p\n", task->tc_SPLower, task->tc_SPUpper, task->tc_SPReg);

        task->tc_SigWait    = 0;
        task->tc_State      = TS_WAIT;
#if defined(__AROSEXEC_SMP__)
        KrnSpinLock(&PrivExecBase(SysBase)->TaskWaitSpinLock, NULL,
                    SPINLOCK_MODE_WRITE);
#endif
        Enqueue(&SysBase->TaskWait, &task->tc_Node);
#if defined(__AROSEXEC_SMP__)
        KrnSpinUnLock(&PrivExecBase(SysBase)->TaskWaitSpinLock);
#endif

        Alert(AN_StackProbe);
    }

    task->tc_IDNestCnt = SysBase->IDNestCnt;

    if (task->tc_Flags & TF_SWITCH)
        AROS_UFC1NR(void, task->tc_Switch, AROS_UFCA(struct ExecBase *, SysBase, A6));
}

/* Dispatch a "new" ready task on this cpu */
struct Task *core_Dispatch(void)
{
    struct Task *newtask;
    struct Task *task = GET_THIS_TASK;
#if defined(__AROSEXEC_SMP__)
    int cpunum = GetCPUNumber();
    uint32_t cpumask = (1 << cpunum);
#endif

    DSCHED(bug("[KRN:BCM2708] core_Dispatch()\n"));

#if defined(__AROSEXEC_SMP__)
    KrnSpinLock(&PrivExecBase(SysBase)->TaskReadySpinLock, NULL,
                SPINLOCK_MODE_WRITE);
#endif
    for (newtask = (struct Task *)GetHead(&SysBase->TaskReady); newtask != NULL; newtask = (struct Task *)GetSucc(newtask))
    {
#if defined(__AROSEXEC_SMP__)
        if ((GetIntETask(newtask)->iet_CpuAffinity  & cpumask) == cpumask)
        {
#endif
            Remove(&newtask->tc_Node);
            break;
#if defined(__AROSEXEC_SMP__)
        }
#endif
    }

    if (task)
    {
        if (newtask == NULL)
            newtask = task;
        else if ((task->tc_State == TS_READY) && (task != newtask))
            Enqueue(&SysBase->TaskReady, &task->tc_Node);
    }
#if defined(__AROSEXEC_SMP__)
    KrnSpinUnLock(&PrivExecBase(SysBase)->TaskReadySpinLock);
#endif

    if ((newtask) &&
        (newtask->tc_State == TS_READY) ||
        (newtask->tc_State == TS_RUN))
    {
        SysBase->DispCount++;
        SysBase->IDNestCnt = newtask->tc_IDNestCnt;
        SET_THIS_TASK(newtask);
        SysBase->Elapsed   = SysBase->Quantum;
        SysBase->SysFlags &= ~SFF_QuantumOver;

        /* Check the stack of the task we are about to launch. */
        if ((newtask->tc_SPReg <= newtask->tc_SPLower) ||
             (newtask->tc_SPReg > newtask->tc_SPUpper))
            newtask->tc_State     = TS_WAIT;
        else
            newtask->tc_State     = TS_RUN;
    }

    if (newtask)
    {
#if defined(__AROSEXEC_SMP__)
        if (newtask->tc_State == TS_SPIN)
        {
            /* move it to the spinning list */
            KrnSpinLock(&PrivExecBase(SysBase)->TaskSpinningLock, NULL,
                SPINLOCK_MODE_WRITE);
            AddHead(&PrivExecBase(SysBase)->TaskSpinning, &newtask->tc_Node);
            KrnSpinUnLock(&PrivExecBase(SysBase)->TaskReadySpinLock);
        }
#endif

        /* if the task shouldnt run - force a reschedule.. */
        if (newtask->tc_State != TS_RUN)
        {
            core_Switch();
            newtask = core_Dispatch();
        }
        else
        {
            DSCHED(bug("[KRN:BCM2708] Launching '%s' @ %p\n", newtask->tc_Node.ln_Name, newtask));
        }
    }
    else
    {
        /* Go idle if there is nothing to do ... */
        DSCHED(bug("[KRN:BCM2708] No ready Task(s) - entering sleep mode\n"));

        /*
         * Idle counter is incremented every time when we enter here,
         * not only once. This is correct.
         */
        SysBase->IdleCount++;
        SysBase->AttnResched |= ARF_AttnSwitch;
    } 

    return newtask;
}
