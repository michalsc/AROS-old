/*
    Copyright � 1995-2007, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: english
*/

#define DEBUG 0
#include <aros/debug.h>
#include "expansion_intern.h"
#include <proto/expansion.h>
#include <aros/asmcall.h>

/* do not touch. Ugly hack. UAE direct JIT versions need this */
/* check UAE expansion.c for ugly details */
AROS_UFH5(void, writeexpansion,
    AROS_UFHA(APTR, board, A0),
    AROS_UFHA(APTR, configdev, A3), // <- configdev = A3. This is important.
    AROS_UFHA(UBYTE, type, D0),
    AROS_UFHA(UWORD, startaddr, D1),
    AROS_UFHA(struct ExpansionBase *, ExpansionBase, A6))
{ 
    AROS_USERFUNC_INIT

    if (type == ERT_ZORROII) {
	WriteExpansionByte(board, 18, startaddr);
    } else {
	WriteExpansionWord(board, 17, startaddr);
    }

    AROS_USERFUNC_EXIT
}

/*****************************************************************************

    NAME */
#include <clib/expansion_protos.h>

	AROS_LH2(BOOL, ConfigBoard,

/*  SYNOPSIS */
	AROS_LHA(APTR              , board, A0),
	AROS_LHA(struct ConfigDev *, configDev, A1),

/*  LOCATION */
	struct ExpansionBase *, ExpansionBase, 10, Expansion)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    expansion_lib.fd and clib/expansion_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

	UBYTE type = configDev->cd_Rom.er_Type & ERT_TYPEMASK;
	BOOL memorydevice;
	UBYTE *space;
	ULONG align;
	ULONG size = configDev->cd_BoardSize;
	ULONG start, end, addr;
	
	D(bug("Configuring board: mfg=%d prod=%d size=%08x\n",
	    configDev->cd_Rom.er_Manufacturer, configDev->cd_Rom.er_Product, size));

	memorydevice = (configDev->cd_Rom.er_Type & ERTF_MEMLIST) != 0;
	if (type == ERT_ZORROII) {
		start = 0x00200000;
		end   = 0x009FFFFF;
		space = IntExpBase(ExpansionBase)->eb_z2Slots;
		align = configDev->cd_BoardSize;
	} else {
		start = size > 0x20000000 ? 0x10000000 : 0x40000000;
		end   = 0x7FFFFFFF;
		space = IntExpBase(ExpansionBase)->eb_z3Slots;
		align = 0x01000000;
	}
	if (!memorydevice && type == ERT_ZORROII) {
		start = 0x00E90000;
		end   = 0x00EFFFFF;
		align = size;
		/* Blizzard 128k expansion must be at 0x00EA0000
		 * 128k boards must have even start address? */
		if (size > E_SLOTSIZE)
			start = 0x00EA0000;
	}
	D(bug("Configuration area %08x to %08x, block %08x\n", start, end, align));

	for (addr = start; addr < end; addr += align) {
		ULONG startaddr = addr;
		UWORD offset = startaddr / (E_SLOTSIZE * SLOTSPERBYTE);
		BYTE bit = 7 - ((startaddr / E_SLOTSIZE) % SLOTSPERBYTE);
		UBYTE res = space[offset];
		ULONG sizeleft = size;

		if (res & (1 << bit))
			continue;

		// found free start address
		if (size >= E_SLOTSIZE * SLOTSPERBYTE) {
			// needs at least 1 byte and is always aligned to byte
			while (space[offset] == 0 && sizeleft > 0 && offset <= end / (E_SLOTSIZE * SLOTSPERBYTE)) {
				offset++;
				sizeleft -= E_SLOTSIZE * SLOTSPERBYTE;
			}
		} else {
			// bit by bit small board check (fits in one byte)
			while ((res & (1 << bit)) == 0 && sizeleft > 0 && bit >= 0) {
				sizeleft -= E_SLOTSIZE;
				bit--;
			}
		}
		if (sizeleft > 0)
			continue;
		
		configDev->cd_BoardAddr	 = (APTR)startaddr;
		AROS_UFC5(void, writeexpansion,
			AROS_UFCA(ULONG, board, A0),
			AROS_UFCA(ULONG, configDev, A3),
			AROS_UFCA(UBYTE, type, D0),
                	AROS_UFCA(UWORD, (startaddr >> 16), D1),
                       	AROS_UFCA(struct ExpansionBase*, ExpansionBase, A6)
             	);
		D(bug("Configured at %08x\n", configDev->cd_BoardAddr));
		
		// do not remove this, configDev->cd_BoardAddr
		// might have changed inside writeexpansion
		startaddr = (ULONG)configDev->cd_BoardAddr;
		offset = startaddr / (E_SLOTSIZE * SLOTSPERBYTE);
		bit = 7 - ((startaddr / E_SLOTSIZE) % SLOTSPERBYTE);
		sizeleft = size;
		// now allocate area we reserved
		if (size >= E_SLOTSIZE * SLOTSPERBYTE) {
			memset(space + offset, 0xff, sizeleft / (E_SLOTSIZE * SLOTSPERBYTE));
		} else {
			while (sizeleft > 0) {
				space[offset] |= 1 << bit;
				sizeleft -= E_SLOTSIZE;
				bit--;
			}
		}

		return TRUE;
	}
	D(bug("Configuration failed!\n"));
	if (!(configDev->cd_Flags & ERFF_NOSHUTUP)) {
		configDev->cd_Flags |= CDF_SHUTUP;
		WriteExpansionByte(board, 19, 0); // SHUT-UP!
	} else {
		// uh?
	}
	return FALSE;

    AROS_LIBFUNC_EXIT
} /* ConfigBoard */

