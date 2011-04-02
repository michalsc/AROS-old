/*
    Copyright � 1995-2008, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Paula audio.device
    Lang: English
*/

#define DEBUG 0
#include <aros/debug.h>

#include <exec/resident.h>
#include <exec/errors.h>
#include <exec/memory.h>
#include <exec/lists.h>
#include <exec/alerts.h>
#include <exec/tasks.h>
#include <exec/interrupts.h>
#include <devices/audio.h>
#include <hardware/intbits.h>
#include <hardware/custom.h>

#include <proto/exec.h>

#include "audio_intern.h"

void audiohw_stop(struct AudioBase *ab, UWORD mask)
{
    volatile struct Custom *custom = (struct Custom*)0xdff000;

    if (!mask)
    	return;
    custom->dmacon = mask;
    custom->intena = mask << INTB_AUD0;
}

void audiohw_preparept(struct AudioBase *ab, struct IOAudio *io, UBYTE ch)
{
    volatile struct Custom *custom = (struct Custom*)0xdff000;

    if (io) {
    	custom->aud[ch].ac_ptr = (UWORD*)io->ioa_Data;
    	custom->aud[ch].ac_len = io->ioa_Length / 2;
    	ab->cycles[ch] = io->ioa_Cycles;
    	D(bug("ch%d: pt=%08x len=%d cyc=%d\n", ch, io->ioa_Data, io->ioa_Length / 2, io->ioa_Cycles));
    } else {
    	custom->aud[ch].ac_ptr = ab->zerosample;
    	custom->aud[ch].ac_len = 1;
    	ab->cycles[ch] = 1;
    	D(bug("ch%d: null\n", ch));
    }
    ab->initialcyclemask |= 1 << ch;
}

void audiohw_preparepervol(struct AudioBase *ab, struct IOAudio *io, UBYTE ch)
{
    volatile struct Custom *custom = (struct Custom*)0xdff000;

    if (io && (io->ioa_Request.io_Flags & ADIOF_PERVOL)) {
    	custom->aud[ch].ac_per = io->ioa_Period;
    	custom->aud[ch].ac_vol = io->ioa_Volume;
    	D(bug("ch%d: per=%d vol=%d\n", ch, io->ioa_Period, io->ioa_Volume));
    }
}

static void audioirq(struct AudioBase *ab, UBYTE ch)
{
    struct IOAudio *io = getnextwrite(ab, ch, FALSE);
    UBYTE mask = 1 << ch;

    D(bug("audio: ch %d interrupt, io %p %04x %d\n", ch, io, ab->initialcyclemask, ab->cycles[ch]));

    if (!io || (ab->stopmask & mask)) {
        audiohw_stop(ab, mask);
        D(bug("audio: finished\n"));
        return;
    }
    
    if (!(ab->initialcyclemask & mask)) {
    	ab->initialcyclemask |= mask;
    	if (io->ioa_Request.io_Flags & ADIOF_WRITEMESSAGE)
    	    ReplyMsg(&io->ioa_WriteMsg);
    	io = getnextwrite(ab, ch, TRUE);
        D(bug("audio: initial next io %p\n", io));
        audiohw_preparept(ab, io, ch);
    } else {
        struct IOAudio *wio, *next;
    	struct IOAudio *io2 = getnextwrite(ab, ch, TRUE);
    	audiohw_preparepervol(ab, io2, ch);
        if (ab->cycles[ch] == 1) {
    	    REMOVE(io);
    	    ReplyMsg((struct Message*)io);
    	    io = getnextwrite(ab, ch, TRUE);
            audiohw_preparept(ab, io, ch);
            D(bug("audio: next io %p\n", io));
    	} else {
	    ab->cycles[ch]--;
	}
    	ForeachNodeSafe(&ab->misclist, wio, next) {
    	    UWORD cmd = wio->ioa_Request.io_Command;
    	    UBYTE cmask = (UBYTE)(ULONG)wio->ioa_Request.io_Unit;
    	    if (cmd != ADCMD_PERVOL && cmd != ADCMD_FINISH && cmd != ADCMD_WAITCYCLE)
    	    	continue;
    	    if (!(cmask & mask))
    	    	continue;
    	    cmask &= ~(mask << NR_CH);
    	    if ((cmask >> NR_CH) == 0) {
     	        D(bug("audio: ch %d SYNCCYLE woken up, io %p\n", ch, wio));
    	        REMOVE(wio);
    	        ReplyMsg((struct Message*)wio);
    	    }
    	}

    }
}

AROS_UFH4(ULONG, audio_int,
    AROS_UFHA(ULONG, dummy, A0),
    AROS_UFHA(void *, data, A1),
    AROS_UFHA(ULONG, dummy2, A5),
    AROS_UFHA(struct ExecBase *, mySysBase, A6))
{ 
    AROS_USERFUNC_INIT

    volatile struct Custom *custom = (struct Custom*)0xdff000;
    struct AudioBase *ab = data;
    UWORD irqmask = custom->intreqr;
    D(bug("intreqr=%04x\n", irqmask));
    if (irqmask & INTF_AUD0)
    	audioirq(ab, 0);
    if (irqmask & INTF_AUD1)
    	audioirq(ab, 1);
    if (irqmask & INTF_AUD2)
    	audioirq(ab, 2);
    if (irqmask & INTF_AUD3)
    	audioirq(ab, 3);

    return 0;

    AROS_USERFUNC_EXIT
}


void audiohw_reset(struct AudioBase *ab, UWORD mask)
{
    volatile struct Custom *custom = (struct Custom*)0xdff000;
    struct Interrupt *inter;
    UBYTE ch;

    Disable();
    custom->adkcon = mask | (mask << 4);
    custom->dmacon = mask;
    for (ch = 0; ch < NR_CH; ch++) {
    	if (!(mask & (1 << ch)))
    	    continue;
	inter = &ab->audint[ch];
	inter->is_Code = (APTR)audio_int;
	inter->is_Data = ab;
	inter->is_Node.ln_Name = "audio";
	inter->is_Node.ln_Type = NT_INTERRUPT;
	SetIntVector(INTB_AUD0 + ch, inter);
	custom->aud[ch].ac_vol = 0;
	custom->aud[ch].ac_per = 1000;
    }
    custom->intena = mask << INTB_AUD0;
    custom->intreq = mask << INTB_AUD0;
    Enable();
}

static void preparech_initial(struct AudioBase *ab, UBYTE ch)
{
    struct IOAudio *io = getnextwrite(ab, ch, FALSE);
    audiohw_preparept(ab, io, ch);
    audiohw_preparepervol(ab, io, ch);
    ab->initialcyclemask &= ~1 << ch;
}	

void audiohw_start(struct AudioBase *ab, UWORD mask)
{
    volatile struct Custom *custom = (struct Custom*)0xdff000;
    UWORD hwmask;
    UBYTE ch;

    if (!mask)
    	return;
    hwmask = 0;
    for (ch = 0; ch < NR_CH; ch++) {
    	if ((mask & (1 << ch)) && getnextwrite(ab, ch, FALSE)) {
    	    hwmask |= 1 << ch;
    	    preparech_initial(ab, ch);
    	}
    }
    D(bug("hw_start: %02x\n", hwmask));
    if (hwmask) {
    	custom->intreq = hwmask << INTB_AUD0;
    	custom->intena = INTF_SETCLR | (hwmask << INTB_AUD0);
       	custom->dmacon = 0x8000 | hwmask;
    }
}

