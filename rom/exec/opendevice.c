/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$

    Desc:
    Lang: english
*/
#include <aros/config.h>
#include <exec/execbase.h>
#include <exec/devices.h>
#include <exec/io.h>
#include <exec/errors.h>
#include <aros/libcall.h>
#include <exec/libraries.h>
#include <proto/exec.h>

#ifndef DEBUG_SetFunction
#   define DEBUG_SetFunction 0
#endif
#if DEBUG_SetFunction
#   undef DEBUG
#   define DEBUG 1
#endif
#include <aros/debug.h>
#undef kprintf

/*****************************************************************************

    NAME */

	AROS_LH4(BYTE, OpenDevice,

/*  SYNOPSIS */
	AROS_LHA(STRPTR,             devName,    A0),
	AROS_LHA(ULONG,              unitNumber, D0),
	AROS_LHA(struct IORequest *, iORequest,  A1),
	AROS_LHA(ULONG,              flags,      D1),

/*  LOCATION */
	struct ExecBase *, SysBase, 74, Exec)

/*  FUNCTION
	Tries to open a device and fill the iORequest structure.
	An error is returned if this fails, 0 if all went well.

    INPUTS
	devName    - Pointer to the devices's name.
	unitNumber - The unit number. Most often 0.
	iORequest  - Pointer do device specific information.
		     Will be filled out by the device.
		     Must lie in public (or at least shared) memory.
	flags	   - Some flags to give to the device.

    RESULT
	Error code or 0 if all went well. The same value can be found
	in the io_Error field.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
	CloseDevice()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    AROS_LIBBASE_EXT_DECL(struct ExecBase *,SysBase)
    struct Device *device;
    BYTE ret=IOERR_OPENFAIL;

    D(bug("OpenDevice $%lx $%lx $%lx %ld (\"%s\") by \"%s\"\n", devName, unitNumber, iORequest,
	flags, (devName > (STRPTR)0x400) ? devName : (UBYTE *)"(null)", SysBase->ThisTask->tc_Node.ln_Name));

    /* Arbitrate for the device list */
    Forbid();

    /* Look for the device in our list */
    device=(struct Device *)FindName(&SysBase->DeviceList,devName);

    /* Something found ? */
    if(device!=NULL)
    {
	/* Init iorequest */
	iORequest->io_Error=0;
	iORequest->io_Device=device;
	iORequest->io_Unit = NULL;

	/* Call Open vector. */
	AROS_LVO_CALL3NR(
	    AROS_LCA(struct IORequest *,iORequest,A1),
	    AROS_LCA(ULONG,unitNumber,D0),
	    AROS_LCA(ULONG,flags,D1),
	    struct Device, device, 1, dev
	);

	/* Check for error */
	ret=iORequest->io_Error;
	if(ret)
	    /* Mark request as non-open */
	    iORequest->io_Device=NULL;
    }
    /*
	else
	{
	Under normal circumstances you'd expect the device loading here -
	but this is only exec which doesn't know anything about the
	filesystem level. Therefore dos.library has to SetFunction() this vector
	for the additional functionality.
	}
    */

    /* All done. */
    Permit();
    return ret;
    AROS_LIBFUNC_EXIT
} /* OpenDevice */

