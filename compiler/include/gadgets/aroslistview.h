#ifndef GADGETS_AROSLISTVIEW_H
#define GADGETS_AROSLISTVIEW_H

/*
    (C) 1995-97 AROS - The Amiga Replacement OS
    $Id$

    Desc: MethodIDs and AttrIDs for the AROS listview class.
    Lang: english
*/

#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif
#ifndef UTILITY_TAGITEM_H
#   include <utility/tagitem.h>
#endif

#define AROSLISTVIEWCLASS "listview.aros"

#define AROSLV_TAGBASE 20000L

/* Attribute IDs */

#define AROSA_Listview_DisplayHook	AROSLV_TAGBASE + 0 /* [I] */
#define AROSA_Listview_RenderHook	AROSLV_TAGBASE + 1 /* [IS] */
#define AROSA_Listview_HorSpacing	AROSLV_TAGBASE + 2 /* [ISG] */
#define AROSA_Listview_VertSpacing	AROSLV_TAGBASE + 3 /* [ISG] */
#define AROSA_Listview_List		AROSLV_TAGBASE + 4 /* [IS] */


#define AROSA_Listview_MaxColumns	AROSLV_TAGBASE + 5 /* [I] The displayhook limit */


/* Formatstring for the coulumns. If none is specified the default will be BACKGROUNDPEN,
 Leftalign and equal weight */

#define AROSA_Listview_Format 		AROSLV_TAGBASE + 6  /* [IS]	*/
#define AROSA_Listview_First 		AROSLV_TAGBASE + 7  /* [ISGUN]	*/
#define AROSA_Listview_MultiSelect	AROSLV_TAGBASE + 8  /* [IS]	*/
#define AROSA_Listview_DoubleClick	AROSLV_TAGBASE + 9  /* [G]	*/
#define AROSA_Listview_Visible		AROSLV_TAGBASE + 10 /* [N]	*/
#define AROSA_Listview_Total		AROSLV_TAGBASE + 11 /* [N]	*/

/* These methods are merely aliases for the List ones.
 * These are needed because I must include that damn
 * GadgetInfo structure as the 2nd argument.
 */
 
#define AROSM_Listview_Insert	    AROSM_List_Insert
#define AROSM_Listview_InsertSingle AROSM_List_InsertSingle
#define AROSM_Listview_Remove	    AROSM_List_Remove

struct AROSP_Listview_Insert
{
    STACKULONG 	MethodID;
    struct GadgetInfo *GInfo;
    APTR	*ItemArray;
    STACKLONG	Position;
};

struct AROSP_Listview_InsertSingle
{
    STACKULONG	MethodID;
    struct GadgetInfo *GInfo;
    APTR	Item;
    STACKLONG	Position;
};

struct AROSP_Listview_Remove
{
    STACKULONG		MethodID;
    struct GadgetInfo 	*GInfo;
    STACKLONG		Position;
};


/* Method IDs */


#endif /* GADGETS_AROSLISTVIEW_H */
