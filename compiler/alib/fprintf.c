/*
    Copyright (C) 1995-1998 AROS - The Amiga Replacement OS
    $Id$

    Desc: Varargs stub for VFPrintf()
    Lang: english
*/

#include <dos/dos.h>
#include <proto/dos.h>
#include <stdarg.h>

void FPrintf(BPTR fh, STRPTR fmt, LONG arg, ...)
{
    va_list args;
    va_start(args,arg);

    VFPrintf(fh, fmt, (LONG *)args);

    va_end(args);
}
