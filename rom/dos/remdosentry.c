/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$
    $Log$
    Revision 1.4  1996/10/24 15:50:35  aros
    Use the official AROS macros over the __AROS versions.

    Revision 1.3  1996/08/13 13:52:50  digulla
    Replaced <dos/dosextens.h> by "dos_intern.h" or added "dos_intern.h"
    Replaced AROS_LA by AROS_LHA

    Revision 1.2  1996/08/01 17:40:57  digulla
    Added standard header for all files

    Desc:
    Lang: english
*/
#include <dos/dosextens.h>
#include <clib/utility_protos.h>

/*****************************************************************************

    NAME */
	#include <clib/dos_protos.h>

	AROS_LH1(LONG, RemDosEntry,

/*  SYNOPSIS */
	AROS_LHA(struct DosList *, dlist, D1),

/*  LOCATION */
	struct DosLibrary *, DOSBase, 112, Dos)

/*  FUNCTION
	Removes a given dos list entry from the dos list. Automatically
	locks the list for writing.

    INPUTS
	dlist - pointer to dos list entry.

    RESULT
	!=0 if all went well, 0 otherwise.

    NOTES
	Since anybody who wants to use a device or volume node in the
	dos list has to lock the list, filesystems may be called with
	the dos list locked. So if you want to add a dos list entry
	out of a filesystem don't just wait on the lock but serve all
	incoming requests until the dos list is free instead.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	29-10-95    digulla automatically created from
			    dos_lib.fd and clib/dos_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct DosLibrary *,DOSBase)
    struct DosList *dl;

    dl=LockDosList(LDF_ALL|LDF_WRITE);
    for(;;)
    {
        if(dl->dol_Next==dlist)
	{
	    dl->dol_Next=dlist->dol_Next;
	    break;
	}
	dl=dl->dol_Next;
    }
    UnLockDosList(LDF_ALL|LDF_WRITE);

    return 1;
    AROS_LIBFUNC_EXIT
} /* RemDosEntry */
