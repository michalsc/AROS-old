/*
    (C) 2000 AROS - The Amiga Research OS
    $Id$

    Desc: AmigaGuide function LoadXRef()
    Lang: english
*/
#include "amigaguide_intern.h"

/*****************************************************************************

    NAME */
#include <proto/amigaguide.h>

        AROS_LH2(LONG, LoadXRef,

/*  SYNOPSIS */
        AROS_LHA(BPTR, lock, A0),
        AROS_LHA(STRPTR, name, A1),

/*  LOCATION */
        struct Library *, AmigaGuideBase, 21, AmigaGuide)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY


******************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *, AmigaGuideBase)

#warning TODO: Write amigaguide/LoadXRef()
    aros_print_not_implemented ("amigaguide/LoadXRef");

    return 0;

    AROS_LIBFUNC_EXIT
} /* LoadXRef */
