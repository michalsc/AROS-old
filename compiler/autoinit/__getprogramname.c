/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc: autoinit library - support function to get the program's name
    Lang: english
*/

#include <proto/exec.h>
#include <proto/dos.h>

#include <exec/memory.h>
#include <dos/dos.h>

char *__getprogramname(void)
{
    char *name;
    LONG  namlen = 64;
    int   done   = 0;

    do
    {
	if (!(name = AllocVec(namlen, MEMF_ANY)))
	{
	    SetIoErr(ERROR_NO_FREE_STORE);
	    return NULL;
	}

	if (!(GetProgramName(name, namlen)))
	{
	    if (IoErr() == ERROR_LINE_TOO_LONG)
	    {
		namlen *= 2;
		FreeVec(name);
	    }
	    else
		return NULL;
	}
	else
	    done = 1;
    } while (!done);

    return name;
}
