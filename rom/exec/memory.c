#include <proto/kernel.h>

#include "exec_intern.h"
#include "memory.h"

/* Find MemHeader to which address belongs */
struct MemHeader *FindMem(APTR address, struct ExecBase *SysBase)
{
    struct MemHeader *mh;

    /* Nobody should change the memory list now. */
    Forbid();

    /* Follow the list of MemHeaders */
    mh = (struct MemHeader *)SysBase->MemList.lh_Head;

    while(mh->mh_Node.ln_Succ != NULL)
    {
	/* Check if this MemHeader fits */
	if(address >= mh->mh_Lower && address <= mh->mh_Upper)
	{
	    /* Yes. Return it. */
	    Permit();
	    return mh;
	}

	/* Go to next MemHeader */
	mh = (struct MemHeader *)mh->mh_Node.ln_Succ;
    }

    Permit();
    return NULL;
}

/*
 * Allocate block from the given MemHeader.
 * From the requirements it takes only MEMF_REVERSE flag in order to
 * know allocation direction.
 * This routine can be called with SysBase = NULL.
 */
APTR stdAlloc(struct MemHeader *mh, ULONG byteSize, ULONG requirements, struct ExecBase *SysBase)
{
    struct MemChunk *mc=NULL, *p1, *p2;
    
    /*
        The free memory list is only single linked, i.e. to remove
        elements from the list I need node's predessor. For the
        first element I can use mh->mh_First instead of a real predessor.
    */
    p1 = (struct MemChunk *)&mh->mh_First;
    p2 = p1->mc_Next;

    /* Is there anything in the list? */
    if (p2 != NULL)
    {
        /* Then follow it */
        for (;;)
        {
#if !defined(NO_CONSISTENCY_CHECKS)
            /* Consistency check: Check alignment restrictions */
            if (((IPTR)p2|(IPTR)p2->mc_Bytes) & (MEMCHUNK_TOTAL-1))
	    {
		if (SysBase)
		    Alert(AN_MemCorrupt|AT_DeadEnd);
		return NULL;
	    }
#endif
            
            /* Check if the current block is large enough */
            if(p2->mc_Bytes>=byteSize)
            {
                /* It is. */
                mc=p1;
                /* Use this one if MEMF_REVERSE is not set.*/
                if(!(requirements&MEMF_REVERSE))
                    break;
                /* Else continue - there may be more to come. */
            }

            /* Go to next block */
            p1=p2;
            p2=p1->mc_Next;

            /* Check if this was the end */
            if(p2==NULL)
                break;
#if !defined(NO_CONSISTENCY_CHECKS)
            /*
                Consistency check:
                If the end of the last block+1 is bigger or equal to
                the start of the current block something must be wrong.
            */
            if((UBYTE *)p2<=(UBYTE *)p1+p1->mc_Bytes)
	    {
		if (SysBase)
		    Alert(AN_MemCorrupt|AT_DeadEnd);
		return NULL;
	    }
#endif
        }
        
        /* Something found? */
        if (mc != NULL)
        {
            /*
                Remember: if MEMF_REVERSE is set
                p1 and p2 are now invalid.
            */
            p1=mc;
            p2=p1->mc_Next;

            /* Remove the block from the list and return it. */
            if(p2->mc_Bytes == byteSize)
            {
                /* Fits exactly. Just relink the list. */
                p1->mc_Next = p2->mc_Next;
                mc          = p2;
            }
            else
            {
                if(requirements & MEMF_REVERSE)
                {
                    /* Return the last bytes. */
                    p1->mc_Next=p2;
                    mc=(struct MemChunk *)((UBYTE *)p2+p2->mc_Bytes-byteSize);
                }
                else
                {
                    /* Return the first bytes. */
                    p1->mc_Next=(struct MemChunk *)((UBYTE *)p2+byteSize);
                    mc=p2;
                }
                
                p1           = p1->mc_Next;
                p1->mc_Next  = p2->mc_Next;
                p1->mc_Bytes = p2->mc_Bytes-byteSize;
            }
            
            mh->mh_Free -= byteSize;
        }
    }
    
    return mc;
}

/* The following will compile only if KrnAllocPages() is present!!! */
#ifdef KrnAllocPages

/* Allocate a region managed by own header */
APTR AllocMemHeader(IPTR size, ULONG flags, UWORD prot, struct ExecBase *SysBase)
{
    struct MemHeader *mh;

    mh = KrnAllocPages(size, flags, prot);
    if (mh)
    {
        struct MemHeader *orig = FindMem(mh, SysBase);

    	size -= MEMHEADER_TOTAL;

	/*
	 * Initialize new MemHeader.
	 * Copy some attributes from the original one.
	 */
	mh->mh_Node.ln_Name	= orig->mh_Node.ln_Name;
	mh->mh_Node.ln_Type	= NT_MEMORY;
	mh->mh_Node.ln_Pri      = orig->mh_Node.ln_Pri;
	mh->mh_Attributes	= orig->mh_Attributes;
	mh->mh_Lower 	    	= (APTR)mh + MEMHEADER_TOTAL;
	mh->mh_Upper 	    	= mh->mh_Lower + size - 1;
	mh->mh_First	    	= (struct MemChunk *)mh->mh_Lower;
	mh->mh_Free  	    	= size;

	/* Create the first (and the only) MemChunk */
	mh->mh_First->mc_Next 	= NULL;
	mh->mh_First->mc_Bytes  = size;
    }
    return mh;
}

