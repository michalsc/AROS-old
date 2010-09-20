#include <exec/alerts.h>
#include <exec/execbase.h>
#include <proto/exec.h>

#include <kernel_base.h>
#include <kernel_debug.h>
#include <kernel_scheduler.h>

#define D(x)

/*
 * Schedule the currently running task away. Put it into the TaskReady list 
 * in some smart way. This function is subject of change and it will be probably replaced
 * by some plugin system in the future
 */
BOOL core_Schedule(void)
{
    struct Task *task = SysBase->ThisTask;

    D(bug("[KRN] core_Schedule()\n"));

    SysBase->AttnResched &= ~ARF_AttnSwitch;

    /* If task has pending exception, reschedule it so that the dispatcher may handle the exception */
    if (!(task->tc_Flags & TF_EXCEPT))
    {
        BYTE pri;

        /* Is the TaskReady empty? If yes, then the running task is the only one. Let it work */
        if (IsListEmpty(&SysBase->TaskReady))
            return FALSE;

	/* Does the TaskReady list contains tasks with priority equal or lower than current task?
         * If so, then check further... */
	pri = ((struct Task*)GetHead(&SysBase->TaskReady))->tc_Node.ln_Pri;
        if (pri <= task->tc_Node.ln_Pri)
        {
            /* If the running task did not used it's whole quantum yet, let it work */
            if (!(SysBase->SysFlags & SFF_QuantumOver))
                return FALSE;
        }
    }

    /* 
     * If we got here, then the rescheduling is necessary. 
     * Put the task into the TaskReady list.
     */
    D(bug("[KRN] Setting task 0x%p (%s) to READY\n", task, task->tc_Node.ln_Name));
    task->tc_State = TS_READY;
    Enqueue(&SysBase->TaskReady, &task->tc_Node);

    /* Select new task to run */
    return TRUE;
}

/* Actually switch away from the task */
void core_Switch(void)
{
    struct Task *task = SysBase->ThisTask;

    D(bug("[KRN] core_Switch(): Old task = %p (%s)\n", task, task->tc_Node.ln_Name));

    task->tc_IDNestCnt = SysBase->IDNestCnt;

    if (task->tc_Flags & TF_SWITCH)
	AROS_UFC1(void, task->tc_Switch, AROS_UFCA(struct ExecBase *, SysBase, A6));
}

/*
 * Task dispatcher. Basically it may be the same one no matter
 * what scheduling algorithm is used (except SysBase->Elapsed reloading)
 */
struct Task *core_Dispatch(void)
{
    struct Task *task;

    D(bug("[KRN] core_Dispatch()\n"));

    task = (struct Task *)REMHEAD(&SysBase->TaskReady);
    if (!task) {
	D(bug("[KRN] No ready tasks, entering sleep mode\n"));
        /*
	 * Is the list of ready tasks empty? Well, go idle.
	 */
	return NULL;
    }

    SysBase->DispCount++;

    SysBase->IDNestCnt = task->tc_IDNestCnt;
    SysBase->ThisTask = task;
    SysBase->Elapsed = SysBase->Quantum;
    SysBase->SysFlags &= ~SFF_QuantumOver;
    task->tc_State = TS_RUN;

    D(bug("[KRN] New task = %p (%s)\n", task, task->tc_Node.ln_Name));

    /* Check the stack of the task we are about to launch */
    if (task->tc_SPReg <= task->tc_SPLower || task->tc_SPReg >= task->tc_SPUpper)
	Alert(AT_DeadEnd|AN_StackProbe);

    if (task->tc_Flags & TF_LAUNCH)
	AROS_UFC1(void, task->tc_Launch, AROS_UFCA(struct ExecBase *, SysBase, A6));

    /* Leave interrupt and jump to the new task */
    return task;
}

void core_Cause(unsigned char n)
{
    struct IntVector *iv = &SysBase->IntVects[n];

    /* If the SoftInt vector in SysBase is set, call it. It will do the rest for us */
    if (iv->iv_Code)
        AROS_UFC5(void, iv->iv_Code,
		  AROS_UFCA(ULONG, 1L << n, D1),
		  AROS_UFCA(APTR, NULL, A0),
		  AROS_UFCA(APTR, iv->iv_Data, A1),
		  AROS_UFCA(APTR, iv->iv_Code, A5),
		  AROS_UFCA(struct ExecBase *, SysBase, A6));
}
