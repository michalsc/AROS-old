/*
    (C) 1998 AROS - The Amiga Research OS
    $Id$

    Desc: GC class for the X11Gfx HIDD
    Lang: English.
*/

#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>


#include <proto/oop.h>
#include <oop/oop.h>
#include <proto/utility.h>
#include <utility/tagitem.h>

#include <hidd/graphics.h>

#include "x11gfx_intern.h"

#define SDEBUG 0
#define DEBUG 0
#include <aros/debug.h>


#undef SysBase
#define SysBase (X11GfxBase->sysbase)

#undef OOPBase
#define OOPBase (X11GfxBase->oopbase)

#undef UtilityBase
#define UtilityBase (X11GfxBase->utilitybase)

#define X11GfxBase ((struct x11gfxbase *)cl->UserData)


/* Attrbases */
static AttrBase HiddGCAttrBase = 0;

/* Private instance data for GC class */
struct gc_data
{
    GC 		gc;
    Window 	xwindow;
    Display	*display;
    int		screen;
    long	*hidd2x11cmap;

    
};

static ULONG map_x11_to_hidd(struct gc_data *data, ULONG x11pixel)
{
    ULONG hidd_pen = 0;
    BOOL pix_found = FALSE;
    for (hidd_pen = 0; hidd_pen < 256; hidd_pen ++)
    {
    	if (x11pixel == data->hidd2x11cmap[hidd_pen])
	{
	    pix_found = TRUE;
	    break;
	}
    }
    if (!pix_found)
    	hidd_pen = 0UL;
	
    return hidd_pen;	
    
}
/****************  GC::New()  *******************************/
static Object *gc_new(Class *cl, Object *o, struct pRoot_New *msg)
{
    EnterFunc(bug("X11Gfx.GC::New()\n"));
    o = (Object *)DoSuperMethod(cl, o, (Msg)msg);
    D(bug("Returned from super: %p\n", o));
    if (o)
    {
         Object *bitmap;
	 struct bitmap_data *bm_data;
	 struct gc_data *data;
	 XGCValues gcval;
	 BOOL ok = TRUE;
	 
	 data = (struct gc_data *)INST_DATA(cl, o);
	 
	 memset(data, 0, sizeof (struct gc_data));
	 

	 
         /* Get the bitmap attriute from the superclass */

	 GetAttr(o, aHidd_GC_BitMap, (IPTR *)&bitmap);
	 	 
	 /* Get direct access to the instance data (ugly, should
	    go through private methods/attrs but we don't hav'em yet
	 */
	 bm_data = (struct bitmap_data *)INST_DATA(X11GfxBase->bitmapclass, bitmap);

	 
	 /* For the sake of speed we duplicate some info from the bitmap object */
	 data->xwindow = bm_data->xwindow;
	 data->display = bm_data->sysdisplay;
	 data->screen  = bm_data->sysscreen;
	 data->hidd2x11cmap = bm_data->hidd2x11cmap;
	 
	 /* Create X11 GC */
	 
	 gcval.plane_mask = 0xFFFFFFFF; /*BlackPixel(data->display, data->screen); */ /* bm_data->sysplanemask; */
	 gcval.graphics_exposures = True;
	 
	 data->gc = XCreateGC( data->display
	 	, DefaultRootWindow( data->display )
		, GCPlaneMask | GCGraphicsExposures
		, &gcval
	 );
	 if (!data->gc)
	     ok = FALSE;
	 
	 if (!ok)
	 {
	      MethodID dispose_mid = GetMethodID(IID_Root, moRoot_Dispose);
	      CoerceMethod(cl, o, (Msg)&dispose_mid);
	      o = NULL;
	 }
	 
    } /* if (Object created by superclass) */
    
    ReturnPtr("X11Gfx.GC::New", Object *, o);

}

/********** GC::Dispose() **********************************/

static VOID gc_dispose(Class *cl, Object *o, Msg msg)
{
    struct gc_data *data = INST_DATA(cl, o);
    struct bitmap_data *bm_data;
    Object *bitmap;
     
    EnterFunc(bug("X11Gfx.GC::Dispose()\n"));
    GetAttr(o, aHidd_GC_BitMap, (IPTR *)&bitmap);
    bm_data = INST_DATA(X11GfxBase->bitmapclass, bitmap);
     
    if (data->gc)
    {
         XFreeGC(bm_data->sysdisplay, data->gc);
	 data->gc = NULL;
    }
    DoSuperMethod(cl, o, msg);
    
   ReturnVoid("X11Gfx.GC::Dispose");
}