/* Free a region allocated by AllocMemHeader() */
void FreeMemHeader(APTR addr, struct ExecBase *SysBase)
{
    KrnFreePages(addr, ((struct MemHeader *)addr)->mh_Upper - addr + 1);
}

/* Allocate puddle of a requested size with given flags and add it to the pool */
APTR AllocPuddle(struct Pool *pool, IPTR size, ULONG flags, struct ExecBase *SysBase)
{
    APTR ret;
    KRN_MapAttr prot = MAP_Readable|MAP_Writable|MAP_Executable;

    ret = AllocMemHeader(size, flags, prot, SysBase);
    if (ret)
    	AddTail((struct List *)&pool->PuddleList, ret);

    return ret;
}

/*
 * Allocate memory with given physical properties from the given pool.
 * Our pools can be mixed. This means that different puddles from the
 * pool can have different physical flags. For example the same pool
 * can contain puddles from both CHIP and FAST memory. This is done in
 * order to provide a single system default pool for all types of memory.
 */
APTR InternalAllocPooled(APTR poolHeader, IPTR memSize, ULONG flags, struct ExecBase *SysBase)
{
    struct ProtectedPool *pool = poolHeader + MEMHEADER_TOTAL;
    APTR    	    	 ret = NULL;
    struct MemHeader *mh;

    if (pool->pool.Requirements & MEMF_SEM_PROTECTED)
    {
    	ObtainSemaphore(&pool->sem);
    }

    if (memSize > pool->pool.ThreshSize)
    {
        /*
         * If the memSize is bigger than the ThreshSize allocate seperately.
         * Our allocation size is always page-aligned, so we will likely have
         * some unused space beyond the requested region. We will make use of
         * it for our pool, in order to to this we actually allocate another
         * puddle, of increased size. It will also contain the MemHeader.
         */
	IPTR align = PrivExecBase(SysBase)->PageSize - 1;
	/* Get enough memory for the memory block including the header. */
        IPTR blockSize = memSize + MEMHEADER_TOTAL;

        /* Align the size up to page boundary */
        blockSize = (blockSize + align) & ~align;

	mh = AllocPuddle(&pool->pool, blockSize, flags, SysBase);
	if (mh)
	    /* Allocate the requested memory from the new header */
	    ret = Allocate(mh, memSize);
    }
    else
    {
	/* Follow the list of MemHeaders */
	mh = (struct MemHeader *)pool->pool.PuddleList.mlh_Head;
	for(;;)
	{
	    /* Are there no more MemHeaders? */
	    if(mh->mh_Node.ln_Succ==NULL)
	    {
	    	/* Get a new one */
	    	mh = AllocPuddle(&pool->pool, pool->pool.PuddleSize, flags, SysBase);

		/* No memory left? */
		if(mh == NULL)
		    goto done;

		/* Fall through to get the memory */
	    }
	    else
	    {
	    	/* Ignore existing MemHeaders with memory type that differ from the requested ones */
	    	if (flags & MEMF_PHYSICAL_MASK & ~mh->mh_Attributes)
	    	    continue;
	    }

	    /* Try to get the memory */
	    ret = Allocate(mh, memSize);

	    /* Got it? */
	    if(ret != NULL)
            {
            	/*
		 * If this is not the first MemHeader and it has some free space,
		 * move it forward (so that the next allocation will attempt to use it first).
		 * We use Enqueue() because we still sort MemHeaders according to their priority
		 * (which they inherit from system MemHeaders).
		 */
            	if (mh->mh_Node.ln_Pred != NULL && mh->mh_Free > 32)
            	{
                    Remove((struct Node *)mh);
                    Enqueue((struct List *)&pool->pool.PuddleList, (struct Node *)&mh->mh_Node);
            	}

                break;
            }

	    /* No. Try next MemHeader */
	    mh = (struct MemHeader *)mh->mh_Node.ln_Succ;
	}
	/* Allocate does not clear the memory! */
	if(flags & MEMF_CLEAR)
	{
	    IPTR *p= ret;

	    /* Round up (clearing IPTRs is faster than just bytes) */
	    memSize = (memSize + sizeof(IPTR) - 1) / sizeof(IPTR);

	    /* NUL the memory out */
	    while(memSize--)
		*p++=0;
	}
    }

done:
    if (pool->pool.Requirements & MEMF_SEM_PROTECTED)
    {
    	ReleaseSemaphore(&pool->sem);
    }
    
    /* Everything fine */
    return ret;
}

#endif
