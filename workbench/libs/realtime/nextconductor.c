
/*
    (C) 1999-2001 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: English
*/

/*****************************************************************************

    NAME */

#include <proto/exec.h>
#include "realtime_intern.h"

    AROS_LH1(struct Conductor *, NextConductor,

/*  SYNOPSIS */

	AROS_LHA(struct Conductor *, previousConductor, A0),

/*  LOCATION */

	struct Library *, RTBase, 12, RealTime)

/*  FUNCTION

    Return the next conductor on the conductor list. If 'previousConductor'
    is NULL, return the first conductor in the list; if not, return the
    conductor following 'previousConductor'. If 'previousConductor' is the
    last conductor, this function returns NULL.

    INPUTS

    previousConductor  --  The previous conductor or NULL to get the first
                           conductor.

    RESULT

    A pointer to the next conductor or NULL if there are no more conductors.

    NOTES

    You have to lock the conductors with LockRealTime(RT_CONDUCTORS)
    before calling this function.

    EXAMPLE

    BUGS

    SEE ALSO

    FindConductor(), LockRealTime(), UnlockRealTime()

    INTERNALS

    HISTORY

    26.7.99  SDuvan  implemented

******************************************************************************/

{
    AROS_LIBFUNC_INIT

    if (previousConductor == NULL)
    {
	return (struct Conductor *)GetHead((struct List *)&GPB(RTBase)->rtb_ConductorList);
    }

    return (struct Conductor *)GetSucc((struct Node *)previousConductor);

    AROS_LIBFUNC_EXIT
} /* NextConductor */
