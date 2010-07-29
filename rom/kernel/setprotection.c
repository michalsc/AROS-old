#include <aros/libcall.h>

#include "kernel_base.h"

/*****************************************************************************

    NAME */
#include <proto/kernel.h>

AROS_LH3I(void, KrnSetProtection,

/*  SYNOPSIS */
	AROS_LHA(void *, address, A0),
	AROS_LHA(uint32_t, length, D0),
        AROS_LHA(KRN_MapAttr, flags, D1),

/*  LOCATION */
	struct KernelBase *, KernelBase, 21, Kernel)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    AROS_LIBFUNC_EXIT
}
