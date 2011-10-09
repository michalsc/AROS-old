/*
    Copyright � 2011, The AROS Development Team. All rights reserved.
    $Id: card_init.c $

    Desc: dummy file
    Lang: English
*/

#include "card_intern.h"

AROS_LH1(struct CardHandle*, OwnCard,
	 AROS_LHA(struct CardHandle*, handle, A1),
	 struct CardResource*, CardResource, 1, Cardres)
{
    AROS_LIBFUNC_INIT

    return NULL;

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, ReleaseCard,
	 AROS_LHA(struct CardHandle*, handle, A1),
	 AROS_LHA(ULONG, flags, D0),
	 struct CardResource*, CardResource, 2, Cardres)
{
    AROS_LIBFUNC_INIT

    AROS_LIBFUNC_EXIT
}

AROS_LH0(struct CardMemoryMap*, GetCardMap,
	struct CardResource*, CardResource, 3, Cardres)
{
    AROS_LIBFUNC_INIT

    return &CardResource->cmm;

    AROS_LIBFUNC_EXIT
}

AROS_LH1(BOOL, BeginCardAccess,
	AROS_LHA(struct CardHandle*, handle, A1),
	struct CardResource*, CardResource, 4, Cardres)
{
    AROS_LIBFUNC_INIT

    return FALSE;

    AROS_LIBFUNC_EXIT
}

AROS_LH1(BOOL, EndCardAccess,
	AROS_LHA(struct CardHandle*, handle, A1),
	struct CardResource*, CardResource, 5, Cardres)
{
    AROS_LIBFUNC_INIT

    return FALSE;

    AROS_LIBFUNC_EXIT
}

AROS_LH0(UBYTE, ReadCardStatus,
	 struct CardResource*, CardResource, 6, Cardres)
{
    AROS_LIBFUNC_INIT

    return 0;

    AROS_LIBFUNC_EXIT
}

AROS_LH2(BOOL, CardResetRemove,
	AROS_LHA(struct CardHandle*, handle, A1),
	AROS_LHA(ULONG, flag, D0),
	struct CardResource*, CardResource, 7, Cardres)
{
    AROS_LIBFUNC_INIT

    return FALSE;

    AROS_LIBFUNC_EXIT
}

AROS_LH2(UBYTE, CardMiscControl,
	AROS_LHA(struct CardHandle*, handle, A1),
	AROS_LHA(UBYTE, control_bits, D0),
	struct CardResource*, CardResource, 8, Cardres)
{
    AROS_LIBFUNC_INIT

    return 0;

    AROS_LIBFUNC_EXIT
}

AROS_LH2(ULONG, CardAccessSpeed,
	AROS_LHA(struct CardHandle*, handle, A1),
	AROS_LHA(ULONG, nanoseconds, D0),
	struct CardResource*, CardResource, 9, Cardres)
{
    AROS_LIBFUNC_INIT

    return 0;

    AROS_LIBFUNC_EXIT
}

AROS_LH2(LONG, CardProgramVoltage,
	AROS_LHA(struct CardHandle*, handle, A1),
	AROS_LHA(ULONG, voltage, D0),
	struct CardResource*, CardResource, 10, Cardres)
{
    AROS_LIBFUNC_INIT

    return -1;

    AROS_LIBFUNC_EXIT
}

AROS_LH1(BOOL, CardResetCard,
	AROS_LHA(struct CardHandle*, handle, A1),
	struct CardResource*, CardResource, 11, Cardres)
{
    AROS_LIBFUNC_INIT

    return FALSE;

    AROS_LIBFUNC_EXIT
}

AROS_LH4(BOOL, CopyTuple,
	AROS_LHA(struct CardHandle*, handle, A1),
	AROS_LHA(UBYTE*, buffer, A0),
	AROS_LHA(ULONG, tuplecode, D1),
	AROS_LHA(ULONG, size, D0),
	struct CardResource*, CardResource, 12, Cardres)
{
    AROS_LIBFUNC_INIT

    return FALSE;

    AROS_LIBFUNC_EXIT
}

AROS_LH2(BOOL, DeviceTuple,
	AROS_LHA(UBYTE*, tuple_data, A0),
	AROS_LHA(struct DeviceTData*, storage, A1),
	struct CardResource*, CardResource, 13, Cardres)
{
    AROS_LIBFUNC_INIT

    return FALSE;

    AROS_LIBFUNC_EXIT
}

AROS_LH1(struct Resident*, IfAmigaXIP,
	AROS_LHA(struct CardHandle*, handle, A2),
	struct CardResource*, CardResource, 14, Cardres)
{
    AROS_LIBFUNC_INIT

    return NULL;

    AROS_LIBFUNC_EXIT
}

AROS_LH0(ULONG, CardChangeCount,
	struct CardResource*, CardResource, 16, Cardres)
{
    AROS_LIBFUNC_INIT

    return 0;

    AROS_LIBFUNC_EXIT
}

AROS_LH0(BOOL, CardForceChange,
	struct CardResource*, CardResource, 15, Cardres)
{
    AROS_LIBFUNC_INIT

    return FALSE;

    AROS_LIBFUNC_EXIT
}

AROS_LH0(ULONG, CardInterface,
	struct CardResource*, CardResource, 17, Cardres)
{
    AROS_LIBFUNC_INIT

    return -1;

    AROS_LIBFUNC_EXIT
}

