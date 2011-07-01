/*
    Copyright � 1995-2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/dos.h>
#include <proto/exec.h>
#include <exec/exec.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h> /* For snprintf */

#include "__errno.h"
#include "__fdesc.h"

/*****************************************************************************

    NAME */

	int pipe(

/*  SYNOPSIS */
	int *pipedes)

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
    BPTR reader, writer;
    fcb *rfcb = NULL, *wfcb = NULL;
    fdesc *rdesc = NULL, *wdesc = NULL;
    /* PIPE:cpipe-%08x-%d, where %x is the getpid(), %d is the nth pipe */
    char pipe_name[5 + 6 + 8 + 1 + 16 + 1];
    static int pipeno = 0;

    if (!pipedes)
    {
	errno = EFAULT;

	return -1;
    }

    if (
	(rfcb = AllocVec(sizeof(fcb), MEMF_ANY | MEMF_CLEAR)) == NULL ||
	(rdesc = __alloc_fdesc()) == NULL ||
	(wfcb = AllocVec(sizeof(fcb), MEMF_ANY | MEMF_CLEAR)) == NULL ||
	(wdesc = __alloc_fdesc()) == NULL
    )
    {
	if(rfcb)
	    FreeVec(rfcb);
	if(rdesc)
	    __free_fdesc(rdesc);
	if(wfcb)
	    FreeVec(wfcb);
	if(wdesc)
	    __free_fdesc(wdesc);
	errno = ENOMEM;
	return -1;
    }

    /* Get the next pipe number */
    Forbid();
    pipeno++;
    Permit();

    snprintf(pipe_name, sizeof(pipe_name), "PIPE:cpipe-%08x-%d",
               (unsigned long)getpid(), pipeno);
    pipe_name[sizeof(pipe_name)-1] = 0;

    writer = Open(pipe_name, MODE_NEWFILE);
    if (writer)
    {
    	reader = Open(pipe_name, MODE_OLDFILE);
	if (!reader)
	{
    	    DeleteFile(pipe_name);
    	    Close(writer);
    	    writer = BNULL;
	}
    }

    if (!writer)
    {
        errno = IoErr2errno(IoErr());
        __free_fdesc(rdesc);
        __free_fdesc(wdesc);
        return -1;
    }

    pipedes[0] = __getfdslot(__getfirstfd(0));
    rdesc->fdflags = 0;
    rdesc->fcb = rfcb;
    rdesc->fcb->fh        = reader;
    rdesc->fcb->flags     = O_RDONLY;
    rdesc->fcb->opencount = 1;
    __setfdesc(pipedes[0], rdesc);

    pipedes[1] = __getfdslot(__getfirstfd(pipedes[0]));
    wdesc->fdflags = 0;
    wdesc->fcb = wfcb;
    wdesc->fcb->fh        = writer;
    wdesc->fcb->flags     = O_WRONLY;
    wdesc->fcb->opencount = 1;
    __setfdesc(pipedes[1], wdesc);

    return 0;
}

