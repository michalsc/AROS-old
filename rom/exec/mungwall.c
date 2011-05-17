/*
    Copyright � 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: MungWall memory anti-trashing checker
    Lang: english
*/

#include <exec/alerts.h>
#include <exec/rawfmt.h>
#include <proto/exec.h>

#include "exec_intern.h"
#include "etask.h"
#include "memory.h"
#include "mungwall.h"

/*
 * Build a wall around the allocated chunk.
 * Returns updated pointer to the beginning of the chunk (actually a pointer to a usable area)
 *
 * 'res' is a pointer to the beginning of a raw memory block (inside which walls will be constructed)
 * 'origSize' is ORIGINAL allocation size (before adding mungwall size)
 */
APTR MungWall_Build(APTR res, APTR pool, IPTR origSize, ULONG requirements, struct ExecBase *SysBase)
{
    if ((PrivExecBase(SysBase)->IntFlags & EXECF_MungWall) && res)
    {
    	struct MungwallHeader *header = res;

	D(bug("[MungWall] Allocated %u bytes at 0x%p\n", origSize, res + MUNGWALL_BLOCK_SHIFT));

        /* Save orig byteSize before wall (there is one room of MUNGWALLHEADER_SIZE
	   bytes before wall for such stuff (see above).
	*/

	header->mwh_magicid   = MUNGWALL_HEADER_ID;
	header->mwh_fault     = FALSE;
	header->mwh_allocsize = origSize;
	header->mwh_pool      = pool;

	/* Skip to the start of the pre-wall */
        res += MUNGWALLHEADER_SIZE;

	/* Initialize pre-wall */
	BUILD_WALL(res, 0xDB, MUNGWALL_SIZE);

	/* move over the block between the walls */
	res += MUNGWALL_SIZE;

	/* Fill the block with weird stuff to exploit bugs in applications */
	if (!(requirements & MEMF_CLEAR))
	    MUNGE_BLOCK(res, MEMFILL_ALLOC, origSize);

	/* Initialize post-wall */
	BUILD_WALL(res + origSize, 0xDB, MUNGWALL_SIZE + AROS_ROUNDUP2(origSize, MEMCHUNK_TOTAL) - origSize);

	Forbid();
    	AddHead((struct List *)&PrivExecBase(SysBase)->AllocMemList, (struct Node *)&header->mwh_node);
	Permit();
    }
    return res;
}

char *FormatMWContext(char *buffer, struct MungwallContext *ctx, struct ExecBase *SysBase)
{
    buffer = NewRawDoFmt("Block at 0x%p, size %lu", (VOID_FUNC)RAWFMTFUNC_STRING, buffer, (APTR)ctx->hdr + MUNGWALL_BLOCK_SHIFT, ctx->hdr->mwh_allocsize) - 1;

    if (ctx->bad_id)
    	buffer = NewRawDoFmt("\nMUNGWALL_HEADER_ID mismatch\n", (VOID_FUNC)RAWFMTFUNC_STRING, buffer) - 1;

    if (ctx->freeSize)
    	buffer = NewRawDoFmt("\nFreeMem size %lu mismatch\n", (VOID_FUNC)RAWFMTFUNC_STRING, buffer, ctx->freeSize) - 1;

    if (ctx->pre_start)
    	buffer = NewRawDoFmt("\nPre-wall broken at 0x%p - 0x%p\n", (VOID_FUNC)RAWFMTFUNC_STRING, buffer, ctx->pre_start, ctx->pre_end) - 1;

    if (ctx->post_start)
    	buffer = NewRawDoFmt("\nPost-wall broken at 0x%p - 0x%p\n", (VOID_FUNC)RAWFMTFUNC_STRING, buffer, ctx->post_start, ctx->post_end) - 1;

    return buffer;
}

static APTR CheckWall(UBYTE *ptr, UBYTE fill, IPTR size, APTR *endptr)
{
    APTR start = NULL;
    APTR end   = NULL;

    while (size--)
    {
	if (*ptr != fill)
	{
	    if (!start)
	    	start = ptr;

	    end = ptr;
	}

	ptr++;
    }

    *endptr = end;
    return start;
}