/*********  GC::Set()  ****************************************/
#define IS_GC_ATTR(attr, idx) ( ( (idx) = (attr) - HiddGCAttrBase) < num_Hidd_GC_Attrs)

static VOID gc_set(Class *cl, Object *o, struct pRoot_Set *msg)
{
    struct gc_data *data = INST_DATA(cl, o);
    struct TagItem *tag, *tstate;
    ULONG idx;
    
    tstate = msg->attrList;
    while((tag = NextTagItem(&tstate)))
    {
        if(IS_GC_ATTR(tag->ti_Tag, idx))
        {
            switch(idx)
            {
                case aoHidd_GC_Foreground :
		    /* Set X GC color */
		    XSetForeground(data->display, data->gc, data->hidd2x11cmap[tag->ti_Data]);
		    break;
		    
                case aoHidd_GC_Background :
		    XSetBackground(data->display, data->gc, data->hidd2x11cmap[tag->ti_Data]);
		    break;
            }
        }
    }
    
    DoSuperMethod(cl, o, (Msg)msg);
    
    return;
}
/*********  GC::WritePixelDirect()  ***************************/

static VOID gc_writepixeldirect(Class *cl, Object *o, struct pHidd_GC_WritePixelDirect *msg)
{
     struct gc_data *data = INST_DATA(cl, o);
     ULONG old_fg;
     
     GetAttr(o, aHidd_GC_Foreground, &old_fg);
     
     XSetForeground(data->display, data->gc, data->hidd2x11cmap[msg->val]);
     XDrawPoint(data->display, data->xwindow, data->gc, msg->x, msg->y);
     
     /* Reset GC to old value */
     XSetForeground(data->display, data->gc, data->hidd2x11cmap[old_fg]);
     
     XFlush(data->display);
     return;
}

/*********  GC::ReadPixel()  *********************************/
static ULONG gc_readpixel(Class *cl, Object *o, struct pHidd_GC_ReadPixel *msg)
{
    ULONG pixel, i;
    struct gc_data *data = INST_DATA(cl, o);
    
    XImage *image;

    
    image = XGetImage(data->display
    	, data->xwindow
	, msg->x, msg->y
	, 1, 1
	, AllPlanes
	, ZPixmap);
    
    if (!image)
    	return -1L;
	
    pixel = XGetPixel(image, 0, 0);
    XFree(image);
    
    /* Get pen number from colortab */

    for (i = 0; i < 256; i ++)
    {
        if (pixel == data->hidd2x11cmap[i])
    	    return i;
    }
    
    return -1L;
    
    
}

