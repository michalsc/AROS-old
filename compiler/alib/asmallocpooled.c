/*
    Copyright (C) 1997-2001 AROS - The Amiga Research OS
    $Id$

    Desc: Simply call the exec function 
    Lang: english
*/
#define AROS_TAGRETURNTYPE APTR

#include "alib_intern.h"

/*****************************************************************************

    NAME */
#define NO_INLINE_STDARG /* turn off inline def */
#include <proto/exec.h>

	AROS_UFH3(APTR, AsmAllocPooled,

/*  SYNOPSIS */
	AROS_UFHA(APTR, poolHeader, A0),
	AROS_UFHA(ULONG, memSize  , D0),
	AROS_UFHA(struct ExecBase *, SysBase, A6)) 

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    return AllocPooled(poolHeader,memSize);	
    
} /* AsmAllocPooled */
