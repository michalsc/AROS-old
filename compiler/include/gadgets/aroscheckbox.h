#ifndef GADGETS_AROSCHECKBOX_H
#define GADGETS_AROSCHECKBOX_H

/*
    (C) 1997 AROS - The Amiga Replacement OS
    $Id$

    Desc: MethodIDs and AttrIDs for the AROS checkbox class.
    Lang: english
*/
#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif
#ifndef UTILITY_TAGITEM_H
#   include <utility/tagitem.h>
#endif
#ifndef LIBRARIES_GADTOOLS_H
#   include <libraries/gadtools.h>
#endif


/* Use that #define instead of a string. */
#define AROSCHECKBOXCLASS "checkbox.aros"


/* Tags to be passed to AROSCHECKBOXCLASS. */
#define AROSCB_Checked GTCB_Checked
  /* [ISG] (BOOL) Set/Get the state of the checkmark. */

#endif /* GADGETS_AROSCHECKBOX_H */

