#include <aros/kernel.h>
#include <aros/libcall.h>

#include <kernel_base.h>

/*****************************************************************************

    NAME */
#include <proto/kernel.h>

AROS_LH0I(void, KrnReleaseInput,

/*  SYNOPSIS */

/*  LOCATION */
	struct KernelBase *, KernelBase, 34, Kernel)

/*  FUNCTION
	Release low-level debug input hardware and hand it back to the operating system

    INPUTS
	None

    RESULT
	None

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    /* Nothing to do */

    AROS_LIBFUNC_EXIT
}