/*********  gc_writepixel ************************************/
static ULONG gc_writepixel(Class *cl, Object *o, struct pHidd_GC_WritePixel *msg)
{

    struct gc_data *data = INST_DATA(cl, o);
    

    /* Foreground pen allready set in X GC. Note, though, that a
       call to WritePixelDirect may owerwrite th GC's pen  */
    XDrawPoint(data->display, data->xwindow, data->gc, msg->x, msg->y);
    XFlush(data->display);
    
    return 0;

    
}
/*********  GC::FillRect()  *************************************/
static VOID gc_fillrect(Class *cl, Object *o, struct pHidd_GC_DrawRect *msg)
{
    struct gc_data *data = INST_DATA(cl, o);
    ULONG mode;
#warning Handle different drawmodes
    EnterFunc(bug("X11Gfx.GC::FillRect(%d,%d,%d,%d)\n",
    	msg->minX, msg->minY, msg->maxX, msg->maxY));
	
    GetAttr(o, aHidd_GC_DrawMode, &mode);
    
    D(bug("Drawmode: %d\n", mode));
    
    
    if (mode == vHIDD_GC_DrawMode_Copy)
    {
    	
	XFillRectangle(data->display
		, data->xwindow
		, data->gc
		, msg->minX
		, msg->minY
		, msg->maxX - msg->minX + 1
		, msg->maxY - msg->minY + 1
	);
    }
    else
    {
    	XImage *image;
	WORD x, y, width, height;
	ULONG src;
	GetAttr(o, aHidd_GC_Foreground, &src);
	
	width  = msg->maxX - msg->minX + 1;
	height = msg->maxY - msg->minY + 1;
    	/* Special drawmode */
	image = XGetImage(data->display
		, data->xwindow
		, msg->minX, msg->minY
		, width, height
		, AllPlanes
		, ZPixmap);
		
	if (!image)
	    ReturnVoid("X11Gfx.GC::FillRect(Couldn't get XImage)");
	    
	for (y = 0; y < width; y ++)
	{
	    for (x = 0; x < height; x ++)
	    {
	        ULONG dest;
		ULONG val = 0UL;
		
		dest = map_x11_to_hidd(data, XGetPixel(image, x, y));
		    
		/* Apply drawmodes to pixel */
	   	if(mode & 1) val = ( src &  dest);
	   	if(mode & 2) val = ( src & ~dest) | val;
	   	if(mode & 4) val = (~src &  dest) | val;
	   	if(mode & 8) val = (~src & ~dest) | val;
		
		D(bug("Putting pixel %d\n", val));
		
		XPutPixel(image, x, y, data->hidd2x11cmap[val]);

	    }
	    
	}  
	D(bug("Putting image at (%d, %d), w=%d, h=%d\n",
		msg->minX, msg->minY, width, height ));
		
	/* Put image back into display */
	XPutImage(data->display
    		, data->xwindow
		, data->gc
		, image
		, 0, 0
		, msg->minX, msg->minY
		, width, height);
	    
	D(bug("image put\n"));

	XFree(image);
	D(bug("image destroyed\n"));
    }
   
    D(bug("Flushing\n"));

    XFlush(data->display);
    ReturnVoid("X11Gfx.GC::FillRect");
    

}

/*********  GC::CopyArea()  *************************************/
static VOID gc_copyarea(Class *cl, Object *o, struct pHidd_GC_CopyArea *msg)
{
    ULONG mode;
    struct gc_data *data = INST_DATA(cl, o);
    GetAttr(o, aHidd_GC_DrawMode, &mode);
    EnterFunc(bug("X11Gfx.GC::CopyArea( %d,%d to %d,%d of dim %d,%d\n",
    	msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height));

#warning Does not handle copying between different windows.

    if (mode == vHIDD_GC_DrawMode_Copy) /* Optimize this drawmode */
    {
    	XCopyArea(data->display
    		, data->xwindow	/* src	*/
		, data->xwindow /* dest */
		, data->gc
		, msg->srcX
		, msg->srcY
		, msg->width
		, msg->height
		, msg->destX
		, msg->destY
    	);
    
    }
    else
    {
	XImage *src_image, *dst_image;
	WORD x, y;
	
	src_image = XGetImage(data->display
		, data->xwindow
		, msg->srcX, msg->srcY
		, msg->width, msg->height
		, AllPlanes
		, ZPixmap);
		
	if (!src_image)
	    ReturnVoid("X11Gfx.GC::CopyArea(Couldn't get source XImage)");
	    
	dst_image = XGetImage(data->display
		, data->xwindow
		, msg->destX, msg->destY
		, msg->width, msg->height
		, AllPlanes
		, ZPixmap);
		
	if (!dst_image)
	{
	    XDestroyImage(src_image);
	    ReturnVoid("X11Gfx.GC::CopyArea(Couldn't get destination XImage)");
	}
     	   
	for (y = 0; y < msg->height; y ++)
	{
	    for (x = 0; x < msg->width; x ++)
	    {
    		/* Drawmodes make things more complicated */
		ULONG src;
		ULONG dest;
		ULONG val = 0;
		
		src  = map_x11_to_hidd(data, XGetPixel(src_image, x, y));
		dest = map_x11_to_hidd(data, XGetPixel(dst_image, x, y));
		    
		/* Apply drawmodes to pixel */
	   	if(mode & 1) val = ( src &  dest);
	   	if(mode & 2) val = ( src & ~dest) | val;
	   	if(mode & 4) val = (~src &  dest) | val;
	   	if(mode & 8) val = (~src & ~dest) | val;
		
		XPutPixel(dst_image, x, y, data->hidd2x11cmap[val]);
	    }
	}
	/* Put image back into display */
	XPutImage(data->display
    		, data->xwindow
		, data->gc
		, dst_image
		, 0, 0
		, msg->destX, msg->destY
		, msg->width, msg->height);
	

	XFree(src_image);
	XFree(dst_image);
	
    }
    
    
    XFlush(data->display);
    ReturnVoid("X11Gfx.GC::CopyArea");
}
/*********  GC::Clear()  *************************************/
static VOID gc_clear(Class *cl, Object *o, struct pHidd_GC_Clear *msg)
{
    Object *bitmap;
    ULONG width, height, bg;
    struct gc_data *data = INST_DATA(cl, o);
    
    XSetWindowAttributes winattr;
    
    GetAttr(o, aHidd_GC_Background, &bg);
    
    GetAttr(o, aHidd_GC_BitMap, (IPTR *)&bitmap);
    
    /* Get width & height from bitmap */
  
    GetAttr(bitmap, aHidd_BitMap_Width,  &width);
    GetAttr(bitmap, aHidd_BitMap_Height, &height);
    
    /* Change background color of X window to bg color of HIDD GC  */
    winattr.background_pixel = data->hidd2x11cmap[bg];
    
    XChangeWindowAttributes(data->display
    		, data->xwindow
		, CWBackPixel
		, &winattr);
    
    XClearArea (data->display, data->xwindow,
	    0, 0,
	    width, height,
	    FALSE);
    
    XFlush(data->display);
    return;
    
}

