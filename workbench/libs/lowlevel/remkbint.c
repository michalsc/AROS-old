/*
    (C) 1995-2000 AROS - The Amiga Research OS
    $Id$

    Desc: 
    Lang: english
*/
#include "lowlevel_intern.h"

#include <aros/libcall.h>
#include <exec/types.h>
#include <libraries/lowlevel.h>

/*****************************************************************************

    NAME */

      AROS_LH1(VOID, RemKBInt,

/*  SYNOPSIS */ 
      AROS_LHA(APTR, intHandle, A1),

/*  LOCATION */
      struct LowLevelBase *, LowLevelBase, 16, LowLevel)

/*  NAME
 
    FUNCTION
 
    INPUTS
 
    RESULT
 
    BUGS

    INTERNALS

    HISTORY

*****************************************************************************/
{
  AROS_LIBFUNC_INIT
  AROS_LIBBASE_EXT_DECL(struct LowLevelBase *, LowLevelBase)

#warning TODO: Write lowlevel/RemKBInt()
    aros_print_not_implemented ("lowlevel/RemKBInt");

    if (intHandle)
    {
    }

    return;

  AROS_LIBFUNC_EXIT
} /* RemKBInt */
