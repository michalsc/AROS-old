/*
    (C) 2000 AROS - The Amiga Research OS
    $Id$

    Desc: AmigaGuide function SetAmigaGuideAttrsA()
    Lang: english
*/
#include "amigaguide_intern.h"

/*****************************************************************************

    NAME */
#include <proto/amigaguide.h>

        AROS_LH2(LONG, SetAmigaGuideAttrsA,

/*  SYNOPSIS */
        AROS_LHA(AMIGAGUIDECONTEXT, handle, A0),
        AROS_LHA(struct TagItem *, attrs, A1),

/*  LOCATION */
        struct Library *, AmigaGuideBase, 18, AmigaGuide)

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

#warning TODO: Write amigaguide/SetAmigaGuideAttrsA()
    aros_print_not_implemented ("amigaguide/SetAmigaGuideAttrsA");

    return 0;

    AROS_LIBFUNC_EXIT
} /* SetAmigaGuideAttrsA */
