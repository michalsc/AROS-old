/*
    (C) 1995-99 AROS - The Amiga Research OS
    $Id$

    Desc: Intuition function GetDefPrefs()
    Lang: english
*/
#include "intuition_intern.h"

/*****************************************************************************

    NAME */
#include <proto/intuition.h>

	AROS_LH2(struct Preferences *, GetDefPrefs,

/*  SYNOPSIS */
	AROS_LHA(struct Preferences * , prefbuffer, A0),
	AROS_LHA(WORD                 , size, D0),

/*  LOCATION */
	struct IntuitionBase *, IntuitionBase, 21, Intuition)

/*  FUNCTION
	Gets a copy of the Intuition default Preferences structure.

    INPUTS
	prefbuffer - The buffer which contains your settings for the
		preferences.
	size - The number of bytes of the buffer you want to be copied.

    RESULT
	Returns your parameter buffer.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
	GetPrefs(), SetPrefs()

    INTERNALS

    HISTORY

*****************************************************************************/
{
  AROS_LIBFUNC_INIT
  AROS_LIBBASE_EXT_DECL(struct IntuitionBase *,IntuitionBase)

  if (NULL != prefbuffer && 0 != size)
    memcpy(prefbuffer,
           GetPrivIBase(IntuitionBase)->DefaultPreferences, 
           size);

  return (struct Preferences *)prefbuffer;

  AROS_LIBFUNC_EXIT
} /* GetDefPrefs */
