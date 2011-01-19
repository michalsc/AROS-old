/*
    Copyright � 1995-2007, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: english
*/

#define DEBUG 0
#include <aros/debug.h>

#include <proto/expansion.h>

#include "expansion_intern.h"

#include <clib/expansion_protos.h>
#include <proto/exec.h>
#include <exec/resident.h>
#include <aros/asmcall.h>

// ROMTAG INIT time
static void romtaginit(struct ExpansionBase *ExpansionBase)
{
	struct Node *node;
	// look for possible romtags in expansion ROM image and InitResident() them if found
	D(bug("romtaginit\n"));
	ForeachNode(&IntExpBase(ExpansionBase)->eb_BoardList, node) {
		struct ConfigDev *configDev = (struct ConfigDev*)node;
		if (configDev->cd_Rom.er_DiagArea && (configDev->cd_Rom.er_DiagArea->da_Config & DAC_BOOTTIME) == DAC_CONFIGTIME) {
			struct Resident *res;
			UWORD *romptr = (UWORD*)configDev->cd_Rom.er_DiagArea;
			UWORD *romend = (UWORD*)(((UBYTE*)configDev->cd_Rom.er_DiagArea) + configDev->cd_Rom.er_DiagArea->da_Size);
			SetCurrentBinding(&configDev, 4);
			while (romptr < romend - sizeof (struct Resident*)) {
				res = (struct Resident*)romptr;
				if (res->rt_MatchWord == RTC_MATCHWORD && res->rt_MatchTag == res && res->rt_Pri < 105) {
					D(bug("initresident %x '%s'\n", res, res->rt_Name));
					InitResident(res, NULL);
					romptr += 13; //sizeof (struct Resident);
				} else {
					romptr += 1;
				}
			}
		}
	}
}

extern UBYTE _rom_start;
extern UBYTE _ext_start;

static ULONG checkramrom(UBYTE *addr, ULONG size, ULONG mask)
{
	/* check if our "rom" is loaded to ram */
	if (addr < &_rom_start && addr + size > &_rom_start)
		size = (&_rom_start - addr) & ~mask;
	return size;
}

AROS_UFP3(ULONG, MemoryTest,
    AROS_UFPA(APTR, startaddr, A0),
    AROS_UFPA(APTR, endaddr, A1),
    AROS_UFPA(ULONG, block, D0));

static ULONG autosize(struct ExpansionBase *ExpansionBase, struct ConfigDev *configDev)
{
	UBYTE sizebits = configDev->cd_Rom.er_Flags & ERT_Z3_SSMASK;
	ULONG maxsize = configDev->cd_BoardSize;
	ULONG size = 0;
	UBYTE *addr = (UBYTE*)configDev->cd_BoardAddr;

	D(bug("sizebits=%x\n", sizebits));
	if (sizebits >= 14) /* 14 and 15 = reserved */
		return 0;
	if (sizebits >= 2 && sizebits <= 8)
		return 0x00010000 << (sizebits - 2);
	if (sizebits >= 9)
		return 0x00600000 + (0x200000 * (sizebits - 9));
	maxsize = checkramrom(addr, maxsize, 0x7ffff);
	if (!maxsize)
		return 0;
	size = AROS_UFC3(ULONG, MemoryTest,
		AROS_UFCA(APTR, addr, A0),
		AROS_UFCA(APTR, addr + maxsize, A1),
		AROS_UFCA(ULONG, 0x80000, D0));
	D(bug("size=%x maxsize=%x\n", size, maxsize));
	return size;
}

static void allocram(struct ExpansionBase *ExpansionBase)
{
	struct Node *node;
	
	// we should merge address spaces, later..
	D(bug("adding ram boards\n"));
	ForeachNode(&IntExpBase(ExpansionBase)->eb_BoardList, node) {
		struct ConfigDev *configDev = (struct ConfigDev*)node;
		if ((configDev->cd_Rom.er_Type & ERTF_MEMLIST) && !(configDev->cd_Flags & CDF_SHUTUP) && !(configDev->cd_Flags & CDF_PROCESSED)) {
			ULONG attr = MEMF_PUBLIC | MEMF_FAST;
			ULONG size = configDev->cd_BoardSize;
			APTR addr = configDev->cd_BoardAddr;
			LONG pri = 20;
			if (configDev->cd_BoardAddr <= (APTR)0x00FFFFFF) {
				attr |= MEMF_24BITDMA;
				pri = 0;
			} else if ((configDev->cd_Rom.er_Flags & ERT_Z3_SSMASK) != 0) {
				size = autosize(ExpansionBase, configDev);
			}
			if (size && size <= configDev->cd_BoardSize) {
				size = checkramrom(addr, size, 65535);
				if (size) {
				    D(bug("ram board at %08x, size %08x attr %08x\n", addr, size, attr));
				    AddMemList(size, attr, pri, addr, "Fast Memory");
				}
			}
			configDev->cd_Flags |= CDF_PROCESSED;
		}
	}
	D(bug("ram boards done\n"));
}
	

AROS_LH1(void, ConfigChain,
	AROS_LHA(APTR, baseAddr, A0),
	struct ExpansionBase *, ExpansionBase, 11, Expansion)
{
    AROS_LIBFUNC_INIT

	struct ConfigDev *configDev = NULL;

	if (baseAddr == 0) {
		// called by strap
		romtaginit(ExpansionBase);
		return;
	}
bug("configchain\n");
	for(;;) {
		if (!configDev)
			configDev = AllocConfigDev();
		if (!configDev)
			break;
		if (!ReadExpansionRom(baseAddr, configDev)) {
			FreeConfigDev(configDev);
			break;
		}
		if (ConfigBoard(baseAddr, configDev)) {
			AddTail(&IntExpBase(ExpansionBase)->eb_BoardList, &configDev->cd_Node);
			configDev = NULL;
		}
	}

	allocram(ExpansionBase);

    AROS_LIBFUNC_EXIT
} /* ConfigChain */
