/*
    (C) 1995-96 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: english
*/
#include "dos_intern.h"

/*****************************************************************************

    NAME */
#include <dos/stdio.h>
#include <proto/dos.h>

	AROS_LH4(LONG, SetVBuf,

/*  SYNOPSIS */
	AROS_LHA(BPTR  , file, D1),
	AROS_LHA(STRPTR, buff, D2),
	AROS_LHA(LONG  , type, D3),
	AROS_LHA(LONG  , size, D4),

/*  LOCATION */
	struct DosLibrary *, DOSBase, 61, Dos)

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
			    dos_lib.fd and clib/dos_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct DosLibrary *,DOSBase)

    struct FileHandle *fh = (struct FileHandle *)BADDR(file);

    switch (type)
    {
        case BUF_LINE: fh->fh_Flags = (fh->fh_Flags & ~FHF_NOBUF) | FHF_LINEBUF; break;
        case BUF_FULL: fh->fh_Flags = fh->fh_Flags & ~(FHF_NOBUF | FHF_LINEBUF); break;
        case BUF_NONE: fh->fh_Flags = (fh->fh_Flags | FHF_NOBUF) & ~FHF_LINEBUF; break;
	default:
	    return EOF;
    }

    if (size != -1)
    {
        if (size < 208) size = 208;

        if (buff)
        {
	    if (fh->fh_Flags & FHF_BUF)
 	        FreeMem(fh->fh_Buf, fh->fh_Size);
	    fh->fh_Flags &= ~FHF_BUF;
	}
	else
	{
	    buff = AllocMem(size, MEMF_ANY);
	    if (!buff)
	    {
	        SetIoErr(ERROR_NO_FREE_STORE);
		return EOF;
	    }
	    fh->fh_Flags |= FHF_BUF;
	}

	fh->fh_Buf  = buff;
	fh->fh_Size = size;
    }

    return 0;
    AROS_LIBFUNC_EXIT
} /* SetVBuf */
