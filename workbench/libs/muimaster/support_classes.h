/*
    Copyright � 2002-2003, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#ifndef _MUIMASTER_SUPPORT_CLASSES_H
#define _MUIMASTER_SUPPORT_CLASSES_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef INTUITION_CLASSUSR_H
#include <intuition/classusr.h>
#endif

#ifndef CLIB_MACROS_H
#include <clib/macros.h>
#endif

#include "support.h"

#if ZUNE_BUILTIN_ABOUTMUI
#   define ZUNE_ABOUTMUI_DESC (&_MUI_Aboutmui_desc),
#else
#   define ZUNE_ABOUTMUI_DESC
#endif

#if ZUNE_BUILTIN_BOOPSI
#   define ZUNE_BOOPSI_DESC (&_MUI_Boopsi_desc),
#else
#   define ZUNE_BOOPSI_DESC
#endif

#if ZUNE_BUILTIN_GAUGE
#   define ZUNE_GAUGE_DESC (&_MUI_Gauge_desc),
#else
#   define ZUNE_GAUGE_DESC
#endif

#if ZUNE_BUILTIN_POPASL
#   define ZUNE_POPASL_DESC (&_MUI_Popasl_desc),
#else
#   define ZUNE_POPASL_DESC
#endif

#if ZUNE_BUILTIN_POPFRAME
#   define ZUNE_POPFRAME_DESC (&_MUI_Popframe_desc),
#else
#   define ZUNE_POPFRAME_DESC
#endif

#if ZUNE_BUILTIN_POPIMAGE
#   define ZUNE_POPIMAGE_DESC (&_MUI_Popimage_desc),
#else
#   define ZUNE_POPIMAGE_DESC
#endif

#if ZUNE_BUILTIN_POPPEN
#   define ZUNE_POPPEN_DESC (&_MUI_Poppen_desc),
#else
#   define ZUNE_POPPEN_DESC
#endif


struct IClass *GetPublicClass(CONST_STRPTR className, struct Library *mb);
BOOL DestroyClasses(struct Library *MUIMasterBase);
struct IClass *CreateBuiltinClass(CONST_STRPTR className, struct Library *MUIMasterBase);

AROS_UFP3
(
    IPTR, metaDispatcher,
    AROS_UFPA(struct IClass *, cl,  A0),
    AROS_UFPA(Object *,        obj, A2),
    AROS_UFPA(Msg     ,        msg, A1)
);

#endif /* _MUIMASTER_SUPPORT_CLASSES_H */

