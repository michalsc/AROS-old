#ifndef DEVICES_TIMER_H
#define DEVICES_TIMER_H

/*
    (C) 1997 AROS - The Amiga Replacement OS
    $Id$

    Desc: Timer device
    Lang: english
*/

#ifndef EXEC_IO_H
#   include <exec/io.h>
#endif

#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif

#define TIMERNAME "timer.device"

/* Units */
#define UNIT_MICROHZ    0
#define UNIT_VBLANK     1
#define UNIT_ECLOCK     2
#define UNIT_WAITUNTIL  3
#define UNIT_WAITECLOCK 4

/* IO-Commands */
#define TR_ADDREQUEST (CMD_NONSTD+0)
#define TR_GETSYSTIME (CMD_NONSTD+1)
#define TR_SETSYSTIME (CMD_NONSTD+2)

struct timeval
{
    ULONG tv_secs;
    ULONG tv_micro;
};

struct EClockVal
{
    ULONG ev_hi;
    ULONG ev_lo;
};

struct timerequest
{
    struct IORequest tr_node;
    struct timeval   tr_time;
};

#endif /* DEVICES_TIMER_H */
