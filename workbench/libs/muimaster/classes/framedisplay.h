/*
    Copyright � 2003, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#ifndef _MUI_CLASSES_FRAMEDISPLAY_H
#define _MUI_CLASSES_FRAMEDISPLAY_H

#define MUIC_Framedisplay "Framedisplay.mui"

#ifndef METHOD_USER
#define METHOD_USER TAG_USER
#endif

/* Framedisplay attributes */
#define MUIA_Framedisplay_Spec       (TAG_USER|0x00421794) /* MUI: V??  isg struct MUI_FrameSpec  * */

extern const struct __MUIBuiltinClass _MUI_Framedisplay_desc; /* PRIV */

#endif
