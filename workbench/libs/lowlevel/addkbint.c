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

      AROS_LH2(APTR, AddKBInt,

/*  SYNOPSIS */ 
      AROS_LHA(APTR, intRoutine, A0),
      AROS_LHA(APTR, intData, A1),

/*  LOCATION */
      struct LowLevelBase *, LowLevelBase, 15, LowLevel)

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

#warning TODO: Write lowlevel/AddKBInt()
    aros_print_not_implemented ("lowlevel/AddKBInt");

    return NULL; // return failure until implemented

  AROS_LIBFUNC_EXIT
} /* AddKBInt */
