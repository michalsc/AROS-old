#ifndef USB2OTG_INTERN_H
#define USB2OTG_INTERN_H
/*
    Copyright � 2013-2015, The AROS Development Team. All rights reserved.
    $Id$
*/

#include LC_LIBDEFS_FILE

#include <aros/libcall.h>
#include <aros/asmcall.h>
#include <aros/symbolsets.h>

#include <exec/types.h>
#include <exec/lists.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/interrupts.h>
#include <exec/semaphores.h>
#include <exec/execbase.h>
#include <exec/devices.h>
#include <exec/io.h>
#include <exec/ports.h>
#include <exec/errors.h>
#include <exec/resident.h>
#include <exec/initializers.h>
#include <dos/dos.h>

#include <devices/timer.h>
#include <utility/utility.h>

#include <devices/usbhardware.h>
#include <devices/newstyle.h>

#include <oop/oop.h>

extern IPTR __arm_periiobase;
#define ARM_PERIIOBASE __arm_periiobase
#include <hardware/bcm2708.h>
#include <hardware/usb2otg.h>

/*
    Force the USB chipset to run in Host mode
    AFAIK Poseidon doesnt support device mode? - TODO
*/
//#define OTG_FORCEHOSTMODE
//#define OTG_FORCEDEVICEMODE

/* Reply the iorequest with success */
#define RC_OK	      0

/* Magic cookie, don't set error fields & don't reply the ioreq */
#define RC_DONTREPLY  -1

#define MAX_ROOT_PORTS	 16

#define VCMB_PROPCHAN   8
#define VCPOWER_USBHCD  3
#define VCPOWER_STATE_ON    1
#define VCPOWER_STATE_WAIT  2

static inline ULONG rd32le(IPTR iobase) {
    ULONG val;
    asm volatile ("dmb":::"memory");
    val = AROS_LE2LONG(*(volatile ULONG *)(iobase));
    asm volatile ("dsb":::"memory");
    return val;
}

static inline UWORD rd16le(IPTR iobase) {
    UWORD val;
    asm volatile ("dmb":::"memory");
    val = AROS_LE2WORD(*(volatile UWORD *)(iobase));
    asm volatile ("dsb":::"memory");
    return val;
}

static inline UBYTE rd8(IPTR iobase) {
    UBYTE val;
    asm volatile ("dmb":::"memory");
    val = *(volatile UBYTE *)(iobase);
    asm volatile ("dsb":::"memory");
    return val;
}

static inline void wr32le(IPTR iobase, ULONG value) {
    asm volatile ("dsb":::"memory");
    *(volatile ULONG *)(iobase) = AROS_LONG2LE(value);
    asm volatile ("dmb":::"memory");
}

static inline void wr16le(IPTR iobase, UWORD value) {
    asm volatile ("dsb":::"memory");
    *(volatile UWORD *)(iobase) = AROS_WORD2LE(value);
    asm volatile ("dmb":::"memory");
}

static inline void wr8be(IPTR iobase, UBYTE value) {
    asm volatile ("dsb":::"memory");
    *(volatile UBYTE *)(iobase) = value;
    asm volatile ("dmb":::"memory");
}

struct USBNSDeviceQueryResult
{
    ULONG               DevQueryFormat;
    ULONG               SizeAvailable;
    UWORD               DeviceType;
    UWORD               DeviceSubType;
    const UWORD         *SupportedCommands;     /* 0 terminated list of cmd's   */
};

struct USB2OTGUnit
{
    struct Unit         hu_Unit;

    struct List         hu_IOPendingQueue;	/* Root Hub Pending IO Requests */

    struct List         hu_TDQueue;
    struct List         hu_PeriodicTDQueue;
    struct List         hu_CtrlXFerQueue;
    struct List         hu_IntXFerQueue;
    struct List         hu_IntXFerScheduled;
    struct List         hu_IsoXFerQueue;
    struct List         hu_BulkXFerQueue;
    struct List         hu_FinishedXfers;


    struct USB2OTGChannel {
        struct IOUsbHWReq * hc_Request;
        ULONG               hc_XferSize;
    }                   hu_Channel[8];

//    struct IOUsbHWReq * hu_InProgressXFer[8];
//    ULONG               hu_InProgressXFerSize[8];

    struct List         hu_AbortQueue;

    APTR                hu_GlobalIRQHandle;
    struct Interrupt	hu_PendingInt;
    struct Interrupt    hu_NakTimeoutInt;
    struct timerequest  hu_NakTimeoutReq;
    struct MsgPort      hu_NakTimeoutMsgPort;

    UBYTE               hu_OperatingMode;       /* HOST/DEVICE mode */
    UBYTE               hu_HubAddr;
    UBYTE               hu_HostChans;
    UBYTE               hu_DevEPs;
    UBYTE               hu_DevInEPs;

    BOOL                hu_UnitAllocated;       /* unit opened */
    BOOL                hu_HubPortChanged;      /* Root port state change */
    APTR                hu_USB2OTGBase;

    ULONG               hu_XferSizeWidth;
    ULONG               hu_PktSizeWidth;

