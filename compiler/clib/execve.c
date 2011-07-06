/*
    Copyright � 2008-2009, The AROS Development Team. All rights reserved.
    $Id$

    POSIX function execve().
*/

#define DEBUG 0

#include <exec/types.h>
#include <exec/lists.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <aros/debug.h>

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <__errno.h>
#include "__exec.h"
#include "__upath.h"
#include "__fdesc.h"
#include "__arosc_privdata.h"
#include "__vfork.h"

/*****************************************************************************

    NAME */
#include <unistd.h>

	int execve(

/*  SYNOPSIS */
	const char *filename,
	char *const argv[],
	char *const envp[])
        
/*  FUNCTION
	Executes a file with given name.

    INPUTS
	filename - Name of the file to execute.
	argv - Array of arguments provided to main() function of the executed
	file.
	envp - Array of environment variables passed as environment to the
	executed program.

    RESULT
	Returns -1 and sets errno appropriately in case of error, otherwise
	doesn't return.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
	
    INTERNALS

******************************************************************************/
{
    APTR id = __exec_prepare(filename, 0, argv, envp);
    if(!id)
        return -1;
    
    __exec_do(id);
    
    assert(0); /* Should not be reached */
    return -1;
} /* execve() */
