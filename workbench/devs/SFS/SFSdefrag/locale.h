#ifndef _LOCALE_H_
#define _LOCALE_H_

/*
    Copyright � 2003-2004, The AROS Development Team. All rights reserved.
    $Id: locale.h 21058 2004-02-23 19:14:08Z chodorowski $
*/

#include <exec/types.h>

#define CATCOMP_NUMBERS
#include "strings.h"

/*** Prototypes *************************************************************/
/* Main *********************************************************************/
CONST_STRPTR _(ULONG ID);       /* Get a message, as a STRPTR */
#define __(id) ((IPTR) _(id))   /* Get a message, as an IPTR */

/* Setup ********************************************************************/
VOID Locale_Initialize(VOID);
VOID Locale_Deinitialize(VOID);

#endif /* _LOCALE_H_ */
