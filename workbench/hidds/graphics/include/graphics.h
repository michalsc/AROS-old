#ifndef HIDD_GRAPHICS_H
#define HIDD_GRAPHICS_H

/*
    Copyright (C) 1998 AROS - The Amiga Research OS
    $Id$

    Desc: Definitions for the Graphics HIDD system.
    Lang: english
*/
#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif
#ifndef HIDD_HIDD_H
#   include <hidd/hidd.h>
#endif
#ifndef OOP_OOP_H
#   include <oop/oop.h>
#endif

#include <utility/utility.h>


#define CLID_Hidd_Gfx           "hidd.graphics.graphics"
#define CLID_Hidd_BitMap        "hidd.graphics.bitmap"
#define CLID_Hidd_GC		"hidd.graphics.gc"

#define IID_Hidd_Gfx      "hidd.graphics.graphics"
#define IID_Hidd_BitMap   "hidd.graphics.bitmap"
#define IID_Hidd_GC       "hidd.graphics.gc"

/* Some "example" hidd bitmaps */

#define CLID_Hidd_PlanarBM "hidd.graphics.bitmap.planarbm"
#define CLID_Hidd_ChunkyBM "hidd.graphics.bitmap.chunkybm"


typedef struct Object *HIDDT_BitMap;
typedef struct Object *HIDDT_GC;


/* Attrbases */
#define HiddGCAttrBase          __IHidd_GC
#define HiddGfxAttrBase         __IHidd_Gfx
#define HiddBitMapAttrBase      __IHidd_BitMap

extern AttrBase HiddGCAttrBase;
extern AttrBase HiddGfxAttrBase;
extern AttrBase HiddBitMapAttrBase;


/**** Graphics definitions ****************************************************/

enum
{
    /* Methods for a graphics hidd */

    moHidd_Gfx_NewGC = 0,       /* Its only allowd to use these methods   */
    moHidd_Gfx_DisposeGC,       /* to create and to dispose a GC and      */
    moHidd_Gfx_NewBitMap,       /* bitmap because only the graphics-hidd  */
    moHidd_Gfx_DisposeBitMap   /* class knows which gc- and bitmap-class */
				/* works together.                        */
};




/* messages for a graphics hidd */

struct pHidd_Gfx_NewGC
{
    MethodID       mID;
    struct TagItem *attrList;
};

struct pHidd_Gfx_DisposeGC
{
    MethodID    mID;
    Object      *gc;
};

struct pHidd_Gfx_NewBitMap
{
    MethodID       mID;
    struct TagItem *attrList;
};

struct pHidd_Gfx_DisposeBitMap
{
    MethodID    mID;
    Object      *bitMap;
};




/**** BitMap definitions ******************************************************/


	/* Types */


typedef UWORD HIDDT_ColComp;	/* Color component */
	
typedef struct {
   HIDDT_ColComp	red;
   HIDDT_ColComp	green;
   HIDDT_ColComp	blue;
   HIDDT_ColComp	alpha;
   
} HIDDT_Color;

typedef ULONG HIDDT_Pixel;


typedef struct {
	ULONG entries;
	HIDDT_Pixel *pixels;
} HIDDT_PixelLUT;

typedef struct {
	ULONG entries;
	HIDDT_Color *colors;
} HIDDT_ColorLUT;

typedef ULONG HIDDT_StdPixFmt;

#define MAP_SHIFT	((sizeof (HIDDT_Pixel) - sizeof (HIDDT_ColComp)) *8)
#define SHIFT_UP_COL(col) ((HIDDT_Pixel)((col) << MAP_SHIFT))

