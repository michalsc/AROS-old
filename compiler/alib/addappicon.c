/*
    (C) 2000 AROS - The Amiga Research OS
    $Id$

    Desc: Varargs version of workbench.library/AddAppIconA()
    Lang: english
*/
#define AROS_TAGRETURNTYPE struct AppIcon *
#include <dos/bptr.h>
#include <utility/tagitem.h>

extern struct WorkbenchBase * WorkbenchBase;

/*****************************************************************************

    NAME */
#define NO_INLINE_STDARG /* turn off inline def */
#include <proto/workbench.h>

	struct AppIcon * AddAppIcon (

/*  SYNOPSIS */
	ULONG id,
	ULONG userdata,
	STRPTR text,
	struct MsgPort * msgport,
	BPTR lock,
	struct DiskObject * diskobj,
	Tag tag1,
	...)

/*  FUNCTION
        This is the varargs version of workbench.library/AddAppIconA().
        For information see workbench.library/AddAppIconA().

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        workbench/AddAppIconA()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_SLOWSTACKTAGS_PRE(tag1)
    AddAppIconA (id, userdata, text, msgport, lock, diskobj, AROS_SLOWSTACKTAGS_ARG(tag1));
    AROS_SLOWSTACKTAGS_POST
} /* AddAppIcon */