static void CheckHeader(struct MungwallHeader *header, IPTR byteSize, APTR caller, APTR stack, struct ExecBase *SysBase)
{
    struct MungwallContext mwdata;

    /* Do not report the fault twice on the same header */
    if (header->mwh_fault)
    	return;

    mwdata.bad_id = (header->mwh_magicid != MUNGWALL_HEADER_ID);
    if (mwdata.bad_id)
        header->mwh_fault = TRUE;

    if (byteSize && (header->mwh_allocsize != byteSize))
    {
    	mwdata.freeSize = byteSize;
	header->mwh_fault = TRUE;
    }
    else
	mwdata.freeSize = 0;

    mwdata.pre_start = CheckWall((UBYTE *)header + MUNGWALLHEADER_SIZE, 0xDB, MUNGWALL_SIZE, &mwdata.pre_end);
    if (mwdata.pre_start)
    	header->mwh_fault = TRUE;

    mwdata.post_start = CheckWall((UBYTE *)header + MUNGWALL_BLOCK_SHIFT + header->mwh_allocsize, 0xDB,
	    	      		 MUNGWALL_SIZE + AROS_ROUNDUP2(header->mwh_allocsize, MEMCHUNK_TOTAL) - header->mwh_allocsize,
	    	      		 &mwdata.post_end);
    if (mwdata.post_start)
    	header->mwh_fault = TRUE;

    if (header->mwh_fault)
    {
    	/* Set mungwall alert context and throw an alert */
    	struct Task *me = FindTask(NULL);
    	struct IntETask *iet = GetIntETask(me);

	if (caller)
	{
    	    iet->iet_AlertFlags   |= AF_Location;
    	    iet->iet_AlertLocation = caller;
    	    iet->iet_AlertStack    = stack;
    	}

    	mwdata.hdr = header;

	iet->iet_AlertType = AT_MUNGWALL;
	CopyMem(&mwdata, &iet->iet_AlertData, sizeof(mwdata));

    	Alert(AN_MemoryInsane);
    	
    	/*
    	 * Our entry can be freed by another process while we are sitting in Alert().
    	 * This is 100% safe as long as we don't touch the entry after Alert().
    	 * Well, potentally dangerous case is list iteration in MungWall_Scan().
    	 * What to do then? Use a semaphore? Won't do because of RemTask(NULL),
    	 * during which SysBase->ThisTask becomes garbage, thus a semaphore can't
    	 * be used.
    	 */
    }
}

/*
 * Check integrity of walls around the specified block.
 *
 * 'memoryBlock' is an address of the block from user's point of view
 * (i. e. what was returned by allocation call)
 * 'byteSize' is length of the block (again, from user's point of view(
 *
 * Returns address of the raw block (what really needs to be deallocated)
 */
APTR MungWall_Check(APTR memoryBlock, IPTR byteSize, APTR caller, APTR stack, struct ExecBase *SysBase)
{
    if (PrivExecBase(SysBase)->IntFlags & EXECF_MungWall)
    {
	struct MungwallHeader *header;

	D(bug("[MungWall] Freeing %u bytes at 0x%p\n", byteSize, memoryBlock));

	/* Align size and block to the requirements (needed because of AllocAbs) */
	byteSize += (IPTR)memoryBlock & (MEMCHUNK_TOTAL - 1);
	memoryBlock = (APTR)AROS_ROUNDDOWN2((IPTR)memoryBlock, MEMCHUNK_TOTAL);

	/* Take address of mungwall header */
	header = memoryBlock - MUNGWALL_BLOCK_SHIFT;

    	/*
    	 * Remove from PrivExecBase->AllocMemList.
    	 * Do it before checking, otherwise AvailMem() can hit into it and cause a deadlock
    	 * while the alert is displayed.
    	 */
	Forbid();
    	Remove((struct Node *)header);
	Permit();

	/* Reset fault state in order to see who is freeing the bad entry */
	header->mwh_fault = FALSE;

	CheckHeader(header, byteSize, caller, stack, SysBase);

	/* Fill block with weird stuff to esploit bugs in applications
	 *
	 * DOH! There's some _BAD_ code around that assumes memory can still be
	 * accessed after freeing by just preventing task switching. In AROS,
	 * RemTask(NULL) suffers of this problem because DOS processes are
	 * created with their TCB placed in the tc_MemEntry list.
	 * The workaround is to avoid munging when current task is in TS_REMOVED
	 * state (RemTask() sets it). However RemTask() still needs reengineering
	 * before memory protection can be used. With MP deallocating memory can
	 * cause immediate blocking of access to it, so RemTask() needs to move
	 * the stack to some safe place and make sure that task structure is not
	 * accessed after freeing it.
	 */
	if (SysBase->ThisTask->tc_State != TS_REMOVED)
		MUNGE_BLOCK(memoryBlock, MEMFILL_FREE, byteSize);

	/* Return real start of the block to deallocate */
	memoryBlock = header;
    }

    return memoryBlock;
}

/*
 * Scan the whole allocations list, optionally removing entries
 * belonging to a particular pool.
 */
void MungWall_Scan(APTR pool, APTR caller, APTR stack, struct ExecBase *SysBase)
{
    D(bug("[Mungwall] Scan(), caller 0x%p\n", caller));

    if (PrivExecBase(SysBase)->IntFlags & EXECF_MungWall)
    {
	struct MungwallHeader 	*allocnode;
	struct MungwallHeader	*tmp;

	Forbid();

	ForeachNodeSafe(&PrivExecBase(SysBase)->AllocMemList, allocnode, tmp)
	{
	    if (pool && (allocnode->mwh_pool == pool))
	    {
		/*
		 * If pool address is given, remove entries belonging to it.
		 * It's DeletePool() and they are going to be freed.
		 * Additionally we reset fault state on them. This will cause
		 * one more alert and we can track where the memory was freed.
	         * This will give us a hint on who was owning it.
		 */
		Remove((struct Node *)allocnode);
	    	allocnode->mwh_fault = FALSE;
	    }

	    CheckHeader(allocnode, 0, caller, stack, SysBase);
	}

	Permit();
    }
}