static VOID gc_readpixelarray(Class *cl, Object *o, struct pHidd_GC_ReadPixelArray *msg)
{
    /* Read an X image from which we can faster get the pixels, since the
       X image resides in the client.
    */
    WORD x, y;
    ULONG *pixarray = msg->pixelArray;
    struct gc_data *data;
    XImage *image;
    
    EnterFunc(bug("X11Gfx.GC::ReadPixelArray(pa=%p, x=%d, y=%d, w=%d, h=%d)\n",
    	msg->pixelArray, msg->x, msg->y, msg->width, msg->height));
	
    data = INST_DATA(cl, o);

    image = XGetImage(data->display
    	, data->xwindow
	, msg->x, msg->y
	, msg->width, msg->height
	, AllPlanes
	, ZPixmap);
	
    if (!image)
    	ReturnVoid("X11Gfx.GC::ReadPixelArray(couldn't get XImage)");
	
    for (y = 0; y < msg->height; y ++)
    {
	for (x = 0; x < msg->width; x ++)
	{
	    *pixarray ++ = map_x11_to_hidd(data, XGetPixel(image, x, y));
	}
	
    }
    XFree(image);
    
    ReturnVoid("X11Gfx.GC::ReadPixelArray");
    
}

static VOID gc_writepixelarray(Class *cl, Object *o, struct pHidd_GC_WritePixelArray *msg)
{
    ULONG mode;
    WORD x, y;
    ULONG *pixarray = msg->pixelArray;
    struct gc_data *data;
    XImage *image;

    EnterFunc(bug("X11Gfx.GC::WritePixelArray(pa=%p, x=%d, y=%d, w=%d, h=%d)\n",
    	msg->pixelArray, msg->x, msg->y, msg->width, msg->height));
	
    data = INST_DATA(cl, o);
    GetAttr(o, aHidd_GC_DrawMode, &mode);
    
    image = XGetImage(data->display
    	, data->xwindow
	, msg->x, msg->y
	, msg->width, msg->height
	, AllPlanes
	, ZPixmap
    );
    if (!image)
    	ReturnVoid("X11Gfx.GC::WritePixelArray(couldn't get XImage)");
    	
    D(bug("drawmode: %d\n", mode));
    if (mode == vHIDD_GC_DrawMode_Copy)
    {
        D(bug("Drawmode COPY\n"));
    	/* Do plain copy, optimized */
	for (y = 0; y < msg->height; y ++)
	{
	    for (x = 0; x < msg->width; x ++)
	    {
		
		XPutPixel(image, x, y, data->hidd2x11cmap[*pixarray ++]);
		
	    }
	    
	}
	
    }
    else
    {
     	   
	for (y = 0; y < msg->height; y ++)
	{
	    for (x = 0; x < msg->width; x ++)
	    {
    		/* Drawmodes make things more complicated */
		ULONG src;
		ULONG dest;
		ULONG val = 0;

		src  = *pixarray ++;
		dest = map_x11_to_hidd(data, XGetPixel(image, x, y));
		    
		/* Apply drawmodes to hidd pen */
	   	if(mode & 1) val = ( src &  dest);
	   	if(mode & 2) val = ( src & ~dest) | val;
	   	if(mode & 4) val = (~src &  dest) | val;
	   	if(mode & 8) val = (~src & ~dest) | val;
		
		XPutPixel(image, x, y, data->hidd2x11cmap[val]);
	    }
	}
    }
    /* Put image back into display */
    XPutImage(data->display
    	, data->xwindow
	, data->gc
	, image
	, 0, 0
	, msg->x, msg->y
	, msg->width, msg->height);
	
	
   XFree(image);

   XFlush(data->display);
   ReturnVoid("X11Gfx.GC::WritePixelArray");

   
}
/****************  init_gcclass()  **************************/
#undef X11GfxBase



