/*
    (C) 1995-96 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: english
*/
#include "graphics_intern.h"
#include <clib/graphics_protos.h>

#if DEBUG
#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;
#endif

/*****************************************************************************

    NAME */

	AROS_LH2(void, LoadRGB32,

/*  SYNOPSIS */
	AROS_LHA(struct ViewPort *, vp, A0),
	AROS_LHA(const ULONG     *, table, A1),

/*  LOCATION */
	struct GfxBase *, GfxBase, 147, Graphics)

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
			    graphics_lib.fd and clib/graphics_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct GfxBase *,GfxBase)
    ASSERT_VALID_PTR(vp);
    ASSERT_VALID_PTR(table);

    driver_LoadRGB32 (vp, table, GfxBase);

    while (*table)
    {
        ULONG count, first, t;
	
	count = (*table) >> 16;
	first = (*table) & 0xFFFF;

	table ++;

	for (t = 0; t < count; t++)
	{
	    SetRGB32CM (vp->ColorMap,
	    		t + first,
			table[0],
			table[1],
			table[2]);

	    table += 3;
	}

    } /* while (*table) */

    AROS_LIBFUNC_EXIT
    
} /* LoadRGB32 */
