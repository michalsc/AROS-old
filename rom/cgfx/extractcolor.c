/*
    (C) 1997 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: english
*/
#include "cybergraphics_intern.h"

/*****************************************************************************

    NAME */
#include <clib/cybergraphics_protos.h>

	AROS_LH7(ULONG, ExtractColor,

/*  SYNOPSIS */
	AROS_LHA(struct RastPort *, , A0),
	AROS_LHA(struct BitMap   *, , A1),
	AROS_LHA(ULONG            , , D0),
	AROS_LHA(ULONG            , , D1),
	AROS_LHA(ULONG            , , D2),
	AROS_LHA(ULONG            , , D3),
	AROS_LHA(ULONG            , , D4),

/*  LOCATION */
	struct Library *, CyberGfxBase, 31, Cybergraphics)

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
			    cybergraphics_lib.fd and clib/cybergraphics_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,CyberGfxBase)
    extern void aros_print_not_implemented (char *);

    aros_print_not_implemented ("ExtractColor");

    AROS_LIBFUNC_EXIT
} /* ExtractColor */
