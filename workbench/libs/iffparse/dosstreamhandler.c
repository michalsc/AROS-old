/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$

    Desc: DOS stream handler. Used in InitIFFasDOS.
    Lang: English.
*/
#define DEBUG 0
#include "iffparse_intern.h"

/********************/
/* DosStreamHandler */
/********************/

#undef DOSBase
#define DOSBase    (IPB(hook->h_Data)->dosbase)

ULONG DOSStreamHandler
(
    struct Hook 	* hook,
    struct IFFHandle	* iff,
    struct IFFStreamCmd * cmd
)
{

    LONG error = 0;

    switch (cmd->sc_Command)
    {
    case IFFCMD_READ:
#if DEBUG
	VPrintf ("   Reading %ld bytes\n", &cmd->sc_NBytes);
	Flush (Output ());
#endif

	error =
	(
	    Read
	    (
		(BPTR)iff->iff_Stream,
		cmd->sc_Buf,
		cmd->sc_NBytes
	    )
	!=
	    cmd->sc_NBytes
	);

	break;

    case IFFCMD_WRITE:

	error =
	(
	    Write
	    (
		(BPTR)iff->iff_Stream,
		cmd->sc_Buf,
		cmd->sc_NBytes
	    )
	!=
	    cmd->sc_NBytes
	);

	break;

    case IFFCMD_SEEK:
#if DEBUG
	VPrintf ("   Seeking %ld bytes\n", &cmd->sc_NBytes);
	Flush (Output ());
#endif

	error =
	(
	    Seek
	    (
		(BPTR)iff->iff_Stream,
		cmd->sc_NBytes,
		OFFSET_CURRENT
	    )

	==
	    -1
	);

	break;

    case IFFCMD_INIT:
    case IFFCMD_CLEANUP:
	/* Don't need these for dos streams
	*/
	error = NULL;
	break;
    }

    return (error);

}
