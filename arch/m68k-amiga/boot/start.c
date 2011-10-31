/*
    Copyright � 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: m68k-amiga bootstrap to exec.
    Lang: english
 */

#include <aros/altstack.h>
#include <aros/kernel.h>
#include <aros/debug.h>
#include <exec/memory.h>
#include <exec/resident.h>
#include <exec/execbase.h>
#include <proto/exec.h>
#include <hardware/cpu/memory.h>

#include "memory.h"

#include "kernel_romtags.h"
#include "kernel_base.h"

#include "exec_intern.h"

#include "amiga_hwreg.h"
#include "amiga_irq.h"

#include "early.h"
#include "debug.h"

#define SS_STACK_SIZE	0x02000

extern const struct Resident Exec_resident;
extern APTR KickMemPtr_Store;
extern void __clear_bss(const struct KernelBSS *bss);

static void protectKick(struct MemHeader *mh, struct MemList *ml);

extern void __attribute__((interrupt)) Exec_Supervisor_Trap (void);

#define _AS_STRING(x)	#x
#define AS_STRING(x)	_AS_STRING(x)
	
/* Create a sign extending call stub:
 * foo:
 *   jsr AROS_SLIB_ENTRY(funcname, libname, funcid)
 *     0x4eb9 .... ....
 *   ext.w %d0	// EXT_BYTE only
 *     0x4880	
 *   ext.l %d0	// EXT_BYTE and EXT_WORD
 *     0x48c0
 *   rts
 *     0x4e75
 */
