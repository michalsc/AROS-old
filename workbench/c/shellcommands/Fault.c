/*
    Copyright (C) 1995-2000 AROS - The Amiga Research OS
    $Id$

    Desc: Fault - Display an informative message about an error number.
    Lang: english
*/

/**************************************************************************

    NAME
	Fault

    FORMAT
	Fault <error number>

    SYNOPSIS
	NUMBERS/N/M

    LOCATION
	INTERNAL

    FUNCTION
	Fault prints the message corresponding with the error number
	supplied. Any number of error numbers can be given at once,
	but they must be separated by spaces.

    EXAMPLE

	1.SYS:> Fault 205
	Fault 205: object not found

	    This tells you that the error code 205 means that a disk
	    object could not be found.

**************************************************************************/

#include <exec/types.h>
#include <dos/dos.h>
#include <dos/rdargs.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <stdlib.h>
#include <stdio.h> /* for sprintf() */
#include <string.h>
#include <utility/tagitem.h>

#include "shcommands.h"

AROS_SH1H(Fault, 41.1,                   "Display the meaning of a DOS error code",
AROS_SHAH(ULONG **, ,NUMBERS,/N/M, NULL, "The error numbers you wish to query"))
{
    AROS_SHCOMMAND_INIT

    UBYTE buffer[128];
    BPTR outStream = Output();
    ULONG **theNum = SHArg(NUMBERS), n;

    if (theNum)
    while( *theNum != NULL )
    {
	PutStr("Fault ");
	VPrintf("%ld", *theNum);
	PrintFault(**theNum, "");
	theNum++;
    }

    SetIoErr(0);

    return RETURN_OK;

    AROS_SHCOMMAND_EXIT
}
