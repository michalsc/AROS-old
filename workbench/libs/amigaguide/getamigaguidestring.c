/*
    (C) 2000 AROS - The Amiga Research OS
    $Id$

    Desc: AmigaGuide function GetAmigaGuideString()
    Lang: english
*/
#include "amigaguide_intern.h"

/*****************************************************************************

    NAME */
#include <proto/amigaguide.h>

        AROS_LH1(STRPTR, GetAmigaGuideString,

/*  SYNOPSIS */
        AROS_LHA(ULONG, id, D0),

/*  LOCATION */
        struct Library *, AmigaGuideBase, 35, AmigaGuide)

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

#warning TODO: Write amigaguide/GetAmigaGuideString()
    aros_print_not_implemented ("amigaguide/GetAmigaGuideString");

    return NULL;

    AROS_LIBFUNC_EXIT
} /* GetAmigaGuideString */