#define NUM_ROOT_METHODS	3
#define NUM_GC_METHODS 		8


Class *init_gcclass(struct x11gfxbase *X11GfxBase)
{
    struct MethodDescr root_descr[NUM_ROOT_METHODS + 1] = 
    {
    	{(IPTR (*)())gc_new,		moRoot_New},
    	{(IPTR (*)())gc_dispose,	moRoot_Dispose},
    	{(IPTR (*)())gc_set,		moRoot_Set},
	{NULL, 0UL}
    };

    struct MethodDescr gc_descr[NUM_GC_METHODS + 1] = 
    {
    	{(IPTR (*)())gc_writepixeldirect,	moHidd_GC_WritePixelDirect},
    	{(IPTR (*)())gc_clear,			moHidd_GC_Clear},
    	{(IPTR (*)())gc_readpixel,		moHidd_GC_ReadPixel},
    	{(IPTR (*)())gc_writepixel,		moHidd_GC_WritePixel},
    	{(IPTR (*)())gc_fillrect,		moHidd_GC_FillRect},
    	{(IPTR (*)())gc_copyarea,		moHidd_GC_CopyArea},
    	{(IPTR (*)())gc_readpixelarray,		moHidd_GC_ReadPixelArray},
    	{(IPTR (*)())gc_writepixelarray,	moHidd_GC_WritePixelArray},
	{NULL, 0UL}
    };
    
    
    struct InterfaceDescr ifdescr[] =
    {
    	{root_descr,	IID_Root,		NUM_ROOT_METHODS},
    	{gc_descr, 	IID_Hidd_GC,		NUM_GC_METHODS},
	{NULL, NULL, 0}
    };
    
    AttrBase MetaAttrBase   = ObtainAttrBase(IID_Meta);
	
    struct TagItem tags[] =
    {
	{aMeta_SuperID,			(IPTR)CLID_Hidd_GCQuick},
	{aMeta_InterfaceDescr,		(IPTR)ifdescr},
	{aMeta_InstSize,		(IPTR)sizeof (struct gc_data) },
	{TAG_DONE, 0UL}
    };
    
    Class *cl;

    if (MetaAttrBase)
    {
    	cl = NewObject(NULL, CLID_HiddMeta, tags);
    
    	if (cl)
    	{
	    /* Get attrbase for the GC interface */
	    HiddGCAttrBase = ObtainAttrBase(IID_Hidd_GC);

	    if (HiddGCAttrBase)
	    {
	    	D(bug("GC class ok\n"));
	    	cl->UserData = (APTR)X11GfxBase;
		AddClass(cl);
	    }
	    else
	    {
	    	free_gcclass(X11GfxBase);
		
		cl = NULL;
	    }
	

	} /* if (cl) */

	/* We don't need this anymore */
	ReleaseAttrBase(IID_Meta);
    } /* if(MetaAttrBase) */

    return cl;
}


/*************** free_gclass() *******************************/

VOID free_gcclass(struct x11gfxbase *X11GfxBase)
{
    EnterFunc(bug("free_gcclass(X11GfxBase=%p)\n", X11GfxBase));

    if(X11GfxBase)
    {
        RemoveClass(X11GfxBase->gcclass);
	
        if(X11GfxBase->gcclass)
	    DisposeObject((Object *) X11GfxBase->gcclass);
	
        X11GfxBase->gcclass = NULL;
	
        if(HiddGCAttrBase) 
	    ReleaseAttrBase(IID_Hidd_GC);
	HiddGCAttrBase = 0;
	
    }

    ReturnVoid("free_gcclass");
}