#define MAP_COLCOMP(comp, val, pixfmt)	\
	((SHIFT_UP_COL(val) >> (pixfmt)-> ## comp ## _shift) & (pixfmt)-> ## comp ## _mask)
	

#define MAP_RGB(r, g, b, pixfmt) 	\
	  MAP_COLCOMP(red,   r, pixfmt) \
	| MAP_COLCOMP(green, g, pixfmt)	\
	| MAP_COLCOMP(blue,  b, pixfmt) 


#define MAP_RGBA(r, g, b, pixfmt) 	\
	  MAP_COMP(red,   r, pixfmt) 	\
	| MAP_COMP(green, g, pixfmt)	\
	| MAP_COMP(blue,  b, pixfmt)	\
	| MAP_COMP(blue,  b, pixfmt)
	
	

#define SHIFT_DOWN_PIX(pix) ((pix) >> MAP_SHIFT)
#define GET_COLCOMP(comp, pix, pixfmt) \
	SHIFT_DOWN_PIX( ( (pix) & (pixfmt)-> ## comp ## _mask) << pixfmt-> ## comp ## _shift )

#define RED_COMP(pix, pixfmt)	GET_COLCOMP(red,   pix, pixfmt)
#define GREEN_COMP(pix, pixfmt)	GET_COLCOMP(green, pix, pixfmt)
#define BLUE_COMP(pix, pixfmt)	GET_COLCOMP(blue,  pix, pixfmt)

typedef struct {
	UWORD	depth;
	UWORD	size;	
	
	HIDDT_Pixel red_mask;
	HIDDT_Pixel green_mask;
	HIDDT_Pixel blue_mask;
	HIDDT_Pixel alpha_mask;
	
	HIDDT_Pixel red_shift;
	HIDDT_Pixel green_shift;
	HIDDT_Pixel blue_shift;
	HIDDT_Pixel alpha_shift;
	
} HIDDT_PixelFormat;



enum
{
    /* Methods for a bitmap */

    moHidd_BitMap_SetColors,
    moHidd_BitMap_CopyBox,
    moHidd_BitMap_PutPixel,
    moHidd_BitMap_DrawPixel,
    moHidd_BitMap_PutImage,
    moHidd_BitMap_GetImage,
    moHidd_BitMap_GetPixel,
    moHidd_BitMap_DrawLine,
    moHidd_BitMap_DrawRect,
    moHidd_BitMap_FillRect,
    moHidd_BitMap_DrawEllipse,
    moHidd_BitMap_FillEllipse,
    moHidd_BitMap_DrawPolygon,
    moHidd_BitMap_FillPolygon,
    moHidd_BitMap_DrawText,
    moHidd_BitMap_FillText,
    moHidd_BitMap_FillSpan,
    moHidd_BitMap_Clear,
    moHidd_BitMap_BlitColorExpansion,
    moHidd_BitMap_MapColor,
    moHidd_BitMap_UnmapPixel,
    moHidd_BitMap_PutImageLUT,
    moHidd_BitMap_GetImageLUT,
    
    moHidd_BitMap_PrivateSet
};

enum {
    /* Attributes for a bitmap */
    aoHidd_BitMap_BitMap,        /* [..G] pointer to bitmap structure        */
    aoHidd_BitMap_Width,         /* [ISG] Bitmap with                          */
    aoHidd_BitMap_Height,        /* [ISG] Bitmap height                        */
    aoHidd_BitMap_Depth,         /* [I.G] Bitmap depth                         */
    aoHidd_BitMap_Displayable,   /* [I.G] BOOL bitmap is displayable           */
    aoHidd_BitMap_Visible,       /* [..G] Check if a bitmap is visible         */
    aoHidd_BitMap_Mode,          /* [ISG] The display mode of this bitmap      */
    aoHidd_BitMap_BaseAddress,   /* [ISG] Bitmap adress in RAM                 */
    aoHidd_BitMap_Format,        /* [..G] Tell the format of the bitmap data   */
    aoHidd_BitMap_BytesPerRow,   /* [..G] Number of bytes in a row             */
    aoHidd_BitMap_BytesPerPixel, /* [..G] Number of byter per pixel            */
    aoHidd_BitMap_BestSize,      /* [..G] Best size for depth                  */
    aoHidd_BitMap_LeftEdge,      /* [I.G] Left edge position of the bitmap     */
    aoHidd_BitMap_TopEdge,       /* [I.G] Top edge position of the bitmap      */
    aoHidd_BitMap_ColorTab,      /* [ISG] Colormap of the bitmap               */
    aoHidd_BitMap_AllocBuffer,   /* [I..] BOOL allocate buffer (default: TRUE) */
    
    aoHidd_BitMap_Foreground,          /* [.SG] Foreground color                   */
    aoHidd_BitMap_Background,          /* [.SG] Background color                   */
    aoHidd_BitMap_DrawMode,            /* [.SG] Draw mode                          */
    aoHidd_BitMap_Font,                /* [.SG] Current font                       */
    aoHidd_BitMap_ColorMask,           /* [.SG] Prevents some color bits from      */
                                   /*       changing                           */
    aoHidd_BitMap_LinePattern,         /* [.SG] Pattern for line drawing          */
    aoHidd_BitMap_PlaneMask,           /* [.SG] Shape bitmap                       */

    aoHidd_BitMap_GC,           /* [ISG]  bitmaps GC                       */
    aoHidd_BitMap_ColorExpansionMode,	/* [ISG] Mode for color expansion operations */
    
    
    aoHidd_BitMap_Friend,	/* [I.G] Friend bitmap. The bitmap will be allocated so that it
    				   is optimized for blitting to this bitmap */
    
    num_Hidd_BitMap_Attrs
};    

#define aHidd_BitMap_BitMap        (HiddBitMapAttrBase + aoHidd_BitMap_BitMap)
#define aHidd_BitMap_Width         (HiddBitMapAttrBase + aoHidd_BitMap_Width)
#define aHidd_BitMap_Height        (HiddBitMapAttrBase + aoHidd_BitMap_Height)
#define aHidd_BitMap_Depth         (HiddBitMapAttrBase + aoHidd_BitMap_Depth)
#define aHidd_BitMap_Displayable   (HiddBitMapAttrBase + aoHidd_BitMap_Displayable)
#define aHidd_BitMap_Visible       (HiddBitMapAttrBase + aoHidd_BitMap_Visible)
#define aHidd_BitMap_Mode          (HiddBitMapAttrBase + aoHidd_BitMap_Mode)
#define aHidd_BitMap_BaseAddress   (HiddBitMapAttrBase + aoHidd_BitMap_BaseAddress)
#define aHidd_BitMap_Format        (HiddBitMapAttrBase + aoHidd_BitMap_Format)
#define aHidd_BitMap_BytesPerRow   (HiddBitMapAttrBase + aoHidd_BitMap_BytesPerRow)
#define aHidd_BitMap_BytesPerPixel (HiddBitMapAttrBase + aoHidd_BitMap_BytesPerPixel)
#define aHidd_BitMap_BestSize      (HiddBitMapAttrBase + aoHidd_BitMap_BestSize)
#define aHidd_BitMap_LeftEdge      (HiddBitMapAttrBase + aoHidd_BitMap_LeftEdge)
#define aHidd_BitMap_TopEdge       (HiddBitMapAttrBase + aoHidd_BitMap_TopEdge)
#define aHidd_BitMap_ColorTab      (HiddBitMapAttrBase + aoHidd_BitMap_ColorTab)
#define aHidd_BitMap_AllocBuffer   (HiddBitMapAttrBase + aoHidd_BitMap_AllocBuffer)

#define aHidd_BitMap_Foreground  (HiddBitMapAttrBase + aoHidd_BitMap_Foreground)
#define aHidd_BitMap_Background  (HiddBitMapAttrBase + aoHidd_BitMap_Background)
#define aHidd_BitMap_DrawMode    (HiddBitMapAttrBase + aoHidd_BitMap_DrawMode)
#define aHidd_BitMap_Font        (HiddBitMapAttrBase + aoHidd_BitMap_Font)
#define aHidd_BitMap_ColorMask   (HiddBitMapAttrBase + aoHidd_BitMap_ColorMask)
#define aHidd_BitMap_LinePattern (HiddBitMapAttrBase + aoHidd_BitMap_LinePattern)
#define aHidd_BitMap_PlaneMask   (HiddBitMapAttrBase + aoHidd_BitMap_PlaneMask)
#define aHidd_BitMap_GC		 (HiddBitMapAttrBase + aoHidd_BitMap_GC)
#define aHidd_BitMap_ColorExpansionMode	 (HiddBitMapAttrBase + aoHidd_BitMap_ColorExpansionMode)

#define aHidd_BitMap_Friend		 (HiddBitMapAttrBase + aoHidd_BitMap_Friend)



/* BitMap formats */

#define vHidd_BitMap_Format_Planar   0x1
#define vHidd_BitMap_Format_Chunky   0x2


/* messages for a bitmap */

struct pHidd_BitMap_PrivateSet
{
    MethodID       mID;
    struct TagItem *attrList;
};

struct pHidd_BitMap_SetColors
{
    MethodID	mID;
    HIDDT_Color	*colors;
    ULONG	firstColor;
    ULONG	numColors;
};

/* messages for a graphics context */

struct pHidd_BitMap_PutPixel
{
    MethodID  mID;
    WORD x, y;
    HIDDT_Pixel pixel;
};

struct pHidd_BitMap_GetPixel
{
    MethodID  mID;
    WORD x, y;
};

struct pHidd_BitMap_DrawPixel
{
    MethodID  mID;
    WORD x, y;
};

struct pHidd_BitMap_DrawLine
{
    MethodID    mID;
    WORD        x1 ,y1, x2, y2;
};

struct pHidd_BitMap_CopyBox
{
    MethodID    mID;
    WORD        srcX, srcY;
    Object      *dest;
    WORD        destX, destY;
    UWORD       width, height;
};

struct pHidd_BitMap_GetImage
{
    MethodID mID;
    ULONG	*pixels;
    WORD	x, y;
    WORD	width, height;
};

struct pHidd_BitMap_PutImage
{
    MethodID mID;
    ULONG 	*pixels;
    WORD	x, y;
    WORD	width, height;
};


struct pHidd_BitMap_DrawRect
{
    MethodID    mID;
    WORD        minX, minY, maxX, maxY;
};

struct pHidd_BitMap_DrawEllipse
{
    MethodID    mID;
    WORD        x, y;
    UWORD       rx, ry;
};

struct pHidd_BitMap_DrawPolygon
{
    MethodID    mID;
    WORD        n;         /* number of coordinates */
    WORD        *coords;   /* size 2*n              */
};

struct pHidd_BitMap_DrawText
{
    MethodID    mID;
    WORD        x, y;      /* Start position, see autodocs */
    STRPTR      text;      /* Latin 1 string               */
    UWORD       length;    /* Number of characters to draw */
};

struct pHidd_BitMap_Clear
{
    MethodID    mID;
};

struct pHidd_BitMap_BlitColorExpansion
{
    MethodID mID;
    Object	*srcBitMap;
    WORD	srcX;
    WORD	srcY;
    WORD	destX;
    WORD	destY;
    UWORD	width;
    UWORD	height;
};

struct pHidd_BitMap_MapColor
{
    MethodID mID;
    HIDDT_Color *color;
};

struct pHidd_BitMap_UnmapPixel
{
    MethodID mID;
    HIDDT_Pixel pixel;
    HIDDT_Color *color;
};

struct pHidd_BitMap_PutImageLUT
{
    MethodID mID;
    UBYTE 	*pixels;
    ULONG	modulo;
    WORD	x, y;
    WORD	width, height;
    HIDDT_PixelLUT *pixlut;
};

struct pHidd_BitMap_GetImageLUT
{
    MethodID mID;
    UBYTE	*pixels;
    ULONG	modulo;
    WORD	x, y;
    WORD	width, height;
    HIDDT_PixelLUT *pixlut;
};


/**** Graphics context definitions ********************************************/
    /* Methods for a graphics context */
/*    
enum
{


};
*/
enum
{
    /* Attributes for a graphics context */

    aoHidd_GC_UserData,            /* [.SG] User data                          */
    aoHidd_GC_BitMap,              /* [I.G] Bitmap which this gc uses          */
    aoHidd_GC_Foreground,          /* [.SG] Foreground color                   */
    aoHidd_GC_Background,          /* [.SG] Background color                   */
    aoHidd_GC_DrawMode,            /* [.SG] Draw mode                          */
    aoHidd_GC_Font,                /* [.SG] Current font                       */
    aoHidd_GC_ColorMask,           /* [.SG] Prevents some color bits from      */
                                   /*       changing                           */
    aoHidd_GC_LinePattern,         /* [.SG] Pattern for line drawing          */
    aoHidd_GC_PlaneMask,           /* [.SG] Shape bitmap                       */
    aoHidd_GC_ColorExpansionMode,	   /* [.SG] Mode for color expansion */
    
    num_Hidd_GC_Attrs
};

#define aHidd_GC_UserData    (HiddGCAttrBase + aoHidd_GC_UserData)
#define aHidd_GC_BitMap      (HiddGCAttrBase + aoHidd_GC_BitMap)
#define aHidd_GC_Foreground  (HiddGCAttrBase + aoHidd_GC_Foreground)
#define aHidd_GC_Background  (HiddGCAttrBase + aoHidd_GC_Background)
#define aHidd_GC_DrawMode    (HiddGCAttrBase + aoHidd_GC_DrawMode)
#define aHidd_GC_Font        (HiddGCAttrBase + aoHidd_GC_Font)
#define aHidd_GC_ColorMask   (HiddGCAttrBase + aoHidd_GC_ColorMask)
#define aHidd_GC_LinePattern (HiddGCAttrBase + aoHidd_GC_LinePattern)
#define aHidd_GC_PlaneMask   (HiddGCAttrBase + aoHidd_GC_PlaneMask)
#define aHidd_GC_ColorExpansionMode  (HiddGCAttrBase + aoHidd_GC_ColorExpansionMode)


/* Drawmodes for a graphics context */


#define vHidd_GC_DrawMode_Clear 	0x00 /* 0 */
#define vHidd_GC_DrawMode_And 		0x01 /* src AND dst	*/
#define vHidd_GC_DrawMode_Copy 		0x03 /* src		*/
#define vHidd_GC_DrawMode_Xor  		0x06 /* src XOR dst	*/
#define vHidd_GC_DrawMode_Invert  	0x0A /* NOT dst		*/


#define vHidd_GC_ColExp_Transparent	(1 << 0)
#define vHidd_GC_ColExp_Opaque		(1 << 1)

/* Predeclarations of stubs in libhiddgraphicsstubs.h */

Object * HIDD_Gfx_NewGC        (Object *hiddGfx, struct TagItem *tagList);
VOID     HIDD_Gfx_DisposeGC    (Object *hiddGfx, Object *gc);
Object * HIDD_Gfx_NewBitMap    (Object *hiddGfx, struct TagItem *tagList);
VOID     HIDD_Gfx_DisposeBitMap(Object *hiddGfx, Object *bitMap);


VOID     HIDD_BM_BltBitMap   (Object obj, Object dest, WORD srcX, WORD srcY, WORD destX, WORD destY, WORD width, WORD height);
BOOL     HIDD_BM_Show        (Object obj);
VOID     HIDD_BM_Move        (Object obj, WORD x, WORD y);
BOOL     HIDD_BM_DepthArrange(Object obj, Object bm);
BOOL	 HIDD_BM_SetColors	(Object *obj, HIDDT_Color *tab, ULONG firstcolor, ULONG numcolors);

ULONG    HIDD_BM_PutPixel(Object *obj, WORD x, WORD y, HIDDT_Pixel pixel);
HIDDT_Pixel    HIDD_BM_GetPixel       (Object *obj, WORD x, WORD y);
ULONG    HIDD_BM_DrawPixel      (Object *obj, WORD x, WORD y);
VOID     HIDD_BM_CopyBox         (Object *obj, WORD srcX, WORD srcY, Object *dest, WORD destX, WORD destY, UWORD width, UWORD height);
VOID     HIDD_BM_GetImage	 	 (Object *obj, ULONG *pixelArray, WORD x, WORD y, WORD width, WORD height);
VOID	 HIDD_BM_PutImage 	 (Object *obj, ULONG *pixelArray, WORD x, WORD y, WORD width, WORD height);
VOID     HIDD_BM_DrawLine        (Object *obj, WORD x1, WORD y1, WORD x2, WORD y2);
VOID     HIDD_BM_DrawRect        (Object *obj, WORD minX, WORD minY, WORD maxX, WORD maxY);
VOID     HIDD_BM_FillRect        (Object *obj, WORD minX, WORD minY, WORD maxX, WORD maxY);
VOID     HIDD_BM_DrawEllipse     (Object *obj, WORD x, WORD y, WORD ry, WORD rx);
VOID     HIDD_BM_FillEllipse     (Object *obj, WORD x, WORD y, WORD ry, WORD rx);
VOID     HIDD_BM_DrawArc         (Object *obj);
VOID     HIDD_BM_FillArc         (Object *obj);
VOID     HIDD_BM_DrawPolygon     (Object *obj, UWORD n, WORD *coords);
VOID     HIDD_BM_FillPolygon     (Object *obj, UWORD n, WORD *coords);
VOID     HIDD_BM_DrawText        (Object *obj, WORD x, WORD y, STRPTR text, UWORD length);
VOID     HIDD_BM_FillText        (Object *obj, WORD x, WORD y, STRPTR text, UWORD length);
VOID     HIDD_BM_FillSpan        (Object *obj);
VOID     HIDD_BM_Clear           (Object *obj);
VOID	 HIDD_BM_BlitColorExpansion	 (Object *destObj, Object *srcObj, WORD srcX, WORD srcY, WORD destX, WORD destY,  UWORD width, UWORD height);

HIDDT_Pixel HIDD_BM_MapColor (Object *destObj, HIDDT_Color *color);
VOID	 HIDD_BM_UnmapPixel(Object *destObj, HIDDT_Pixel pixel, HIDDT_Color *color);

VOID	 HIDD_BM_PutImageLUT 	 (Object *obj, UBYTE *pixels, ULONG modulo, WORD x, WORD y, WORD width, WORD height, HIDDT_PixelLUT *pixlut);
VOID	 HIDD_BM_GetImageLUT 	 (Object *obj, UBYTE *pixels, ULONG modulo, WORD x, WORD y, WORD width, WORD height, HIDDT_PixelLUT *pixlut);


/*******************************************************/
/**  PROTECTED DATA 
	!! These structures are at the top of the gfx hidd baseclasses.
	DO NEVER ATTEMPT TO ACCESS THESE FROM OUTSIDE OF SUBCLASSES.
	DON'T EVEN THINK ABOUT IT !!!
	THEY SHOULD ONLY BE ACCESSED FROM THE SUBCLASSES, AND THEN
	BY USING THE MACROS BELOW !! 
*/

struct _hidd_bitmap_protected
{
	HIDDT_PixelFormat pixfmt;
};

#define BM_PIXFMT(bm) (&((struct _hidd_bitmap_protected *)bm)->pixfmt)
#define BM_DEPTH(bm)	BM_PIXFMT(bm)->depth


#endif /* HIDD_GRAPHICS_H */
