/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: english
*/
#include "iffparse_intern.h"

/*****************************************************************************

    NAME */
#include <proto/iffparse.h>

	AROS_LH3(LONG, StopChunk,

/*  SYNOPSIS */
	AROS_LHA(struct IFFHandle *, iff, A0),
	AROS_LHA(LONG              , type, D0),
	AROS_LHA(LONG              , id, D1),

/*  LOCATION */
	struct Library *, IFFParseBase, 21, IFFParse)

/*  FUNCTION
	Inserts an entry handler for the given type and id, that will cause the parser
	to stop when such a chunk is entered.

    INPUTS
	 iff   - Pointer to IFFHandle struct. (does not need to be open).
	type  - IFF chunk type declarator for chunk to stop at.
	id    -  IFF chunk id identifier for chunk to stop at.

    RESULT
	error  -  0 if successfull, IFFERR_#? otherwise.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
	StopChunks(), ParseIFF()

    INTERNALS

    HISTORY
  27-11-96    digulla automatically created from
	  iffparse_lib.fd and clib/iffparse_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,IFFParseBase)

    DEBUG_STOPCHUNK(dprintf("StopChunk: iff 0x%lx type 0x%08lx (%.4s) id 0x%08lx (%.4s)\n",
			    iff, type, &type, id, &id));

    /* Install an EntryHandler */
    return
    (
	EntryHandler
	(
	    iff,
	    type,
	    id,
	    IFFSLI_TOP,
	    &(IPB(IFFParseBase)->stophook),
	    iff
	)
    );

    AROS_LIBFUNC_EXIT
} /* StopChunk */