#define EXT_BYTE(lib, libname, funcname, funcid) \
	do { \
		void libname##_##funcname##_Wrapper(void) \
		{ asm volatile ( \
			"jsr " AS_STRING(AROS_SLIB_ENTRY(funcname, libname, funcid)) "\n" \
			"ext.w %d0\n" \
			"ext.l %d0\n" \
			"rts\n"); } \
		/* Insert into the library's jumptable */ \
		__AROS_SETVECADDR(lib, funcid, libname##_##funcname##_Wrapper); \
	} while (0)
#define EXT_WORD(lib, libname, funcname, funcid) \
	do { \
		void libname##_##funcname##_Wrapper(void) \
		{ asm volatile ( \
			"jsr " AS_STRING(AROS_SLIB_ENTRY(funcname, libname, funcid)) "\n" \
			"ext.l %d0\n" \
			"rts\n"); } \
		/* Insert into the library's jumptable */ \
		__AROS_SETVECADDR(lib, funcid, libname##_##funcname##_Wrapper); \
	} while (0)
/*
 * Create a register preserving call stub:
 * foo:
 *   movem.l %d0-%d1/%a0-%a1,%sp@-
 *     0x48e7 0xc0c0
 *   jsr AROS_SLIB_ENTRY(funcname, libname, funcid)
 *     0x4eb9 .... ....
 *   movem.l %sp@+,%d0-%d1/%d0-%a1
 *     0x4cdf 0x0303
 *   rts
 *     0x4e75
 */
#define PRESERVE_ALL(lib, libname, funcname, funcid) \
	do { \
		void libname##_##funcname##_Wrapper(void) \
	        { asm volatile ( \
	        	"movem.l %d0-%d1/%a0-%a1,%sp@-\n" \
	        	"jsr " AS_STRING(AROS_SLIB_ENTRY(funcname, libname, funcid)) "\n" \
	        	"movem.l %sp@+,%d0-%d1/%a0-%a1\n" \
	        	"rts\n" ); } \
		/* Insert into the library's jumptable */ \
		__AROS_SETVECADDR(lib, funcid, libname##_##funcname##_Wrapper); \
	} while (0)
/* Inject arbitrary code into the jump table
 * Used for GetCC and nano-stubs
 */
#define FAKE_IT(lib, libname, funcname, funcid, ...) \
	do { \
		UWORD *asmcall = (UWORD *)__AROS_GETJUMPVEC(lib, funcid); \
		const UWORD code[] = { __VA_ARGS__ }; \
		asmcall[0] = code[0]; \
		asmcall[1] = code[1]; \
		asmcall[2] = code[2]; \
	} while (0)
/* Inject a 'move.w #value,%d0; rts" sequence into the
 * jump table, to fake an private syscall.
 */
#define FAKE_ID(lib, libname, funcname, funcid, value) \
	FAKE_IT(lib, libname, funcname, funcid, 0x303c, value, 0x4e75)

extern void SuperstackSwap(void);
/* This calls the register-ABI library
 * routine Exec/InitCode, for use in NewStackSwap()
 */
static LONG doInitCode(void)
{
	/* Attempt to allocate a new supervisor stack */
	do {
	    APTR ss_stack;

	    ss_stack = AllocMem(SS_STACK_SIZE, MEMF_ANY | MEMF_CLEAR | MEMF_REVERSE);
	    if (ss_stack && ((ULONG)ss_stack & (PAGE_SIZE - 1))) {
	    	/* Normally ss_stack is page aligned because it is first MEMF_REVERSE
	    	 * allocation. But we must check it because enabled mungwall or expansion
	    	 * boot rom code can allocate some memory.
	    	 */
	    	FreeMem(ss_stack, SS_STACK_SIZE);
	        ss_stack = AllocMem(SS_STACK_SIZE + PAGE_SIZE - 1, MEMF_ANY | MEMF_CLEAR | MEMF_REVERSE);
	        ss_stack = (APTR)(((ULONG)ss_stack + PAGE_SIZE - 1) & PAGE_MASK);
	    }
	    DEBUGPUTHEX(("SS  lower", (ULONG)ss_stack));
	    DEBUGPUTHEX(("SS  upper", (ULONG)ss_stack + SS_STACK_SIZE - 1));
	    if (ss_stack == NULL) {
	    	DEBUGPUTS(("Strange. Can't allocate a new system stack\n"));
	    	Early_Alert(CODE_ALLOC_FAIL);
	    	break;
	    }
            SysBase->SysStkLower    = ss_stack;
            SysBase->SysStkUpper    = ss_stack + SS_STACK_SIZE;
	    SetSysBaseChkSum();

	    Supervisor((ULONG_FUNC)SuperstackSwap);
	} while(0);

	InitCode(RTF_COLDSTART, 0);

	return 0;
}

extern BYTE _rom_start;
extern BYTE _rom_end;
extern BYTE _ext_start;
extern BYTE _ext_end;
extern BYTE _bss;
extern BYTE _bss_end;
extern BYTE _ss;
extern BYTE _ss_end;

/* Protect the 'ROM' if it is actually in RAM.
 */
static APTR protectAlloc(struct MemHeader *mh, APTR start, APTR end, const UBYTE *name, BOOL pagealign)
{
    APTR tmp;
    ULONG length;
    
    if (pagealign) {
    	start = (APTR)((ULONG)start & PAGE_MASK);
    	length = (end - start + PAGE_SIZE - 1) & PAGE_MASK;
    } else {
    	length = end - start;
    }
    tmp = Early_AllocAbs(mh, start, length);
    DEBUGPUTS(("* "));
    if (name)
	DEBUGPUTS((name));
    else
    	DEBUGPUTS(("<no name>"));
    if (!tmp)
    	DEBUGPUTS((" !"));
    DEBUGPUTHEX(("\nStart  ", (ULONG)start));
    DEBUGPUTHEX(("End    ", (ULONG)end - 1));
    DEBUGPUTHEX(("Size   ", length));
    return tmp;
}
static void protectROM(struct MemHeader *mh)
{
    DEBUGPUTHEX(("Protect", (IPTR)mh));
    protectAlloc(mh, &_ss, &_ss_end, "SS", FALSE);
    protectAlloc(mh, &_bss, &_bss_end, "BSS", FALSE);
    DEBUGPUTHEX(("First  ", (IPTR)mh->mh_First));
    DEBUGPUTHEX(("Bytes  ", (IPTR)mh->mh_First->mc_Bytes));
}

static struct MemHeader *addmemoryregion(ULONG startaddr, ULONG size, struct MemList *ml)
{
	if (size < 65536)
		return NULL;
	if (startaddr < 0x00c00000) {
		krnCreateMemHeader("chip memory", -10,
			(APTR)startaddr, size,
			 MEMF_CHIP | MEMF_KICK | MEMF_PUBLIC | MEMF_LOCAL | MEMF_24BITDMA);
	} else {
		krnCreateMemHeader(startaddr < 0x01000000 ? "memory" : "expansion memory",
			startaddr < 0x01000000 ? -5 : (startaddr < 0x08000000 ? 30 : 40),
			(APTR)startaddr, size,
			MEMF_FAST | MEMF_KICK | MEMF_PUBLIC | MEMF_LOCAL | (startaddr < 0x01000000 ? MEMF_24BITDMA : 0));
	}

        /* Must be done first, in case BSS and SS are in it */
	protectKick((struct MemHeader*)startaddr, ml);

	protectROM((struct MemHeader*)startaddr);
	return (struct MemHeader*)startaddr;
}

static BOOL IsSysBaseValidNoVersion(struct ExecBase *sysbase)
{
    if (sysbase == NULL || (((ULONG)sysbase) & 0x80000001))
	return FALSE;
    if (sysbase->ChkBase != ~(IPTR)sysbase)
	return FALSE;
    return GetSysBaseChkSum(sysbase) == 0xffff;
}

/* In the following functions, we use bit 0 of the
 * MemEntry->me_Addr to indicate whether or not
 * that MemEntry in the MemList has been 'locked down'
 *
 *  (addr & 1) = unallocated
 * !(addr & 1) = allocated
 */

/* Remove all bit 0 marks
 */
/* We support up to 32 KickMemPtr tags in pre-expansion.library RAM */
static ULONG protectKickBits;

static void markKick(struct MemList *ml)
{
    int ndx;
    for (ndx = 0; ml ; ml = (struct MemList*)ml->ml_Node.ln_Succ) {
        int i;
        for (i = 0; i < ml->ml_NumEntries; i++, ndx++) {
            if (ndx < 32)
                protectKickBits |= (1 << ndx);
        }
    }
}

static void protectKick(struct MemHeader *mh, struct MemList *ml)
{
    int ndx = 0;

    DEBUGPUTHEX(("protectKick", (IPTR)ml));

    if (!mh)
        return;

    while (ml) {
	int i;
	DEBUGPUTHEX(("NumEntries", ml->ml_NumEntries));
	for (i = 0; i < ml->ml_NumEntries; i++, ndx++) {
	    APTR start = (APTR)((IPTR)ml->ml_ME[i].me_Addr & ~1);
	    APTR end = start + ml->ml_ME[i].me_Length;

            /* Already allocated? */
	    if (ndx >= 32 || !(protectKickBits & (1 << ndx)))
	        continue;

	    if ((mh->mh_Lower > start)  ||
	        (mh->mh_Upper < end))
	        continue;

            if (protectAlloc(mh, start, end, ml->ml_Node.ln_Name, FALSE)) {
                protectKickBits &= ~(1 << ndx);
            }
	}
	ml = (struct MemList*)ml->ml_Node.ln_Succ;
    }

    return;
}

static BOOL InitKickMem(struct ExecBase *SysBase)
{
    int ndx = 0;
    struct MemList *ml = SysBase->KickMemPtr;

    DEBUGPUTHEX(("KickMemPtr", (IPTR)ml));

    while (ml) {
	int i;
	DEBUGPUTHEX(("NumEntries", ml->ml_NumEntries));
	for (i = 0; i < ml->ml_NumEntries; i++,ndx++) {
	    APTR start = ml->ml_ME[i].me_Addr;
	    ULONG len = ml->ml_ME[i].me_Length;

            /* Already allocated? */
	    if (ndx < 32 && !(protectKickBits & (1 << ndx)))
	        continue;

	    DEBUGPUTHEX(("      Addr", (IPTR)start));
	    DEBUGPUTHEX(("       Len", len));

	    /* Use the non-mungwalling AllocAbs */
	    if (!InternalAllocAbs(start, len, SysBase))
		return FALSE;
	}
	ml = (struct MemList*)ml->ml_Node.ln_Succ;
    }

    return TRUE;
}
#if 0 // debug stuff, do not remove
static ULONG SumKickDataX(struct ExecBase *sb)
{
    ULONG chksum = 0;
    BOOL isdata = FALSE;
    struct ExecBase *sysbase = sb;

    if (sysbase->KickTagPtr) {
    	IPTR *list = sysbase->KickTagPtr;
 	while(*list)
	{
   	    chksum += (ULONG)*list;
   	    DEBUGPUTHEX(("LIST", (ULONG)list));
   	    DEBUGPUTHEX(("LISTP", (ULONG)*list));
   	    DEBUGPUTHEX(("CHK", chksum));
            /* on amiga, if bit 31 is set then this points to another list of
             * modules rather than pointing to a single module. bit 31 is
             * inconvenient on architectures where code may be loaded above
             * 2GB. on these platforms we assume aligned pointers and use bit
             * 0 instead */
#ifdef __mc68000__
	    if(*list & 0x80000000) { list = (IPTR *)(*list & 0x7fffffff); continue; }
#else
            if(*list & 0x1) { list = (IPTR *)(*list & ~(IPTR)0x1); continue; }
#endif
	    list++;
   	    isdata = TRUE;
   	}
    }

    if (sysbase->KickMemPtr) {
	struct MemList *ml = (struct MemList*)sysbase->KickMemPtr;
	while (ml) {
	    UBYTE i;
	    ULONG *p = (ULONG*)ml;
	    for (i = 0; i < sizeof(struct MemList) / sizeof(ULONG); i++)
	    	chksum += p[i];
	    DEBUGPUTHEX(("MEM", (ULONG)p));
	    DEBUGPUTHEX(("CHK", chksum));
	    ml = (struct MemList*)ml->ml_Node.ln_Succ;
	    isdata = TRUE;
	}
    }
    if (isdata && !chksum)
    	chksum--;
    return chksum;
}
#endif

void doColdCapture(void)
{
    APTR ColdCapture = SysBase->ColdCapture;
    if (ColdCapture == NULL)
    	return;

    if (((ULONG *)ColdCapture)[1] == AROS_MAKE_ID('F','A','K','E')) {
    	/* Fake SysBase installed by AROSBootstrap.
    	 *
    	 * In this case, ColdCapture is the trampoline executed
    	 * by the AOS ROM to get into AROS. We need to keep
    	 * ColdCapture around in AROS SysBase, but we don't
    	 * want to execute it (and cause an infinite loop).
    	 */
    	DEBUGPUTS(("[ColdCapture] Ignoring AOS->AROS trampoline\n"));
    	return;
    }

    SysBase->ColdCapture = NULL;
    /* ColdCapture calling method is a little
     * strange. It's in supervisor mode, requires
     * the return location in A5, and SysBase in A6.
     */
    asm volatile (
    	"move.l %0,%%a0\n"
    	"move.l %1,%%a6\n"
    	"move.l #0f,%%a5\n"
    	"jmp (%%a0)\n"
    	"0:\n"
    	:
    	: "m" (ColdCapture), "m" (SysBase)
    	: "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
    	  "a0", "a1", "a2", "a3", "a4", "a5", "a6");
}

static void RomInfo(IPTR rom)
{
    APTR ptr = (APTR)rom;
    CONST_STRPTR str;

    if ((*(UWORD *)(ptr + 8) == 0x0000) &&
        (*(UWORD *)(ptr + 10) == 0xffff) &&
        (*(UWORD *)(ptr + 12) == *(UWORD *)(ptr + 16)) &&
        (*(UWORD *)(ptr + 14) == *(UWORD *)(ptr + 18)) &&
        (*(UWORD *)(ptr + 20) == 0xffff) &&
        (*(UWORD *)(ptr + 22) == 0xffff)) {
    	DEBUGPUTHEX(("ROM Location", rom));
    	DEBUGPUTD(("   Version", *(UWORD *)(ptr + 12)));
    	DEBUGPUTD(("  Revision", *(UWORD *)(ptr + 14)));
    	str = (ptr + 24);
    	DEBUGPUTS(("  ROM Type: ")); DEBUGPUTS((str)); DEBUGPUTS(("\n"));
    	str += strlen(str) + 1;
    	DEBUGPUTS((" Copyright: ")); DEBUGPUTS((str));
    	str += strlen(str) + 1;
    	DEBUGPUTS((str));
    	str += strlen(str) + 1;
    	DEBUGPUTS((str));
    	DEBUGPUTS(("\n"));
    	str += strlen(str) + 1;
    	DEBUGPUTS((" ROM Model: ")); DEBUGPUTS((str));
    	DEBUGPUTS(("\n"));
    }
}

void exec_boot(ULONG *membanks, ULONG *cpupcr)
{
#if 0
	struct ExecBase *oldSysBase = *(APTR *)4;
	DebugInit();
	DEBUGPUTHEX(("X SysBase->KickCheckSum", (ULONG)oldSysBase->KickCheckSum));
	SumKickDataX(oldSysBase);
#endif

	struct TagItem bootmsg[] = {
#ifdef AROS_SERIAL_DEBUG
	    { KRN_CmdLine, (IPTR)"sysdebug=InitCode" },
#endif
            { KRN_KernelStackBase, (IPTR)&_ss },
            { KRN_KernelStackSize, (IPTR)(&_ss_end - &_ss) },
	    { TAG_END },
	};
	struct TagItem *bootmsgptr = bootmsg;
	volatile APTR *trap;
	int i;
	BOOL wasvalid;
	UWORD *kickrom[8];
	const struct KernelBSS kbss[2] = {
		{
			.addr = &_bss,
			.len = &_bss_end - &_bss,
		}, {
			.addr = 0,
			.len = 0,
		}
	};
	struct MemHeader *mh;
	LONG oldLastAlert[4];
	ULONG oldmem;
	UWORD attnflags;
	APTR ColdCapture = NULL, CoolCapture = NULL, WarmCapture = NULL;
	APTR KickMemPtr = NULL, KickTagPtr = NULL, KickCheckSum = NULL;
	/* We can't use the global 'SysBase' symbol, since
	 * the compiler does not know that PrepareExecBase
	 * may change it out from under us.
	 */
	struct ExecBase *oldSysBase = *(APTR *)4;
#define SysBase CANNOT_USE_SYSBASE_SYMBOL_HERE

#ifdef AROS_SERIAL_DEBUG
        DebugInit();
#endif

	trap = (APTR *)(NULL);

	/* Set all the exceptions to the Early_Exception
	 */
	for (i = 2; i < 64; i++) {
	    if (i != 31)
	    	trap[i] = Early_Exception;
	}

	/* Let the world know we exist
	 */
	DebugInit();
	DEBUGPUTS(("[reset]\n"));
	RomInfo(0xf80000);
	RomInfo(0xe00000);
	RomInfo(0xf00000);

	/* Convert CPU/FPU flags to AttnFlags */
	attnflags = cpupcr[0] & 0xffff;
	if (attnflags & (AFF_68030 | AFF_68040 | AFF_68060))
		attnflags |= AFF_ADDR32;
	if (cpupcr[0] & 0xffff0000) {
		attnflags |= AFF_FPU;
		if (attnflags & (AFF_68040 | AFF_68060))
			attnflags |= AFF_FPU40;
			// AFF_68881 | AFF_68882 set only if 040/060 math emulation running
		else if (((cpupcr[0] >> 16) & 0xff) <= 0x1f)
			attnflags |= AFF_68881;
		else
			attnflags |= AFF_68881 | AFF_68882;
	}

#ifdef AROS_SERIAL_DEBUG
	DEBUGPUTS(("CPU: "));
	if (attnflags & AFF_68060)
		DEBUGPUTS(("68060"));
	else if (attnflags & AFF_68040)
		DEBUGPUTS(("68040"));
	else if (attnflags & AFF_68030)
		DEBUGPUTS(("68030"));
	else if (attnflags & AFF_68020) {
		if (attnflags & AFF_ADDR32)
			DEBUGPUTS(("68020"));
		else
			DEBUGPUTS(("68EC020"));
	} else if (attnflags & AFF_68010)
		DEBUGPUTS(("68010"));
	else
		DEBUGPUTS(("68000"));
	DEBUGPUTS((" FPU: "));
	if (attnflags & AFF_FPU40) {
		if (attnflags & AFF_68060)
			DEBUGPUTS(("68060"));
		else if (attnflags & AFF_68040)
			DEBUGPUTS(("68040"));
		else
			DEBUGPUTS(("-"));
	} else if (attnflags & AFF_68882)
		DEBUGPUTS(("68882"));
	else if (attnflags & AFF_68881)
		DEBUGPUTS(("68881"));
	else
		DEBUGPUTS(("-"));
	DEBUGPUTS(("\n"));
	if (cpupcr[1])
		DEBUGPUTHEX(("PCR", cpupcr[1]));
#endif


	/* Zap out old SysBase if invalid */
	wasvalid = IsSysBaseValid(oldSysBase);
	if (wasvalid) {
	    DEBUGPUTHEX(("[SysBase] was at", (ULONG)oldSysBase));
	} else {
	    wasvalid = IsSysBaseValidNoVersion(oldSysBase);
	    if (wasvalid) {
	    	DEBUGPUTHEX(("[SysBase] fakebase at", (ULONG)oldSysBase));
	    	if (oldSysBase->DebugData)
	    	    bootmsgptr = (struct TagItem*)oldSysBase->DebugData;
	    	wasvalid = TRUE;
	    } else {
	    	DEBUGPUTHEX(("[SysBase] invalid at", (ULONG)oldSysBase));
	    	wasvalid = FALSE;
	    }
	}
    	if (bootmsgptr[0].ti_Tag == KRN_CmdLine) {
	    DEBUGPUTS(("[SysBase] kernel commandline '"));
    	    DEBUGPUTS(((CONST_STRPTR)bootmsgptr[0].ti_Data));
    	    DEBUGPUTS(("'\n"));
    	}

	if (wasvalid) {
	    /* Save reset proof vectors */
	    ColdCapture  = oldSysBase->ColdCapture;
	    CoolCapture  = oldSysBase->CoolCapture;
	    WarmCapture  = oldSysBase->WarmCapture;
	    KickMemPtr   = oldSysBase->KickMemPtr; 
	    KickTagPtr   = oldSysBase->KickTagPtr;
	    KickCheckSum = oldSysBase->KickCheckSum;

	    /* Mark the oldSysBase as processed */
	    oldSysBase = NULL;
	}

	for (i = 0; membanks[i + 1]; i += 2) {
		ULONG addr = membanks[i + 0];
		ULONG size = membanks[i + 1];
		DEBUGPUTHEX(("RAM lower", addr));
		DEBUGPUTHEX(("RAM upper", addr + size - 1));
	}

	kickrom[0] = (UWORD*)&_rom_start;
	kickrom[1] = (UWORD*)&_rom_end;
	kickrom[2] = (UWORD*)0x00f00000;
	kickrom[3] = (UWORD*)0x00f80000;
	kickrom[4] = (UWORD*)&_ext_start;
	kickrom[5] = (UWORD*)&_ext_end;
	kickrom[6] = (UWORD*)~0;
	kickrom[7] = (UWORD*)~0;

	/* Look for 'HELP' at address 0 - we're recovering
	 * from a fatal alert
	 */
	if (trap[0] == (APTR)0x48454c50) {
	    for (i = 0; i < 4; i++)
	    	oldLastAlert[i] = (LONG)trap[64 + i];

	    DEBUGPUTHEX(("LastAlert Alert", oldLastAlert[0]));
	    DEBUGPUTHEX(("LastAlert  Task", oldLastAlert[1]));
	} else {
	    oldLastAlert[0] = (LONG)-1;
	    oldLastAlert[1] = 0;
	    oldLastAlert[2] = 0;
	    oldLastAlert[2] = 0;
	}

	/* Clear alert marker */
	trap[0] = 0;

	DEBUGPUTHEX(("BSS lower", (ULONG)&_bss));
	DEBUGPUTHEX(("BSS upper", (ULONG)&_bss_end - 1));
	DEBUGPUTHEX(("SS  lower", (ULONG)&_ss));
	DEBUGPUTHEX(("SS  upper", (ULONG)&_ss_end - 1));

	Early_ScreenCode(CODE_RAM_CHECK);

	/* Clear the BSS. */
	__clear_bss(&kbss[0]);

        /* Mark all the kick memory as 'unprotected' */
	markKick(KickMemPtr);

	mh = addmemoryregion(membanks[0], membanks[1], KickMemPtr);
	if (mh == NULL) {
	    DEBUGPUTS(("Can't create initial memory header!\n"));
	    Early_Alert(AT_DeadEnd | AG_NoMemory);
	}

	/*
	 * Call the SysBase initialization.
	 */
	Early_ScreenCode(CODE_EXEC_CHECK);
	if (!krnPrepareExecBase(kickrom, mh, bootmsgptr))
	    Early_Alert(AT_DeadEnd | AG_MakeLib | AO_ExecLib);

	/* From here on, we can reference SysBase */
#undef SysBase
	DEBUGPUTHEX(("[SysBase at]", (ULONG)SysBase));

	SysBase->ThisTask->tc_SPLower = &_ss;
        SysBase->ThisTask->tc_SPUpper = &_ss_end;
        aros_init_altstack(SysBase->ThisTask);

    	if (wasvalid) {
    	    SysBase->ColdCapture = ColdCapture;
    	    SysBase->CoolCapture = CoolCapture;
    	    SysBase->WarmCapture = WarmCapture;
    	    SysBase->ChkSum = 0;
    	    SysBase->ChkSum = GetSysBaseChkSum(SysBase) ^ 0xffff; 
    	    SysBase->KickMemPtr = KickMemPtr;
    	    SysBase->KickTagPtr = KickTagPtr;
    	    SysBase->KickCheckSum = KickCheckSum;
    	}

        SysBase->SysStkUpper    = (APTR)&_ss_end;
        SysBase->SysStkLower    = (APTR)&_ss;

        /* Mark what the last alert was */
        for (i = 0; i < 4; i++)
            SysBase->LastAlert[i] = oldLastAlert[i];

	SysBase->AttnFlags = attnflags;

	/* Inject code for GetCC, depending on CPU model */
	if (SysBase->AttnFlags & AFF_68010) {
		/* move.w %ccr,%d0; rts; nop */
		FAKE_IT(SysBase, Exec, GetCC, 88, 0x42c0, 0x4e75, 0x4e71);
	} else {
		/* move.w %sr,%d0; rts; nop */
		FAKE_IT(SysBase, Exec, GetCC, 88, 0x40c0, 0x4e75, 0x4e71);
	}

#ifdef THESE_ARE_KNOWN_SAFE_ASM_ROUTINES
	PRESERVE_ALL(SysBase, Exec, Disable, 20);
	PRESERVE_ALL(SysBase, Exec, Enable, 21);
	PRESERVE_ALL(SysBase, Exec, Forbid, 22);
#endif
	PRESERVE_ALL(SysBase, Exec, Permit, 23);
	PRESERVE_ALL(SysBase, Exec, ObtainSemaphore, 94);
	PRESERVE_ALL(SysBase, Exec, ReleaseSemaphore, 95);
	PRESERVE_ALL(SysBase, Exec, ObtainSemaphoreShared, 113);

	/* Functions that need sign extension */
	EXT_BYTE(SysBase, Exec, SetTaskPri, 50);
	EXT_BYTE(SysBase, Exec, AllocSignal, 55);

	/* Only add the 2 standard ROM locations, since
	 * we may get memory at 0x00f00000, or when we
	 * are ReKicked, at the rom-in-ram locations.
	 */
	krnCreateROMHeader("Kickstart ROM", (APTR)0x00f80000, (APTR)0x00ffffff);
	krnCreateROMHeader("Kickstart ROM", (APTR)0x00e00000, (APTR)0x00e7ffff);

	/* Add remaining memory regions */
	for (i = 2; membanks[i + 1]; i += 2) {
		IPTR  addr = membanks[i];
		ULONG size = membanks[i + 1];

		DEBUGPUTHEX(("RAM Addr: ", addr));
		DEBUGPUTHEX(("RAM Size: ", size));
		mh = addmemoryregion(addr, size, KickMemPtr);
		Enqueue(&SysBase->MemList, &mh->mh_Node);

		/* Adjust MaxLocMem and MaxExtMem as needed */
		if (addr < 0x00200000)
			SysBase->MaxLocMem = (size + 0xffff) & 0xffff0000;
		else if (addr < 0x00d00000)
			SysBase->MaxExtMem = size ? (APTR)(((0xc00000 + (size + 0xffff)) & 0xffff0000)) : 0;
	}

	/* Now that we have a valid SysBase,
	 * we can call ColdCapture
	 */
	if (wasvalid)
	    doColdCapture();

	/* Seal up SysBase's critical variables */
	SetSysBaseChkSum();

	/* Set privilege violation trap - we
	 * need this to support the Exec/Supervisor call
	 */
	trap[8] = Exec_Supervisor_Trap;

	oldmem = AvailMem(MEMF_FAST);
	
	/* SysBase is complete, now we can enable instruction caches safely. */
	CacheControl(CACRF_EnableI, CACRF_EnableI);
	CacheClearU();

	/* Ok, let's start the system. We have to
	 * do this in Supervisor context, since some
	 * expansions ROMs (Cyperstorm PPC) expect it.
	 */
	DEBUGPUTS(("[start] InitCode(RTF_SINGLETASK, 0)\n"));
	InitCode(RTF_SINGLETASK, 0);

	/* Autoconfig ram expansions are now configured */

	/* If oldSysBase is not NULL, that means that it
	 * (a) wasn't valid before when we only had MEMF_LOCAL
	 * ram and (b) could possibly be in the MEMF_KICK memory
	 * we just got. Let's check it and find out if we
	 * can use it's capture vectors.
	 */
	if (oldSysBase && IsSysBaseValidNoVersion(oldSysBase)) {
	    /* Save reset proof vectors */
	    SysBase->ColdCapture  = oldSysBase->ColdCapture;
	    SysBase->CoolCapture  = oldSysBase->CoolCapture;
	    SysBase->WarmCapture  = oldSysBase->WarmCapture;
	    /* Save KickData */
	    SysBase->KickMemPtr   = oldSysBase->KickMemPtr; 
	    SysBase->KickTagPtr   = oldSysBase->KickTagPtr;
	    SysBase->KickCheckSum = oldSysBase->KickCheckSum;
	    doColdCapture();
	    /* Re-seal SysBase */
	    SetSysBaseChkSum();
	    wasvalid = TRUE;
	}

	/* Before we allocate anything else, we need to
	 * lock down the entries in KickMemPtr
	 *
	 * If we get a single failure, don't run any
	 * of the KickTags.
	 */
	if (SysBase->KickCheckSum) {
	    if (SysBase->KickCheckSum == (APTR)SumKickData()) {
		if (!InitKickMem(SysBase)) {
	    	    DEBUGPUTS(("[KickMem] KickMem failed an allocation. Ignoring KickTags\n"));
	    	    SysBase->KickTagPtr = NULL;
	    	}
	    } else {
	    	DEBUGPUTS(("[KickMem] Checksum mismatch\n"));
		SysBase->KickTagPtr = NULL;
	    }
	    KickMemPtr_Store = SysBase->KickMemPtr;
	    SysBase->KickMemPtr = NULL;
	    SysBase->KickCheckSum = (APTR)SumKickData();
	}

	if ((AvailMem(MEMF_FAST) > (oldmem + 256 * 1024)) &&
	    ((TypeOfMem(SysBase) & MEMF_CHIP) ||
	     ((ULONG)SysBase >= 0x00a00000ul && (ULONG)SysBase < 0x01000000ul))) {
	    /* Move execbase to real fast if available now */
	    SysBase = PrepareExecBaseMove(SysBase);
	    DEBUGPUTHEX(("[Sysbase] now at", (ULONG)SysBase));
	}

	/* Initialize IRQ subsystem */
	AmigaIRQInit(SysBase);

	/* Set privilege violation trap again.
	 * AmigaIRQInit may have blown it away.
	 */
	trap[8] = Exec_Supervisor_Trap;

	/* Attempt to allocate a real stack, and switch to it. */
	do {
	    const ULONG size = AROS_STACKSIZE;
	    IPTR *usp;
	
	    usp = AllocMem(size * sizeof(IPTR), MEMF_PUBLIC);
	    if (usp == NULL) {
		DEBUGPUTS(("Can't allocate a new stack for Exec... Strange.\n"));
	    	Early_Alert(CODE_ALLOC_FAIL);
		break;
	    }

	    SysBase->ThisTask->tc_SPUpper = &usp[size];
	    SysBase->ThisTask->tc_SPLower = usp;

	    /* Leave supervisor mode, switch power led on */
	    asm volatile (
	    	"or.b	#2,0xbfe001\n"
	    	"move.l %0,%%usp\n"
	    	"move.w #0,%%sr\n"
	    	"pea.l 0f\n"
	    	"jmp %1@\n"
	    	"0:\n"
	    	:
	    	: "a" (&usp[size-3]),
	    	  "a" (doInitCode)
	    	:);
	} while (0);

	/* We shouldn't get here */
	Early_Alert(CODE_EXEC_FAIL);
	for (;;);
}
