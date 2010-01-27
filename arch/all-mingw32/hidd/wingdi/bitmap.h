/*
    Copyright  1995-2008, The AROS Development Team. All rights reserved.
    $Id: bitmap.h 23833 2005-12-20 14:41:50Z stegerg $
*/

#ifndef _BITMAP_H
#define _BITMAP_H

/* This attribute interface is common for both gdi onscreen and offscreen bitmap
   classes, although they don't have a common superclass
*/


#define IID_Hidd_GDIBitMap "hidd.bitmap.gdibitmap"

#define HiddGDIBitMapAB __abHidd_GDIBitMap

enum
{
    aoHidd_GDIBitMap_DeviceContext, /* ..G - bitmap's device context	      */
    aoHidd_GDIBitMap_Window,	    /* .SG - a window the bitmap displayed in */
    aoHidd_GDIBitMap_SysDisplay,    /* I.. - a display to be compatible with  */
    aoHidd_GDIBitMap_DisplayWidth,  /* I.G - display window width	      */
    aoHidd_GDIBitMap_DisplayHeight, /* I.G - display window height	      */
    num_Hidd_GDIBitMap_Attrs
};

#define aHidd_GDIBitMap_DeviceContext	(HiddGDIBitMapAB + aoHidd_GDIBitMap_DeviceContext)
#define aHidd_GDIBitMap_Window		(HiddGDIBitMapAB + aoHidd_GDIBitMap_Window)
#define aHidd_GDIBitMap_SysDisplay	(HiddGDIBitMapAB + aoHidd_GDIBitMap_SysDisplay)
#define aHidd_GDIBitMap_DisplayWidth	(HiddGDIBitMapAB + aoHidd_GDIBitMap_DisplayWidth)
#define aHidd_GDIBitMap_DisplayHeight	(HiddGDIBitMapAB + aoHidd_GDIBitMap_DisplayHeight)

#define IS_BM_ATTR(attr, idx) 	    ( ( (idx) = (attr) - HiddBitMapAttrBase) < num_Hidd_BitMap_Attrs)
#define IS_GDIBM_ATTR(attr, idx)    ( ( (idx) = (attr) - HiddGDIBitMapAB) < num_Hidd_GDIBitMap_Attrs)

/* This structure is used as instance data for the bitmap class.
*/

struct bitmap_data
{
    APTR dc;	    /* Device context							       */
    APTR bitmap;    /* Actual bitmap object						       */
    APTR dc_bitmap; /* Original DC's bitmap object, needs to be put back before freeing the DC */
    APTR display;   /* System display - to what DC should be compatible			       */
    APTR window;    /* Window in which the bitmap is displayed				       */
    IPTR win_width; /* Window size (cached from ModeID)					       */
    IPTR win_height;
};

#endif /* _BITMAP_H */