    ULONG               hu_PIDBits[128];        /* PID 2-bit pairs, one ULONG per device, each ULONG contains 2-bits for every endpoint */
};

/* PRIVATE device node */
struct USB2OTGDevice
{
    struct Library	hd_Library;	        /* standard */
    UWORD		hd_Flags;	        /* various flags */

    APTR		hd_KernelBase;		/* kernel.resource base */
    APTR                hd_UtilityBase;	        /* for tags etc */

    APTR		hd_MemPool;	        /* memory pool */

    struct USB2OTGUnit  *hd_Unit;	        /* we only currently support a single unit.. */

    struct MsgPort	*hd_MsgPort;
    struct timerequest	*hd_TimerReq;	        /* Timer I/O Requests */

};

#define FNAME_DEV(x)            USB2OTG__Dev__ ## x
#define FNAME_ROOTHUB(x)        USB2OTG__RootHub__ ## x

#ifdef UtilityBase
#undef UtilityBase
#endif

#ifdef KernelBase
#undef KernelBase
#endif

#define	UtilityBase     USB2OTGBase->hd_UtilityBase

#define KernelBase      USB2OTGBase->hd_KernelBase

struct Unit             *FNAME_DEV(OpenUnit)(struct IOUsbHWReq *, LONG, struct USB2OTGDevice *);
void                    FNAME_DEV(CloseUnit)(struct IOUsbHWReq *, struct USB2OTGUnit *, struct USB2OTGDevice *);

void                    FNAME_DEV(TermIO)(struct IOUsbHWReq *, struct USB2OTGDevice *);

WORD                    FNAME_DEV(cmdNSDeviceQuery)(struct IOStdReq *, struct USB2OTGUnit *, struct USB2OTGDevice *);
WORD                    FNAME_DEV(cmdQueryDevice)(struct IOUsbHWReq *, struct USB2OTGUnit *, struct USB2OTGDevice *);

WORD                    FNAME_DEV(cmdReset)(struct IOUsbHWReq *, struct USB2OTGUnit *, struct USB2OTGDevice *);
WORD                    FNAME_DEV(cmdFlush)(struct IOUsbHWReq *, struct USB2OTGUnit *, struct USB2OTGDevice *);

WORD                    FNAME_DEV(cmdUsbReset)(struct IOUsbHWReq *, struct USB2OTGUnit *, struct USB2OTGDevice *);
WORD                    FNAME_DEV(cmdUsbResume)(struct IOUsbHWReq *, struct USB2OTGUnit *, struct USB2OTGDevice *);
WORD                    FNAME_DEV(cmdUsbSuspend)(struct IOUsbHWReq *, struct USB2OTGUnit *, struct USB2OTGDevice *);
WORD                    FNAME_DEV(cmdUsbOper)(struct IOUsbHWReq *, struct USB2OTGUnit *, struct USB2OTGDevice *);
WORD                    FNAME_DEV(cmdControlXFer)(struct IOUsbHWReq *, struct USB2OTGUnit *, struct USB2OTGDevice *);
WORD                    FNAME_DEV(cmdBulkXFer)(struct IOUsbHWReq *, struct USB2OTGUnit *, struct USB2OTGDevice *);
WORD                    FNAME_DEV(cmdIntXFer)(struct IOUsbHWReq *, struct USB2OTGUnit *, struct USB2OTGDevice *);
WORD                    FNAME_DEV(cmdIsoXFer)(struct IOUsbHWReq *, struct USB2OTGUnit *, struct USB2OTGDevice *);

void                    FNAME_DEV(Cause)(struct USB2OTGDevice *, struct Interrupt *);

WORD                    FNAME_ROOTHUB(cmdControlXFer)(struct IOUsbHWReq *, struct USB2OTGUnit *, struct USB2OTGDevice *);
WORD                    FNAME_ROOTHUB(cmdIntXFer)(struct IOUsbHWReq *, struct USB2OTGUnit *, struct USB2OTGDevice *);
void                    FNAME_ROOTHUB(PendingIO)(struct USB2OTGUnit *);

void                    FNAME_DEV(GlobalIRQHandler)(struct USB2OTGUnit *USBUnit, struct ExecBase *SysBase);
void                    FNAME_DEV(ScheduleCtrlTDs)(struct USB2OTGUnit *);
void                    FNAME_DEV(ScheduleBulkTDs)(struct USB2OTGUnit *);
void                    FNAME_DEV(ScheduleIntTDs)(struct USB2OTGUnit *);
void                    FNAME_DEV(SetupChannel)(struct USB2OTGUnit *, int chan);
void                    FNAME_DEV(StartChannel)(struct USB2OTGUnit *, int chan, int quick);
int                     FNAME_DEV(AdvanceChannel)(struct USB2OTGUnit *, int chan);
void                    FNAME_DEV(FinalizeChannel)(struct USB2OTGUnit *, int chan);

#define CHAN_CTRL       0
#define CHAN_BULK       1
#define CHAN_INT1       2
#define CHAN_INT2       3
#define CHAN_INT3       4
#define CHAN_ISO1       5
#define CHAN_ISO2       6
#define CHAN_ISO3       7

#endif /* USB2OTG_INTERN_H */
