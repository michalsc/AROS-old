#ifndef INTUITION_POINTERCLASS_H
#define INTUITION_POINTERCLASS_H

/*
    (C) 1995-98 AROS - The Amiga Research OS
    $Id$

    Desc: Headerfile for Intuitions' pointer classes.
    Lang: english
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

#define POINTERA_Dummy	    (TAG_USER + 0x39000)

#define POINTERA_BitMap		(POINTERA_Dummy + 0x01)
#define POINTERA_WordWidth	(POINTERA_Dummy + 0x04)

#endif
