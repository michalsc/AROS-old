/*
    (C) 1995-98 AROS - The Amiga Research OS
    $Id$

    Desc: Driver for using gfxhidd for gfx output
    Lang: english
*/

#define AROS_ALMOST_COMPATIBLE 1


#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/arossupport.h>
#include <proto/utility.h>
#include <proto/oop.h>

#include <exec/memory.h>
#include <exec/semaphores.h>
#include <clib/macros.h>

#include <graphics/rastport.h>
#include <graphics/gfxbase.h>
#include <graphics/text.h>
#include <graphics/view.h>
#include <graphics/layers.h>
#include <graphics/clip.h>
#include <graphics/gfxmacros.h>
#include <graphics/regions.h>

#include <oop/oop.h>
#include <utility/tagitem.h>
#include <aros/asmcall.h>

#include <intuition/intuition.h>

#include <hidd/graphics.h>

#include <cybergraphx/cybergraphics.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "graphics_intern.h"
#include "graphics_internal.h"
#include "intregions.h"

#define SDEBUG 0
#define DEBUG 0
#include <aros/debug.h>

/* Default font for the HIDD driver */
#include "default_font.c"

#define PEN_BITS    8
#define NUM_COLORS  (1L << PEN_BITS)
#define PEN_MASK    (NUM_COLORS - 1)



/* Storage for bitmap object */

#define OBTAIN_HIDD_BM(bitmap)	\
	( ( IS_HIDD_BM(bitmap))	\
		? HIDD_BM_OBJ(bitmap)		\
		: get_planarbm_object((bitmap), GfxBase) ) 
		
#define RELEASE_HIDD_BM(bm_obj, bitmap)	\
	( ( IS_HIDD_BM(bitmap))	\
		? 0				\
		: release_cache_object(SDD(GfxBase)->planarbm_cache, (bm_obj), GfxBase) ) 
	
					
#define IS_HIDD_BM(bitmap) (((bitmap)->Flags & BMF_AROS_HIDD) == BMF_AROS_HIDD)


#define BM_PIXEL(bitmap, pen) (IS_HIDD_BM(bitmap) ? HIDD_CM_GetPixel(HIDD_BM_COLMAP(bitmap), pen) : (pen))


/* Minterms and GC drawmodes are in opposite order */
#define MINTERM_TO_GCDRMD(minterm) 	\
((  	  ((minterm & 0x80) >> 3)	\
	| ((minterm & 0x40) >> 1)	\
	| ((minterm & 0x20) << 1)	\
	| ((minterm & 0x10) << 3) )  >> 4 )



/* Rastport flag that tells whether or not the driver has been inited */

#define RPF_DRIVER_INITED (1L << 15)

#define AROS_PALETTE_SIZE 256
#define AROS_PALETTE_MEMSIZE (sizeof (HIDDT_Pixel) * AROS_PALETTE_SIZE)

static VOID setbitmapfast(struct BitMap *bm, LONG x_start, LONG y_start, LONG xsize, LONG ysize, ULONG pen);
static Object *fontbm_to_hiddbm(struct TextFont *font, struct GfxBase *GfxBase);
static LONG fillrect_pendrmd(struct RastPort *tp
	, LONG x1, LONG y1
	, LONG x2, LONG y2
	, HIDDT_Pixel pix
	, ULONG drmd
	, struct GfxBase *GfxBase);
	
static inline Object *get_planarbm_object(struct BitMap *bitmap, struct GfxBase *GfxBase);

struct render_special_info {
    struct BitMap *curbm;
    BOOL onscreen;
    LONG layer_rel_srcx;
    LONG layer_rel_srcy;
};


ULONG do_render_func(struct RastPort *rp
	, Point *src
	, struct Rectangle *rr
	, ULONG (*render_func)(APTR, LONG, LONG, Object *, Object *, LONG, LONG, LONG, LONG, struct GfxBase *)
	, APTR funcdata
	, BOOL get_special_info
	, struct GfxBase *GfxBase);

ULONG do_pixel_func(struct RastPort *rp
	, LONG x, LONG y
	, LONG (*render_func)(APTR, Object *, Object *, LONG, LONG, struct GfxBase *)
	, APTR funcdata
	, struct GfxBase *GfxBase);

static HIDDT_StdPixFmt cyber2hidd_pixfmt(UWORD cpf, struct GfxBase *GfxBase);
static UWORD hidd2cyber_pixfmt(HIDDT_StdPixFmt stdpf, struct GfxBase *GfxBase);

static BOOL int_bltbitmap(struct BitMap *srcBitMap, Object *srcbm_obj
	, LONG xSrc, LONG ySrc
	, struct BitMap *dstBitMap, Object *dstbm_obj
	, LONG xDest, LONG yDest, LONG xSize, LONG ySize
	, ULONG minterm, Object *gc, struct GfxBase *GfxBase);

struct pix_render_data {
    HIDDT_Pixel pixel;
};

static LONG pix_write(APTR pr_data
	, Object *bm, Object *gc
	, LONG x, LONG y
	, struct GfxBase *GfxBase);

#if 0
static AttrBase HiddBitMapAttrBase	= 0;
static AttrBase HiddGCAttrBase		= 0;
static AttrBase HiddGfxModeAttrBase	= 0;
static AttrBase HiddPixFmtAttrBase	= 0; 
static AttrBase HiddPlanarBMAttrBase	= 0; 
static AttrBase HiddGfxAttrBase		= 0; 

static struct ABDescr attrbases[] = {
    { IID_Hidd_BitMap,		&HiddBitMapAttrBase	},
    { IID_Hidd_GC,		&HiddGCAttrBase		},
    { IID_Hidd_GfxMode,		&HiddGfxModeAttrBase	},
    { IID_Hidd_PixFmt,		&HiddPixFmtAttrBase	},
    { IID_Hidd_PlanarBM,	&HiddPlanarBMAttrBase	},
    { IID_Hidd_Gfx,		&HiddGfxAttrBase	},
    { NULL, 0UL }
};

#else
AttrBase HiddBitMapAttrBase	= 0;
AttrBase HiddGCAttrBase		= 0;
AttrBase HiddGfxModeAttrBase	= 0;
AttrBase HiddPixFmtAttrBase	= 0; 
AttrBase HiddPlanarBMAttrBase	= 0; 
AttrBase HiddGfxAttrBase	= 0; 

struct ABDescr attrbases[] = {
    { IID_Hidd_BitMap,		&HiddBitMapAttrBase	},
    { IID_Hidd_GC,		&HiddGCAttrBase		},
    { IID_Hidd_GfxMode,		&HiddGfxModeAttrBase	},
    { IID_Hidd_PixFmt,		&HiddPixFmtAttrBase	},
    { IID_Hidd_PlanarBM,	&HiddPlanarBMAttrBase	},
    { IID_Hidd_Gfx,		&HiddGfxAttrBase	},
    { NULL, 0UL }
};

#endif
#define PIXELBUF_SIZE 200000
#define NUMPIX (PIXELBUF_SIZE / 4)

#define NUMLUTPIX (PIXELBUF_SIZE)

/* This buffer is used for planar-to-chunky-converion */
static ULONG *pixel_buf;
static struct SignalSemaphore pixbuf_sema;
static struct SignalSemaphore blit_sema;

#define LOCK_PIXBUF ObtainSemaphore(&pixbuf_sema);
#define ULOCK_PIXBUF ReleaseSemaphore(&pixbuf_sema);

#define LOCK_BLIT ObtainSemaphore(&blit_sema);
#define ULOCK_BLIT ReleaseSemaphore(&blit_sema);

struct ETextFont
{
    struct TextFont	etf_Font;
};

struct gfx_driverdata * GetDriverData (struct RastPort * rp)
{
    return (struct gfx_driverdata *) rp->longreserved[0];
}


/* SetDriverData() should only be used when cloning RastPorts           */
/* For other purposes just change the values directly in the struct.	*/

void SetDriverData (struct RastPort * rp, struct gfx_driverdata * DriverData)
{
    rp->longreserved[0] = (IPTR) DriverData;
}


/* InitDriverData() just allocates memory for the struct. To use e.g.   */
/* AreaPtrns, UpdateAreaPtrn() has to allocate the memory for the       */
/* Pattern itself (and free previously used memory!)                    */

struct gfx_driverdata * InitDriverData (struct RastPort * rp, struct GfxBase * GfxBase)
{
    struct gfx_driverdata * dd;
    EnterFunc(bug("InitDriverData(rp=%p)\n", rp));

    /* Does this rastport have a bitmap ? */
    if (rp->BitMap)
    {
        D(bug("Got bitmap\n"));
        /* Displayable ? (== rastport of a screen) */
	if (IS_HIDD_BM(rp->BitMap))
	{
            D(bug("Has HIDD bitmap (displayable)\n"));

	    /* We can use this rastport. Allocate driverdata */
    	    dd = AllocMem (sizeof(struct gfx_driverdata), MEMF_CLEAR);
    	    if (dd)
    	    {
	        struct shared_driverdata *sdd;
		struct TagItem gc_tags[] = {
		    { TAG_DONE, 	0UL} 
		};
		
		
		D(bug("Got driverdata\n"));
		sdd = SDD(GfxBase);
		
		dd->dd_GC = HIDD_Gfx_NewGC(sdd->gfxhidd, gc_tags);
		if (dd->dd_GC)
		{
	
		    D(bug("Got GC HIDD object\n"));
    		    dd->dd_RastPort = rp;
    		    SetDriverData(rp, dd);
    		    rp->Flags |= RPF_DRIVER_INITED;

		    ReturnPtr("InitDriverData", struct gfx_driverdata *, dd);
	        }
		
		FreeMem(dd, sizeof (struct gfx_driverdata));
	
    	    }
	}
    }

    ReturnPtr("InitDriverData", struct gfx_driverdata *, NULL);
}

void DeinitDriverData (struct RastPort * rp, struct GfxBase * GfxBase)
{
    struct gfx_driverdata * dd;
    struct shared_driverdata *sdd;
    
    EnterFunc(bug("DeInitDriverData(rp=%p)\n", rp));
		
    sdd = SDD(GfxBase);

    dd = (struct gfx_driverdata *) rp->longreserved[0];
    rp->longreserved[0] = 0;
    
    HIDD_Gfx_DisposeGC(sdd->gfxhidd, dd->dd_GC);

    FreeMem (dd, sizeof(struct gfx_driverdata));
    rp->Flags &= ~RPF_DRIVER_INITED;
    
    ReturnVoid("DeinitDriverData");
}

BOOL CorrectDriverData (struct RastPort * rp, struct GfxBase * GfxBase)
{
    BOOL retval = TRUE;
    struct gfx_driverdata * dd, * old;
    

    
    if (!rp)
    {
	retval = FALSE;
    }
    else
    {
	old = GetDriverData(rp);
	if (!old)
	{
	    old = InitDriverData(rp, GfxBase);

/* stegerg: ???? would have returned TRUE even if old == NULL
	    if (old)
	    	retval = TRUE;
*/
	    if (!old) retval = FALSE;
	}
	else if (rp != old->dd_RastPort)
	{
	    /* stegerg: cloned rastport?	    
	    ** Make sure to clear driverdata pointer and flag
	    ** in case InitDriverData fail
	    */
	    rp->longreserved[0] = 0;
	    rp->Flags &= ~RPF_DRIVER_INITED;
	    
	    dd = InitDriverData(rp, GfxBase);

/* stegerg: ???? would have returned TRUE even if dd = NULL
	    if (dd)
	   	 retval = TRUE;
*/

	    if (!dd) retval = FALSE;

	}
    }
    
    return retval;
}

BOOL init_romfonts(struct GfxBase *GfxBase)
{
    struct TextFont *tf;
    
    
    tf = AllocMem( sizeof (struct TextFont), MEMF_ANY);
    if (tf)
    {
    	/* Copy the const font struct into allocated mem */
	CopyMem((APTR)&topaz8_tf, tf, sizeof (struct TextFont));
	
	AddFont(tf);
	GfxBase->DefaultFont = tf;
	
	return TRUE;
    }
    return FALSE;
}

int driver_init(struct GfxBase * GfxBase)
{

    EnterFunc(bug("driver_init()\n"));
    
    /* Initialize the semaphore used for the chunky buffer */
    InitSemaphore(&pixbuf_sema);
    InitSemaphore(&blit_sema);
    
    /* Allocate memory for driver data */
    SDD(GfxBase) = (struct shared_driverdata *)AllocMem(sizeof (struct shared_driverdata), MEMF_ANY|MEMF_CLEAR);
    if ( SDD(GfxBase) )
    {
	NEWLIST(&SDD(GfxBase)->dispinfo_db);

        /* Open the OOP library */
	SDD(GfxBase)->oopbase = OpenLibrary(AROSOOP_NAME, 0);
	if ( SDD(GfxBase)->oopbase )
	{
	    /* Init the needed attrbases */
	    
	    if (ObtainAttrBases(attrbases))
	    {
		/* Init the driver's defaultfont */
		if (init_romfonts(GfxBase))
		{
		    pixel_buf=AllocMem(PIXELBUF_SIZE,MEMF_ANY);
		    if (pixel_buf) {
		    
			ReturnInt("driver_init", int, TRUE);
		
			FreeMem(pixel_buf, PIXELBUF_SIZE);
		    }
		}	
		ReleaseAttrBases(attrbases);
	    }
	    CloseLibrary( SDD(GfxBase)->oopbase );
	}
	FreeMem( SDD(GfxBase), sizeof (struct shared_driverdata));
	
    }
    ReturnInt("driver_init", int, FALSE);
}

int driver_open (struct GfxBase * GfxBase)
{
    return TRUE;
}

void driver_close (struct GfxBase * GfxBase)
{
    return;
}

void driver_expunge (struct GfxBase * GfxBase)
{

    ReleaseAttrBases(attrbases);
    
    if ( SDD(GfxBase) )
    {
	    
	/* Try to free some other stuff */
	
	if (SDD(GfxBase)->activescreen_inited)
	    cleanup_activescreen_stuff(GfxBase);

	if (SDD(GfxBase)->dispinfo_db)
	    destroy_dispinfo_db(SDD(GfxBase)->dispinfo_db, GfxBase);
	    
	if (SDD(GfxBase)->queried_modes)
	    HIDD_Gfx_ReleaseGfxModes(SDD(GfxBase)->gfxhidd,
	    	SDD(GfxBase)->queried_modes);

	if ( SDD(GfxBase)->planarbm_cache )
	    delete_object_cache( SDD(GfxBase)->planarbm_cache, GfxBase );
	    
	if ( SDD(GfxBase)->gc_cache )
	    delete_object_cache( SDD(GfxBase)->gc_cache, GfxBase );
	    
	if ( SDD(GfxBase)->gfxhidd )
	    DisposeObject( SDD(GfxBase)->gfxhidd );
	    
        if ( SDD(GfxBase)->oopbase )
	     CloseLibrary( SDD(GfxBase)->oopbase );
	     
	FreeMem( SDD(GfxBase), sizeof (struct shared_driverdata) );
    }
    return;
}

/* Called after DOS is up & running */


BOOL driver_LateGfxInit (APTR data, struct GfxBase *GfxBase)
{

    /* Supplied data is really the librarybase of a HIDD */
    STRPTR gfxhiddname = (STRPTR)data;
    struct TagItem tags[] = {
    	{ aHidd_Gfx_ActiveBMCallBack,		(IPTR)activatebm_callback	},
    	{ aHidd_Gfx_ActiveBMCallBackData,	(IPTR)GfxBase			},
    	{ TAG_DONE, 0UL },
    };    
    
    EnterFunc(bug("driver_LateGfxInit(gfxhiddname=%s)\n", gfxhiddname));
    
    /* Create a new GfxHidd object */
	
    SDD(GfxBase)->gfxhidd = NewObject(NULL, gfxhiddname, tags);
    D(bug("driver_LateGfxInit: gfxhidd=%p\n", SDD(GfxBase)->gfxhidd));
	
    if (NULL != SDD(GfxBase)->gfxhidd) {
    
    	struct TagItem gc_create_tags[] = { { TAG_DONE, 0UL } };
	
	/* Create a GC object cache */
	SDD(GfxBase)->gc_cache = create_object_cache(NULL, IID_Hidd_GC, gc_create_tags, GfxBase);
	if (NULL != SDD(GfxBase)->gc_cache) {

	    struct TagItem bm_create_tags[] = {
#warning Maybe make this attr private and create the object through the graphicshidd	    
	    	{ aHidd_BitMap_GfxHidd,		(IPTR)SDD(GfxBase)->gfxhidd },
	    	{ aHidd_PlanarBM_AllocPlanes,	FALSE },
	    	{ TAG_DONE, 0UL }
	    };
	    
	    SDD(GfxBase)->planarbm_cache
	    	= create_object_cache(NULL, CLID_Hidd_PlanarBM, bm_create_tags, GfxBase);
		
		
	    if (NULL != SDD(GfxBase)->planarbm_cache) {
		struct TagItem qgm_tags[] = {
		    { TAG_DONE, 0UL }	/* Anything goes ! */
		};
	    	
		/* Get all available possible gfxmodes from the HIDD */
		SDD(GfxBase)->queried_modes = HIDD_Gfx_QueryGfxModes(SDD(GfxBase)->gfxhidd, qgm_tags);
		if (NULL != SDD(GfxBase)->queried_modes) {
		
		    /* Move the modes into the displayinfo DB */
		    SDD(GfxBase)->dispinfo_db = build_dispinfo_db(SDD(GfxBase)->queried_modes, GfxBase);
		    if (NULL != SDD(GfxBase)->dispinfo_db) {

	            	SDD(GfxBase)->activescreen_inited = init_activescreen_stuff(GfxBase);
kprintf("ACTIVE_SCREEN_INITED: %d\n", SDD(GfxBase)->activescreen_inited);
		    	if (SDD(GfxBase)->activescreen_inited) {
		
			    ReturnBool("driver_LateGfxInit", TRUE);
		    	}

			destroy_dispinfo_db(SDD(GfxBase)->dispinfo_db, GfxBase);
			SDD(GfxBase)->dispinfo_db = NULL;
		    }
		    
		    HIDD_Gfx_ReleaseGfxModes(SDD(GfxBase)->gfxhidd, SDD(GfxBase)->queried_modes);
	    	    SDD(GfxBase)->queried_modes = NULL;
		}
		
		delete_object_cache(SDD(GfxBase)->planarbm_cache, GfxBase);
	    SDD(GfxBase)->planarbm_cache = NULL;
	    }
	    delete_object_cache(SDD(GfxBase)->gc_cache, GfxBase);
	    SDD(GfxBase)->gc_cache = NULL;
	}

	DisposeObject(SDD(GfxBase)->gfxhidd);
	SDD(GfxBase)->gfxhidd = NULL;
	    
    }
	
    
    ReturnBool("driver_LateGfxInit", FALSE);

}


static VOID clipagainstbitmap(struct BitMap *bm, LONG *x1, LONG *y1, LONG *x2, LONG *y2, struct GfxBase *GfxBase)
{
    ULONG width  = GetBitMapAttr(bm, BMA_WIDTH);
    ULONG height = GetBitMapAttr(bm, BMA_HEIGHT);
    
    /* Clip against bitmap bounds  */
	    
    if (*x1 < 0)  *x1 = 0;
    if (*y1 < 0)  *y1 = 0;

    if (*x2 >= width)  *x2 = width  - 1;
    if (*y2 >= height) *y2 = height - 1; 
    
    return;
}

void driver_SetABPenDrMd (struct RastPort * rp, ULONG apen, ULONG bpen,
	ULONG drmd, struct GfxBase * GfxBase)
{
    struct gfx_driverdata *dd;
    if (!CorrectDriverData (rp, GfxBase))
    	return;
	
	
    dd = GetDriverData(rp);
    if (dd)
    {
    	struct TagItem gc_tags[] = {
    		{ aHidd_GC_Foreground,	0},
    		{ aHidd_GC_Background,	0},
		{ aHidd_GC_ColorExpansionMode, 0UL},
		{ aHidd_GC_DrawMode, vHidd_GC_DrawMode_Copy},
		{ TAG_DONE,	0}
    	};

	gc_tags[0].ti_Data = BM_PIXEL(rp->BitMap, apen & PEN_MASK);
	gc_tags[1].ti_Data = BM_PIXEL(rp->BitMap, bpen & PEN_MASK);
	
	if (drmd & JAM2)
	{
	    gc_tags[2].ti_Data = vHidd_GC_ColExp_Opaque;
	}	
	else if (drmd & COMPLEMENT)
	{
	    gc_tags[3].ti_Data = vHidd_GC_DrawMode_Invert;
	}
	else if ((drmd & (~INVERSVID)) == JAM1)
	{
	    gc_tags[2].ti_Data = vHidd_GC_ColExp_Transparent;
	}
	
    	SetAttrs(dd->dd_GC, gc_tags);
	
    }
    return;
	
    
}

void driver_SetAPen (struct RastPort * rp, ULONG pen,
		    struct GfxBase * GfxBase)
{
    struct gfx_driverdata *dd;

/*    EnterFunc(bug("driver_SetAPen(rp=%p, pen=%d)\n", rp, pen));
*/    if (!CorrectDriverData (rp, GfxBase))
    	return;

    dd = GetDriverData(rp);
    if (dd)
    {
        struct TagItem col_tags[]= {
		{ aHidd_GC_Foreground, 0},
		{ TAG_DONE,	0UL}
	};
	
	col_tags[0].ti_Data = BM_PIXEL(rp->BitMap, pen & PEN_MASK);

	SetAttrs( dd->dd_GC, col_tags );
	
    }
    return;
/*    ReturnVoid("driver_SetAPen");
*/
}

void driver_SetBPen (struct RastPort * rp, ULONG pen,
		    struct GfxBase * GfxBase)
{
    struct gfx_driverdata *dd;
    
    if (CorrectDriverData (rp, GfxBase))
    {
    	
        struct TagItem col_tags[]= {
		{ aHidd_GC_Background, 0},
		{ TAG_DONE,	0UL}
	};

	col_tags[0].ti_Data = BM_PIXEL(rp->BitMap, pen & PEN_MASK);
	
	dd = GetDriverData(rp);
	if (dd)
	{
	    SetAttrs( dd->dd_GC, col_tags );
	}
    }
}

void driver_SetOutlinePen (struct RastPort * rp, ULONG pen,
		    struct GfxBase * GfxBase)
{
}

void driver_SetDrMd (struct RastPort * rp, ULONG mode,
		    struct GfxBase * GfxBase)
{
    struct TagItem drmd_tags[] =
    {
	{ aHidd_GC_ColorExpansionMode, 0UL },
	{ aHidd_GC_DrawMode,	vHidd_GC_DrawMode_Copy },
	{ TAG_DONE, 0UL }
    };
    struct gfx_driverdata *dd;
    
    if (!CorrectDriverData (rp, GfxBase))
    	return;
	
	
    if (mode & JAM2)
    {
    	drmd_tags[0].ti_Data = vHidd_GC_ColExp_Opaque;
    }	
    else if (mode & COMPLEMENT)
    {
	drmd_tags[1].ti_Data = vHidd_GC_DrawMode_Invert;
    }
    else if ((mode & (~INVERSVID)) == JAM1)
    {
    	drmd_tags[0].ti_Data = vHidd_GC_ColExp_Transparent;
    }

#warning Handle INVERSVID by swapping apen and bpen ?

    dd = GetDriverData(rp);
    if (dd)
    {
    	SetAttrs(dd->dd_GC, drmd_tags);
    }
    return;
    	
}



static VOID setbitmappixel(struct BitMap *bm
	, LONG x, LONG y
	, ULONG pen
	, UBYTE depth
	, UBYTE plane_mask)
{
    UBYTE i;
    ULONG idx;
    UBYTE mask, clr_mask;
    ULONG penmask;

    idx = COORD_TO_BYTEIDX(x, y, bm->BytesPerRow);

    mask = XCOORD_TO_MASK( x );
    clr_mask = ~mask;
    
    penmask = 1;
    for (i = 0; i < depth; i ++)
    {

	if ((1L << i) & plane_mask)
	{
            UBYTE *plane = bm->Planes[i];
	
	    if ((plane != NULL) && (plane != (PLANEPTR)-1))
	    {
		if ((penmask & pen) != 0)
		    plane[idx] |=  mask;
		else
		    plane[idx] &=  clr_mask;
            }

	}
	penmask <<= 1;
	
    }
    return;
}

static ULONG getbitmappixel(struct BitMap *bm
	, LONG x
	, LONG y
	, UBYTE depth
	, UBYTE plane_mask)
{
    UBYTE i;
    ULONG idx;

    ULONG mask;
    ULONG pen = 0L;
    
    idx = COORD_TO_BYTEIDX(x, y, bm->BytesPerRow);
    mask = XCOORD_TO_MASK( x );
    
    for (i = depth - 1; depth; i -- , depth -- )
    {
        pen <<= 1; /* stegerg: moved to here, was inside if!? */

        if ((1L << i) & plane_mask)
	{
	    UBYTE *plane = bm->Planes[i];
	
	    if (plane == (PLANEPTR)-1)
	    {
	        pen |= 1;
	    }
	    else if (plane != NULL)
	    {
		if ((plane[idx] & mask) != 0)
		    pen |= 1;
	    }
	}

    }
    return pen;
}


enum { SB_SINGLEMASK, SB_PREPOSTMASK, SB_FULL };
static VOID setbitmapfast(struct BitMap *bm, LONG x_start, LONG y_start, LONG xsize, LONG ysize, ULONG pen)
{
    LONG num_whole;
    UBYTE sb_type;
    
    UBYTE plane;
    UBYTE pre_pixels_to_set,
    	  post_pixels_to_set,
	  pre_and_post; /* number pixels to clear in pre and post byte */
 
    UBYTE prebyte_mask, postbyte_mask;
    
/*    kprintf("x_start: %d, y_start: %d, xsize: %d, ysize: %d, pen: %d\n",
    	x_start, y_start, xsize, ysize, pen);
*/	

    pre_pixels_to_set  = (7 - (x_start & 0x07)) + 1;
    post_pixels_to_set = ((x_start + xsize - 1) & 0x07) + 1;
    

    pre_and_post = pre_pixels_to_set + post_pixels_to_set;
    
    if (pre_and_post > xsize)
    {
	UBYTE start_bit, stop_bit;
	/* Check whether the pixels are kept within a byte */
	sb_type = SB_SINGLEMASK;
    	pre_pixels_to_set  = MIN(pre_pixels_to_set,  xsize);
	
	/* Mask out the needed bits */
	start_bit =  7 - (x_start & 0x07) + 1;
	stop_bit = 7 - ((x_start + xsize - 1) & 0x07);

/* kprintf("start_bit: %d, stop_bit: %d\n", start_bit, stop_bit);
*/	
	prebyte_mask = ((1L << start_bit) - 1) - ((1L << stop_bit) - 1) ;
/* kprintf("prebyte_mask: %d\n", prebyte_mask);

kprintf("SB_SINGLE\n");
*/
    }
    else if (pre_and_post == xsize)
    {
    	/* We have bytes within to neighbour pixels */
	sb_type = SB_PREPOSTMASK;
	prebyte_mask  = 0xFF >> (8 - pre_pixels_to_set);
	postbyte_mask = 0xFF << (8 - post_pixels_to_set);
    
/* kprintf("SB_PREPOSTMASK\n");
*/
    }
    else
    {

	/* Say we want to clear two pixels in last byte. We want the mask
	MSB 00000011 LSB
	*/
	sb_type = SB_FULL;
	prebyte_mask = 0xFF >> (8 - pre_pixels_to_set);
    
	/* Say we want to set two pixels in last byte. We want the mask
	MSB 11000000 LSB
	*/
	postbyte_mask = 0xFF << (8 - post_pixels_to_set);
	
        	/* We have at least one whole byte of pixels */
	num_whole = xsize - pre_pixels_to_set - post_pixels_to_set;
	num_whole >>= 3; /* number of bytes */
	
/* kprintf("SB_FULL\n");
*/
    }
	
/*
kprintf("pre_pixels_to_set: %d, post_pixels_to_set: %d, numwhole: %d\n"
	, pre_pixels_to_set, post_pixels_to_set, num_whole);
    
kprintf("prebyte_mask: %d, postbyte_mask: %d, numwhole: %d\n", prebyte_mask, postbyte_mask, num_whole);
*/    
    for (plane = 0; plane < GetBitMapAttr(bm, BMA_DEPTH); plane ++)
    {
    
        LONG y;
	UBYTE pixvals;
	UBYTE prepixels_set, prepixels_clr;
	UBYTE postpixels_set, postpixels_clr;
    	UBYTE *curbyte = ((UBYTE *)bm->Planes[plane]) + COORD_TO_BYTEIDX(x_start, y_start, bm->BytesPerRow);
	
	
	/* Set or clear current bit of pixval ? */
	if (pen & (1L << plane))
	    pixvals = 0xFF;
	else
	    pixvals = 0x00;
	
	/* Set the pre and postpixels */
	switch (sb_type)
	{
	    case SB_FULL:
		prepixels_set  = (pixvals & prebyte_mask);
		postpixels_set = (pixvals & postbyte_mask);
	

		prepixels_clr  = (pixvals & prebyte_mask)  | (~prebyte_mask);
		postpixels_clr = (pixvals & postbyte_mask) | (~postbyte_mask);

		for (y = 0; y < ysize; y ++)
		{
		    LONG x;
		    UBYTE *ptr = curbyte;
	    
		    *ptr |= prepixels_set;
		    *ptr ++ &= prepixels_clr;
	    
		    for (x = 0; x < num_whole; x ++)
		    {
			*ptr ++ = pixvals;
		    }
		    /* Clear the last nonwhole byte */
		    *ptr |= postpixels_set;
		    *ptr ++ &= postpixels_clr;
	    
		    /* Go to next line */
		    curbyte += bm->BytesPerRow;
		}
		break;
		
	    case SB_PREPOSTMASK:
	
		prepixels_set  = (pixvals & prebyte_mask);
		postpixels_set = (pixvals & postbyte_mask);
	

		prepixels_clr  = (pixvals & prebyte_mask)  | (~prebyte_mask);
		postpixels_clr = (pixvals & postbyte_mask) | (~postbyte_mask);

		for (y = 0; y < ysize; y ++)
		{
		    UBYTE *ptr = curbyte;
	    
		    *ptr |= prepixels_set;
		    *ptr ++ &= prepixels_clr;
	    
		    /* Clear the last nonwhole byte */
		    *ptr |= postpixels_set;
		    *ptr ++ &= postpixels_clr;
	    
		    /* Go to next line */
		    curbyte += bm->BytesPerRow;
		}
		break;
		
	    case SB_SINGLEMASK:
	
		prepixels_set  = (pixvals & prebyte_mask);
		prepixels_clr  = (pixvals & prebyte_mask) | (~prebyte_mask);

		for (y = 0; y < ysize; y ++)
		{
		    UBYTE *ptr = curbyte;
	    
		    *ptr |= prepixels_set;
		    *ptr ++ &= prepixels_clr;
	    
		    /* Go to next line */
		    curbyte += bm->BytesPerRow;
		}
		break;
		
	} /* switch() */
    }
    return;
    
}




struct fillrect_data {
    ULONG dummy;
};

static ULONG fillrect_render(APTR funcdata
	, LONG srcx, LONG srcy
	, Object *dstbm_obj
	, Object *dst_gc
	, LONG x1, LONG y1, LONG x2, LONG y2
	, struct GfxBase *GfxBase)
{
    HIDD_BM_FillRect(dstbm_obj, dst_gc, x1, y1, x2, y2);
    
    return (x2 - x1 + 1) * (y2 - y1 + 1);
}


static LONG fillrect_pendrmd(struct RastPort *rp
	, LONG x1, LONG y1
	, LONG x2, LONG y2
	, HIDDT_Pixel pix
	, HIDDT_DrawMode drmd
	, struct GfxBase *GfxBase)
{
    
    LONG pixwritten = 0;
    
    HIDDT_DrawMode old_drmd;
    HIDDT_Pixel old_fg;
    Object *gc;
    struct Rectangle rr;

    struct TagItem gc_tags[] =
    {
	{ aHidd_GC_DrawMode, drmd },
	{ aHidd_GC_Foreground, pix },
	{ TAG_DONE, 0}
    };
    
    
    if (!CorrectDriverData (rp, GfxBase))
	return 0;
	
    gc = GetDriverData(rp)->dd_GC;
	
    GetAttr(gc, aHidd_GC_DrawMode,	(IPTR *)&old_drmd);
    GetAttr(gc, aHidd_GC_Foreground,	(IPTR *)&old_fg);
    
    SetAttrs(gc, gc_tags);
    
    rr.MinX = x1;
    rr.MinY = y1;
    rr.MaxX = x2;
    rr.MaxY = y2;
    
    pixwritten = do_render_func(rp, NULL, &rr, fillrect_render, NULL, FALSE, GfxBase);
    
    /* Restore old GC values */
    gc_tags[0].ti_Data = (IPTR)old_drmd;
    gc_tags[1].ti_Data = (IPTR)old_fg;
    SetAttrs(gc, gc_tags);
    
	
    return pixwritten;
}

void driver_RectFill (struct RastPort * rp, LONG x1, LONG y1, LONG x2, LONG y2,
		    struct GfxBase * GfxBase)
{
    
    
    struct BitMap *bm = rp->BitMap;
    UBYTE rp_drmd;
    
    HIDDT_Pixel pix;
    HIDDT_DrawMode drmd;
    ULONG pen;

    /* Get drawmode */
    rp_drmd = GetDrMd(rp);
    
    pen = (rp_drmd & INVERSVID ? GetBPen(rp) : GetAPen(rp));
    
    /* Get rectfill pixel */
    
    pix = BM_PIXEL(bm, pen);
    
    
    if (rp_drmd & JAM2)
    {
    	drmd = vHidd_GC_DrawMode_Copy;
    }
    else if (rp_drmd & COMPLEMENT)
    {
    	drmd = vHidd_GC_DrawMode_Invert;
    }
    else if ((rp_drmd & (~INVERSVID)) == JAM1)
    {
    	drmd = vHidd_GC_DrawMode_Copy;
    }

    fillrect_pendrmd(rp, x1, y1, x2, y2, pix, drmd, GfxBase);
	
    ReturnVoid("driver_RectFill");
}



struct bitmap_render_data {
    struct render_special_info rsi;
    ULONG minterm;
    struct BitMap *srcbm;
    Object *srcbm_obj;
    
};

static ULONG bitmap_render(APTR bitmap_rd, LONG srcx, LONG srcy
	, Object *dstbm_obj, Object *dst_gc
	, LONG x1, LONG y1, LONG x2, LONG y2
	, struct GfxBase *GfxBase)
{
    struct bitmap_render_data *brd;
    ULONG width, height;
    
    width  = x2 - x1 + 1;
    height = y2 - y1 + 1;
    
    
    brd = (struct bitmap_render_data *)bitmap_rd;
    
kprintf("bitmap_render(%p, %d, %d, %p, %p, %d, %d, %d, %d, %p)\n"
	, bitmap_rd, srcx, srcy, dstbm_obj, dst_gc, x1, y1, x2, y2, GfxBase);
	
	
    /* Get some info on the colormaps. We have to make sure
       that we have the apropriate mapping tables set.
    */
    
    if (!int_bltbitmap(brd->srcbm
    	, brd->srcbm_obj
	, srcx, srcy
	, brd->rsi.curbm
	, dstbm_obj
	, x1, y1
	, x2 - x1 + 1, y2 - y1 + 1
	, brd->minterm
	, dst_gc
	, GfxBase
    ))
    	return 0;

#if 0    
    
    HIDD_BM_CopyBox(brd->srcBM, dst_gc
    	, srcx, srcy
	, dstbm_obj
	, x1, y1
	, width, height
   );
#endif   
   return width * height;
}

void driver_BltBitMapRastPort (struct BitMap   * srcBitMap,
	LONG xSrc, LONG ySrc,
	struct RastPort * destRP,
	LONG xDest, LONG yDest,
	LONG xSize, LONG ySize,
	ULONG minterm, struct GfxBase *GfxBase
)
{
    struct bitmap_render_data brd;
    struct Rectangle rr;
    HIDDT_DrawMode old_drmd;
    Object *gc;
    
    Point src;
    
    struct TagItem gc_tags[] = {
    	{ aHidd_GC_DrawMode,	0UL },
	{ TAG_DONE, 0UL }
    };
    
    EnterFunc(bug("driver_BltBitMapRastPort(%d %d %d, %d, %d, %d)\n"
    	, xSrc, ySrc, xDest, yDest, xSize, ySize));
	
    if (!CorrectDriverData(destRP, GfxBase))
    	return;
	
    brd.minterm	= minterm;
    brd.srcbm_obj = OBTAIN_HIDD_BM(srcBitMap);
    if (NULL == brd.srcbm_obj)
    	return;
	
    brd.srcbm = srcBitMap;

    gc = GetDriverData(destRP)->dd_GC;
    GetAttr(gc, aHidd_GC_DrawMode, &old_drmd);
    
    gc_tags[0].ti_Data = MINTERM_TO_GCDRMD(minterm);
    SetAttrs(gc, gc_tags);

    rr.MinX = xDest;
    rr.MinY = yDest;
    rr.MaxX = xDest + xSize - 1;
    rr.MaxY = yDest + ySize - 1;
    
    src.x = xSrc;
    src.y = ySrc;
    
    
    do_render_func(destRP, &src, &rr, bitmap_render, &brd, TRUE, GfxBase);
    
    RELEASE_HIDD_BM(brd.srcbm_obj, srcBitMap);
    
    gc_tags[0].ti_Data = old_drmd;
    SetAttrs(gc, gc_tags);
    
	
    ReturnVoid("driver_BltBitMapRastPort");
}


BOOL driver_MoveRaster (struct RastPort * rp, LONG dx, LONG dy,
	LONG x1, LONG y1, LONG x2, LONG y2, BOOL UpdateDamageList,
	BOOL hasClipRegion,
	struct GfxBase * GfxBase)
{
  LONG xCorr1, xCorr2, yCorr1, yCorr2, absdx, absdy, xBlockSize, yBlockSize;
  struct BitMap * bm;
  struct Layer * L = rp->Layer;
  ULONG AllocFlag;
  
  if (!CorrectDriverData (rp, GfxBase))
    return FALSE;
    
  if (0 == dx && 0 == dy)
    return TRUE;
  
  if (dx >= 0)
    absdx = dx;
  else
    absdx = -dx;
    
  if (dy >= 0)
    absdy = dy;
  else
    absdy = -dy;
  
  xBlockSize = x2 - x1 + 1 - absdx;  
  yBlockSize = y2 - y1 + 1 - absdy;
  /* 
    It is assumed that (x1,y1)-(x2,y2) describe a valid rectangle
    within the rastport. So any corrections have to be done by
    ScrollRaster() and ScrollRasterBF() 
  */
  /*
    There are four different cases to consider:
    1) rastport without layer (a screen's rastport)
    2) rastport with simple-refresh layer 
    3) rastport with smart -refresh layer
    4) rastport with superbitmap layer
    
    All cases need a TmpRas. If it's not there it will be allocated by
    this function.
  */
  
  /* Only if the rastport has an incomplete list of rectangles
     (because of a clipregion) then it is necessary to
     clear the bitmap when allocating it 
  */
  if (TRUE == hasClipRegion)
    AllocFlag = BMF_CLEAR;
  else
    AllocFlag = 0;
  
  bm = AllocBitMap(xBlockSize,
                   yBlockSize,
                   GetBitMapAttr(rp->BitMap,BMA_DEPTH),
                   AllocFlag,
                   rp->BitMap);
  if (NULL == bm)
    return FALSE;

  if (dx < 0) 
  {
    /* move it to the right */
    xCorr1 = 0;
    xCorr2 = dx;
  }
  else
  {
    /* move it to the left */
    xCorr1 = dx;
    xCorr2 = 0;
  }
  
  if (dy < 0)
  {
    /* move it down */
    yCorr1 = 0;
    yCorr2 = dy;
  }
  else
  {
    /* move it up */
    yCorr1 = dy;
    yCorr2 = 0;
  }
  
  /* must not change x2,x1,y2,y1 here!!! */
  /* scroll the stuff */

  if (NULL == L)
  {
    /* no layer, so this is a screen */
    /* 
       I am copying all the data into the temporary bitmap and then
       copy them back to the old location
    */
    BltBitMap(rp->BitMap,
              x1+xCorr1,
              y1+yCorr1,
              bm,
              0,
              0,
              xBlockSize,
              yBlockSize,
              0x0c0, /* copy */
              0xff,
              NULL );        
    
    /* copy it to the new location */
    BltBitMap(bm,
              0,
              0,
              rp->BitMap,
              x1-xCorr2,
              y1-yCorr2,
              xBlockSize,
              yBlockSize,
              0x0c0,
              0x0ff,
              NULL );
    /* no need to worry about damage lists here */
  }
  else
  {
    LONG xleft, xright, yup, ydown;
    /* rastport with layer - yuk! */
    /* 
       I will walk through all the layer's cliprects and copy the
       necessary parts to the temporary bitmap and if it's a simple
       layer I will also collect those parts that come out of a 
       hidden cliprect and add them to a region. Then I will manipulate
       the region and move it by (dx,dy) towards (0,0) and then 
       subtract all areas within the rectangle that are in hidden
       cliprects when copying the data back into the layer's cliprects. 
       The result of this will be a region corresponding to the damage 
       created by scrolling areas that were hidden into visible areas.
       I will add this region to the damage list and set the 
       LAYERREFRESH flag.
    */
    struct ClipRect * CR = L->ClipRect;
    struct Region * R;
    struct Rectangle Rect;
    struct Rectangle ScrollRect;
     
    LockLayerRom(L);
     
    if (0 != (L->Flags & LAYERSIMPLE) &&
        TRUE == UpdateDamageList)
    {
      /* 
         This region should also contain the current damage list in that
         area.
         Region R = damagelist AND scrollarea
      */
      if (NULL == (R = copyregion(L->DamageList, GfxBase)))
      { 
        /* not enough memory */
        DisposeRegion(R);
        UnlockLayerRom(L);
        goto failexit;
      }
      
      ScrollRect.MinX = x1;
      ScrollRect.MinY = y1;
      ScrollRect.MaxX = x2;
      ScrollRect.MaxY = y2;
      
      /* this cannot fail */
      AndRectRegion(R, &ScrollRect);
      /* Now Region R contains the damage that is already there before
         the area has been scrolled at all.
       */

      /* This area must also be cleared from the current damage list. But
         I do this later as I might run out of memory here somewhere in
         between...
      */    
    }  
    
    /* adapt x1,x2,y1,y2 to the cliprect coordinates */

    /* (xleft,yup)(xright,ydown) defines the rectangle to copy out of */

    xleft  = x1 + L->bounds.MinX + xCorr1;
    xright = x2 + L->bounds.MinX - xCorr2; /* stegerg: was + xCorr2; */
    yup    = y1 + L->bounds.MinY + yCorr1;
    ydown  = y2 + L->bounds.MinY - yCorr2; /* stegerg: was + yCorr2; */

    /* First read all data out of the source area */    
    while (NULL != CR)
    {
      /* Is this a CR to be concerned about at all??? */
      if (!(CR->bounds.MinX > xright ||
            CR->bounds.MaxX < xleft  ||
            CR->bounds.MinY > ydown  ||
            CR->bounds.MaxY < yup))
      {
        /* that is one to be at least partly concerned about */
        /* 
           first determine the area to copy out of this cliprect
           into the destination bitmap
        */
        if (CR->bounds.MinX > xleft)
          Rect.MinX = CR->bounds.MinX;
        else
          Rect.MinX = xleft;
          
        if (CR->bounds.MaxX < xright)
          Rect.MaxX = CR->bounds.MaxX;
        else
          Rect.MaxX = xright;
          
        if (CR->bounds.MinY > yup)
          Rect.MinY = CR->bounds.MinY;
        else
          Rect.MinY = yup;
          
        if (CR->bounds.MaxY < ydown)
          Rect.MaxY = CR->bounds.MaxY;
        else
          Rect.MaxY = ydown;
          
        /* If this cliprect is hidden and belongs to a simple layer then
           I add the Rect to the Region, otherwise I do a blit into the
           temporary bitmap instead 
        */
        if (NULL != CR->lobs && 
            0 != (L->Flags & LAYERSIMPLE) &&
            TRUE == UpdateDamageList)
        { 
	  /* stegerg: damage Region coords are relative to layer!! */
	  struct Rectangle Rect2;

	  Rect2.MinX = Rect.MinX - L->bounds.MinX;
	  Rect2.MinY = Rect.MinY - L->bounds.MinY;
	  Rect2.MaxX = Rect.MaxX - L->bounds.MinX;
	  Rect2.MaxY = Rect.MaxY - L->bounds.MinY;

	  D(bug("adding (%d,%d) - (%d,%d)\n",Rect2.MinX,Rect2.MinY,Rect2.MaxX,Rect2.MaxY));
	  
          if (FALSE == OrRectRegion(R, &Rect2))
          {
            /* there's still time to undo this operation */
            DisposeRegion(R);
            UnlockLayerRom(L);
            goto failexit;
          }
          /* also clear that area in the destination bitmap */
          BltBitMap(bm,
                    0,
                    0,
                    bm,
                    Rect.MinX - xleft,
                    Rect.MinY - yup,
                    Rect.MaxX - Rect.MinX + 1,
                    Rect.MaxY - Rect.MinY + 1,
                    0x000, /* clear */
                    0xff,
                    NULL);
        }
        else
        {
          /* Do the blit into the temporary bitmap */
          /* several cases:
             - non hidden area of superbitmap layer, simple layer, smart layer
                -> can all be treated equally
             - hidden area of a superbitmap layer
             - hidden area of simple layer 
          */
          if (NULL != CR->lobs)
          {
            /* Hidden */
            if (0 == (L->Flags & LAYERSUPER))
            {
              /* a smart layer */
              BltBitMap(CR->BitMap,
                        Rect.MinX - CR->bounds.MinX + ALIGN_OFFSET(CR->bounds.MinX),
                        Rect.MinY - CR->bounds.MinY,
                        bm,
                        Rect.MinX - xleft,
                        Rect.MinY - yup,
                        Rect.MaxX-Rect.MinX+1,
                        Rect.MaxY-Rect.MinY+1,
                        0x0c0,
                        0xFF,
                        NULL);
            }
            else
            {
              /* a superbitmap layer */
              BltBitMap(CR->BitMap,
                        Rect.MinX - CR->bounds.MinX - L->Scroll_X,
                        Rect.MinY - CR->bounds.MinY - L->Scroll_Y,
                        bm,
                        Rect.MinX - xleft,
                        Rect.MinY - yup,
                        Rect.MaxX-Rect.MinX+1,
                        Rect.MaxY-Rect.MinY+1,
                        0x0c0,
                        0xFF,
                        NULL);
            } /* if .. else .. */
          }
          else
          {
            /* copy it out of the rastport's bitmap */
            BltBitMap(rp->BitMap,
                      Rect.MinX,
                      Rect.MinY,
                      bm,
                      Rect.MinX - xleft,
                      Rect.MinY - yup,
                      Rect.MaxX - Rect.MinX + 1,
                      Rect.MaxY - Rect.MinY + 1,
                      0x0c0,
                      0xff,
                      NULL);            
          } /* if .. else  .. */
        } /* if .. else .. */
      } /* if (ClipRect is to be considered at all) */  

      CR = CR->Next;
    } /* while */

    /* (xleft,yup)(xright,ydown) defines the rectangle to copy into */

    
    /* Move the region, if it's a simple layer  */
    if (0 != (L->Flags & LAYERSIMPLE) &&
        TRUE == UpdateDamageList)
    {
      /* the following could possibly lead to problems when the region
         R is moved to negative coordinates. Maybe and maybe not.
      */
      R->bounds.MinX -= dx;
      R->bounds.MinY -= dy;
      R->bounds.MaxX -= dx;
      R->bounds.MaxY -= dy;

      /* make this region valid again */
      AndRectRegion(R, &ScrollRect);

    }

    xleft  -= dx;
    xright -= dx;
    yup    -= dy;
    ydown  -= dy;
    
    /* now copy from the bitmap bm into the destination */
    CR = L->ClipRect;
    while (NULL != CR)
    {
      /* Is this a CR to be concerned about at all??? */
      if (!(CR->bounds.MinX > xright ||
            CR->bounds.MaxX < xleft  ||
            CR->bounds.MinY > ydown  ||
            CR->bounds.MaxY < yup))
      {
        struct Rectangle Rect;
        /* that is one to be at least partly concerned about */
        /* 
           first determine the area to copy into from the
           temporary bitmap
        */
        if (CR->bounds.MinX > xleft)
          Rect.MinX = CR->bounds.MinX;
        else
          Rect.MinX = xleft;
          
        if (CR->bounds.MaxX < xright)
          Rect.MaxX = CR->bounds.MaxX;
        else
          Rect.MaxX = xright;
          
        if (CR->bounds.MinY > yup)
          Rect.MinY = CR->bounds.MinY;
        else
          Rect.MinY = yup;
          
        if (CR->bounds.MaxY < ydown)
          Rect.MaxY = CR->bounds.MaxY;
        else
          Rect.MaxY = ydown;
          
        /* 
           If this cliprect is hidden and belongs to a simple layer then
           I subtract the Rect from the Region, otherwise I do a blit 
           from the temporary bitmap instead 
        */
        if (NULL != CR->lobs && 
            0 != (L->Flags & LAYERSIMPLE) &&
            TRUE == UpdateDamageList)
        {
	  /* stegerg: damage Region coords are relative to layer!! */
	  struct Rectangle Rect2;
	  
	  Rect2.MinX = Rect.MinX - L->bounds.MinX;
	  Rect2.MinY = Rect.MinY - L->bounds.MinY;
	  Rect2.MaxX = Rect.MaxX - L->bounds.MinX;
	  Rect2.MaxY = Rect.MaxY - L->bounds.MinY;
	  
	  D(bug("clearing (%d,%d) - (%d,%d)\n",Rect2.MinX,Rect2.MinY,Rect2.MaxX,Rect2.MaxY));
          ClearRectRegion(R, &Rect2);
        }
        else
        {
          /* Do the blit from the temporary bitmap */
          /* several cases:
             - non hidden area of superbitmap layer, simple layer, smart layer
                -> can all be treated equally
             - hidden area of a superbitmap layer
             - hidden area of simple layer 
          */
          if (NULL != CR->lobs)
          {
            /* Hidden */
            if (0 == (L->Flags & LAYERSUPER))
            {
              /* a smart layer */
              BltBitMap(bm,
                        Rect.MinX - xleft,
                        Rect.MinY - yup,
                        CR->BitMap,
                        Rect.MinX - CR->bounds.MinX + ALIGN_OFFSET(CR->bounds.MinX),
                        Rect.MinY - CR->bounds.MinY,
                        Rect.MaxX-Rect.MinX+1,
                        Rect.MaxY-Rect.MinY+1,
                        0x0c0,
                        0xff,
                        NULL);
                        
            }
            else
            {
              /* a superbitmap layer */
              BltBitMap(bm,
                        Rect.MinX - xleft,
                        Rect.MinY - yup,
                        CR->BitMap,
                        Rect.MinX - L->bounds.MinX - L->Scroll_X,
                        Rect.MinY - L->bounds.MinY - L->Scroll_Y,
                        Rect.MaxX - Rect.MinX + 1,
                        Rect.MaxY - Rect.MinY + 1,
                        0x0c0,
                        0xff,
                        NULL);
            }
          }
          else
          {
            /* blit it into the bitmap of the screen */
            BltBitMap(bm,
                      Rect.MinX - xleft,
                      Rect.MinY - yup,
                      rp->BitMap,
                      Rect.MinX,
                      Rect.MinY,
                      Rect.MaxX - Rect.MinX + 1,
                      Rect.MaxY - Rect.MinY + 1,
                      0x0c0,
                      0xff,
                      NULL);
          }  
          
        } /* if .. else .. */
        
      }
      CR = CR->Next;
    }
    
    
    if (0 != (L->Flags & LAYERSIMPLE) && TRUE == UpdateDamageList)
    {
      /* Add the damagelist to the layer's damage list and set the
         LAYERREFRESH flag, but of course only if it's necessary */

      /* first clear the damage lists of the scrolled area */
      ClearRectRegion(L->DamageList, &ScrollRect);

      if (NULL != R->RegionRectangle)
      {
        OrRegionRegion(R, L->DamageList);
        L->Flags |= LAYERREFRESH;
      }
      DisposeRegion(R);
    }
    
    UnlockLayerRom(L); 
  }  

failexit:
  FreeBitMap(bm);  
  
  return TRUE;
  
}



void driver_Draw( struct RastPort *rp, LONG x, LONG y, struct GfxBase  *GfxBase)
{
    
    struct Rectangle rr;
    Object *gc;
    struct Layer *L = rp->Layer;
    struct BitMap *bm = rp->BitMap;
    
    if (!CorrectDriverData (rp, GfxBase))
	return;
	
    gc = GetDriverData(rp)->dd_GC;

    if (rp->cp_x > x) {
	rr.MinX = x;
	rr.MaxX = rp->cp_x;
    } else {
    	rr.MinX = rp->cp_x;
	rr.MaxX = x;
    }
    
    if (rp->cp_y > y) {
	rr.MinY = y;
	rr.MaxY = rp->cp_y;
    } else {
    	rr.MinY = rp->cp_y;
	rr.MaxY = y;
    }
    
    if (NULL == L)
    {
        /* No layer, probably a screen, but may be a user inited bitmap */
	Object *bm_obj;
	

	bm_obj = OBTAIN_HIDD_BM(bm);
	if (NULL == bm_obj)
	    return;
	    
	/* No need for clipping */
	HIDD_BM_DrawLine(bm_obj, gc, rp->cp_x, rp->cp_y, x, y);  
	
	
	RELEASE_HIDD_BM(bm_obj, bm);
	    
    }
    else
    {
        struct ClipRect *CR;
	WORD xrel;
        WORD yrel;
	struct Rectangle torender, intersect;
	
	LockLayerRom(L);
	
	xrel = L->bounds.MinX;
	yrel = L->bounds.MinY;
	
	
	torender.MinX = rr.MinX + xrel;
	torender.MinY = rr.MinY + yrel;
	torender.MaxX = rr.MaxX + xrel;
	torender.MaxY = rr.MaxY + yrel;
	
	
	CR = L->ClipRect;
	
	for (;NULL != CR; CR = CR->Next)
	{
	    D(bug("Cliprect (%d, %d, %d, %d), lobs=%p\n",
	    	CR->bounds.MinX, CR->bounds.MinY, CR->bounds.MaxX, CR->bounds.MaxY,
		CR->lobs));
		
	    /* Does this cliprect intersect with area to rectfill ? */
	    if (andrectrect(&CR->bounds, &torender, &intersect))
	    {
	    	LONG xoffset, yoffset;
		LONG layer_rel_x, layer_rel_y;
		
		xoffset = intersect.MinX - torender.MinX;
		yoffset = intersect.MinY - torender.MinY;
		
		layer_rel_x = intersect.MinX - L->bounds.MinX;
		layer_rel_y = intersect.MinY - L->bounds.MinY;
		
		
		
	        if (NULL == CR->lobs)
		{
		
		    /* Set clip rectangle */
		    HIDD_GC_SetClipRect(gc
		    	, intersect.MinX
			, intersect.MinY
			, intersect.MaxX
			, intersect.MaxY
		    );
		    
		    HIDD_BM_DrawLine(HIDD_BM_OBJ(bm)
		    	, gc
			, rp->cp_x + xrel
			, rp->cp_y + yrel
			, x + xrel
			, y + yrel
		    );
		    
		    HIDD_GC_UnsetClipRect(gc);
		
		}
		else
		{
		    /* Render into offscreen cliprect bitmap */
		    if (L->Flags & LAYERSIMPLE)
		    	continue;
		    else if (L->Flags & LAYERSUPER)
		    	kprintf("do_render_func(): Superbitmap not handled yet\n");
		    else
		    {
		    	LONG bm_rel_minx, bm_rel_miny, bm_rel_maxx, bm_rel_maxy;
			LONG layer_rel_x, layer_rel_y;
			
			layer_rel_x = intersect.MinX - xrel;
			layer_rel_y = intersect.MinY - yrel;
			
			bm_rel_minx = intersect.MinX - CR->bounds.MinX;
			bm_rel_miny = intersect.MinY - CR->bounds.MinY;
			bm_rel_maxx = intersect.MaxX - CR->bounds.MinX;
			bm_rel_maxy = intersect.MaxY - CR->bounds.MinY;

		    	HIDD_GC_SetClipRect(gc
		    		, bm_rel_minx + ALIGN_OFFSET(CR->bounds.MinX)
				, bm_rel_miny
				, bm_rel_maxx + ALIGN_OFFSET(CR->bounds.MinX) 
				, bm_rel_maxy
			);
			
			HIDD_BM_DrawLine(HIDD_BM_OBJ(CR->BitMap)
				, gc
				, bm_rel_minx - (layer_rel_x - rp->cp_x) + ALIGN_OFFSET(CR->bounds.MinX)
				, bm_rel_miny - (layer_rel_y - rp->cp_y)
				, bm_rel_minx - (layer_rel_x - x) + ALIGN_OFFSET(CR->bounds.MinX)
				, bm_rel_miny - (layer_rel_y - y)
			);
				
			HIDD_GC_UnsetClipRect(gc);
		    }
		    
		} /* if (CR->lobs == NULL) */
		
	    } /* if (cliprect intersects with area to render into) */
	    
	} /* for (each cliprect in the layer) */
	
        UnlockLayerRom(L);
    } /* if (rp->Layer) */
    
    rp->cp_x = x;
    rp->cp_y = y;
    
    return;
}

void driver_DrawEllipse (struct RastPort * rp, LONG center_x, LONG center_y, LONG rx, LONG ry,
		struct GfxBase * GfxBase)
{

    struct Rectangle rr;
    Object *gc;
    struct Layer *L = rp->Layer;
    struct BitMap *bm = rp->BitMap;
    
    if (!CorrectDriverData (rp, GfxBase))
	return;
/* kprintf("driver_DrawEllipse(%d %d %d %d)\n", center_x, center_y, rx, ry);	
*/    gc = GetDriverData(rp)->dd_GC;
    
    rr.MinX = center_x - rx;
    rr.MinY = center_y - ry;
    rr.MaxX = center_x + rx;
    rr.MaxY = center_y + ry;

    
    if (NULL == L)
    {
        /* No layer, probably a screen, but may be a user inited bitmap */
	Object *bm_obj;
	
	bm_obj = OBTAIN_HIDD_BM(bm);
	if (NULL == bm_obj)
	    return;
	    
	/* No need for clipping */
	HIDD_BM_DrawEllipse(bm_obj, gc
		, center_x, center_y
		, rx, ry
	);
	
	RELEASE_HIDD_BM(bm_obj, bm);
	    
    } else {
        struct ClipRect *CR;
	WORD xrel;
        WORD yrel;
	struct Rectangle torender, intersect;
	
	LockLayerRom(L);
	
	CR = L->ClipRect;
	
	xrel = L->bounds.MinX;
	yrel = L->bounds.MinY;
	
	torender.MinX = rr.MinX + xrel;
	torender.MinY = rr.MinY + yrel;
	torender.MaxX = rr.MaxX + xrel;
	torender.MaxY = rr.MaxY + yrel;
	
	
	for (;NULL != CR; CR = CR->Next)
	{
	    D(bug("Cliprect (%d, %d, %d, %d), lobs=%p\n",
	    	CR->bounds.MinX, CR->bounds.MinY, CR->bounds.MaxX, CR->bounds.MaxY,
		CR->lobs));
		
	    /* Does this cliprect intersect with area to rectfill ? */
	    if (andrectrect(&CR->bounds, &torender, &intersect))
	    {
	    	LONG xoffset, yoffset;
		LONG layer_rel_x, layer_rel_y;
		
		xoffset = intersect.MinX - torender.MinX;
		yoffset = intersect.MinY - torender.MinY;
		
		layer_rel_x = intersect.MinX - L->bounds.MinX;
		layer_rel_y = intersect.MinY - L->bounds.MinY;

	        if (NULL == CR->lobs)
		{
		
		    /* Set clip rectangle */
/* kprintf("Setting cliprect: %d %d %d %d : layerrel: %d %d %d %d\n"
		    	, intersect.MinX
			, intersect.MinY
			, intersect.MaxX
			, intersect.MaxY
			
		    	, intersect.MinX - xrel
			, intersect.MinY - yrel
			, intersect.MaxX - xrel
			, intersect.MaxY - yrel
		    );
*/		    
		    HIDD_GC_SetClipRect(gc
		    	, intersect.MinX
			, intersect.MinY
			, intersect.MaxX
			, intersect.MaxY
		    );
		    
		    HIDD_BM_DrawEllipse(HIDD_BM_OBJ(bm)
		    	, gc
			, center_x + xrel
			, center_y + yrel
			, rx
			, ry
		    );
		    
		    HIDD_GC_UnsetClipRect(gc);
		
		
		}
		else
		{
		    /* Render into offscreen cliprect bitmap */
		    if (L->Flags & LAYERSIMPLE)
		    	continue;
		    else if (L->Flags & LAYERSUPER)
		    	kprintf("do_render_func(): Superbitmap not handled yet\n");
		    else
		    {
		    	LONG bm_rel_minx, bm_rel_miny, bm_rel_maxx, bm_rel_maxy;
			LONG layer_rel_x, layer_rel_y;
			
			layer_rel_x = intersect.MinX - xrel;
			layer_rel_y = intersect.MinY - yrel;
			
			bm_rel_minx = intersect.MinX - CR->bounds.MinX;
			bm_rel_miny = intersect.MinY - CR->bounds.MinY;
			bm_rel_maxx = intersect.MaxX - CR->bounds.MinX;
			bm_rel_maxy = intersect.MaxY - CR->bounds.MinY;
			
		    	HIDD_GC_SetClipRect(gc
		    		, bm_rel_minx + ALIGN_OFFSET(CR->bounds.MinX)
				, bm_rel_miny
				, bm_rel_maxx + ALIGN_OFFSET(CR->bounds.MinX) 
				, bm_rel_maxy
			);
			
			HIDD_BM_DrawEllipse(HIDD_BM_OBJ(CR->BitMap)
				, gc
				, bm_rel_minx - (layer_rel_x - center_x) + ALIGN_OFFSET(CR->bounds.MinX)
				, bm_rel_miny - (layer_rel_y - center_y)
				, rx
				, ry
			);
				
				
			HIDD_GC_UnsetClipRect(gc);
		    }
		    
		} /* if (CR->lobs == NULL) */
		
	    } /* if (cliprect intersects with area to render into) */
	    
	} /* for (each cliprect in the layer) */
	
        UnlockLayerRom(L);
    } /* if (rp->Layer) */
    return;

}

struct bgf_render_data {
    WORD fbm_xsrc;
    Object *fbm;
};

static ULONG bgf_render(APTR bgfr_data
	, LONG srcx, LONG srcy
	, Object *dstbm_obj, Object *dst_gc
	, LONG x1, LONG y1, LONG x2, LONG y2
	, struct GfxBase *GfxBase)
{
    struct bgf_render_data *bgfrd;
    ULONG width, height;
    
    width  = x2 - x1 + 1;
    height = y2 - y1 + 1;
    
    bgfrd = (struct bgf_render_data *)bgfr_data;
    
    HIDD_BM_BlitColorExpansion( dstbm_obj
    	, dst_gc
	, bgfrd->fbm
	, srcx + bgfrd->fbm_xsrc, 0
	, x1, y1
	, width, height
     );
     
     return width * height;
    
}
    
void blit_glyph_fast(struct RastPort *rp, Object *fontbm, WORD xsrc
	, WORD destx, WORD desty, UWORD width, UWORD height)
{
    
    struct Rectangle rr;
    struct bgf_render_data bgfrd;

    EnterFunc(bug("blit_glyph_fast(%d, %d, %d, %d, %d)\n"
    	, xsrc, destx, desty, width, height));
	
    
    bgfrd.fbm_xsrc = xsrc;
    bgfrd.fbm	   = fontbm;
    
    rr.MinX = destx;
    rr.MinY = desty;
    rr.MaxX = destx + width  - 1;
    rr.MaxY = desty + height - 1;
	
    if (!CorrectDriverData(rp, GfxBase))
    	ReturnVoid("blit_glyph_fast");
	
    do_render_func(rp, NULL, &rr, bgf_render, &bgfrd, FALSE, GfxBase);
	
    ReturnVoid("blit_glyph_fast");
}

    

#define NUMCHARS(tf) ((tf->tf_HiChar - tf->tf_LoChar) + 2)
#define CTF(x) ((struct ColorTextFont *)x)

void driver_Text (struct RastPort * rp, STRPTR string, LONG len,
		struct GfxBase * GfxBase)
{

#warning Does not handle color textfonts
    WORD  render_y;
    struct TextFont *tf;
    WORD current_x;
    struct tfe_hashnode *hn;
    Object *fontbm = NULL;
    
    if (!CorrectDriverData (rp, GfxBase))
    	return;

    /* does this rastport have a layer. If yes, lock the layer it.*/
    if (NULL != rp->Layer)
      LockLayerRom(rp->Layer);	
    
    tf = rp->Font;
    
    /* Check if font has character data as a HIDD bitmap */
    hn = tfe_hashlookup(tf, GfxBase);
    if (NULL != hn)
    {
	if (NULL == hn->font_bitmap)
	{
	    hn->font_bitmap = fontbm_to_hiddbm(tf, GfxBase);
	}
    }
    else
    {
    	hn = tfe_hashnode_create(GfxBase);
	if (NULL != hn)
	{
	    
	    hn->font_bitmap = fontbm_to_hiddbm(tf, GfxBase);
	    tfe_hashadd(hn, tf, NULL, GfxBase);
	}
    }

    if (NULL != hn)
	fontbm = hn->font_bitmap;
    
    if (NULL == fontbm)
    {
    	kprintf("FONT HAS NO HIDD BITMAP ! Won't render text\n");
	return;
    }


    /* Render along font's baseline */
    render_y = rp->cp_y - tf->tf_Baseline;
    current_x = rp->cp_x;
    
    while ( len -- )
    {
	ULONG charloc;
	WORD render_x;
	ULONG idx;
	
	if (*string < tf->tf_LoChar || *string > tf->tf_HiChar )
	{
	     /* A character which there is no glyph for. We just
	        draw the last glyph in the font
	     */
	     idx = tf->tf_HiChar - tf->tf_LoChar;
	}
	else
	{
	    idx = *string - tf->tf_LoChar;
	}
	
	charloc = ((ULONG *)tf->tf_CharLoc)[idx];
	
	if (tf->tf_Flags & FPF_PROPORTIONAL)
	{
	    render_x = current_x + ((WORD *)tf->tf_CharKern)[idx];
	}
	else
	    render_x = current_x;	/* Monospace */
	    
	if (tf->tf_Style & FSF_COLORFONT)
	{
#warning Handle color fonts	
	}
	else
	{
	    WORD xoffset;
	    xoffset = charloc >> 16;
	
//	    kprintf("F ");  
	    blit_glyph_fast(rp
		, fontbm
		, xoffset
		, render_x, render_y
		, charloc & 0xFFFF
		, tf->tf_YSize
	    );
	}
	
	if (tf->tf_Flags & FPF_PROPORTIONAL)
	    current_x += ((WORD *)tf->tf_CharSpace)[idx];
	else
	    current_x += tf->tf_XSize; /* Add glyph width */
	
	string ++;
    } /* for (each character to render) */
    
    Move(rp, current_x, rp->cp_y);
    
    if (NULL != rp->Layer)
      UnlockLayerRom(rp->Layer);
    
    return;

}

WORD driver_TextLength (struct RastPort * rp, STRPTR string, ULONG len,
		    struct GfxBase * GfxBase)
{
    struct TextFont *tf = rp->Font;
    WORD strlen = 0;

    while (len --)
    {
	
	if (tf->tf_Flags & FPF_PROPORTIONAL)
	{
	    WORD idx;
	
	    if ( *string < tf->tf_LoChar || *string > tf->tf_HiChar)
	    {
		idx = NUMCHARS(tf) - 1; /* Last glyph is the default glyph */
	    }
	    else
	    {
		idx = *string - tf->tf_LoChar;
	    }
	    strlen += ((WORD *)tf->tf_CharSpace)[idx];
	}
	else
	{
	    strlen += tf->tf_XSize;
	}
	
	string ++;
	
    }
    return strlen;
}

void driver_Move (struct RastPort * rp, LONG x, LONG y,
		    struct GfxBase * GfxBase)
{
    return;
}

struct prlut8_render_data {
    ULONG pen;
    HIDDT_PixelLUT *pixlut;
};

static LONG pix_read_lut8(APTR prlr_data
	, Object *bm, Object *gc
	, LONG x, LONG y
	, struct GfxBase *GfxBase)
{
    struct prlut8_render_data *prlrd;
    
    
    prlrd = (struct prlut8_render_data *)prlr_data;
    
    if (NULL != prlrd->pixlut) {
	HIDD_BM_GetImageLUT(bm, (UBYTE *)&prlrd->pen, 1, x, y, 1, 1, prlrd->pixlut);
    } else {
    	prlrd->pen = HIDD_BM_GetPixel(bm, x, y);
    }

    return 0;
}


ULONG driver_ReadPixel (struct RastPort * rp, LONG x, LONG y,
		    struct GfxBase * GfxBase)
{
    struct prlut8_render_data prlrd;
    LONG ret;
    
    HIDDT_PixelLUT pixlut = { AROS_PALETTE_SIZE, HIDD_BM_PIXTAB(rp->BitMap) };
  
    if(!CorrectDriverData (rp, GfxBase))
	return ((ULONG)-1L);
	
    if (IS_HIDD_BM(rp->BitMap))
    	prlrd.pixlut = &pixlut;
    else
    	prlrd.pixlut = NULL;
	
    prlrd.pen = -1;

    ret = do_pixel_func(rp, x, y, pix_read_lut8, &prlrd, GfxBase);
    if (-1 == ret || -1 == prlrd.pen) {
        kprintf("ReadPixel(), COULD NOT GET PEN. TRYING TO READ FROM SimpleRefresh cliprect ??");
    	return (ULONG)-1;
    }
	
    return prlrd.pen;
}

LONG driver_WritePixel (struct RastPort * rp, LONG x, LONG y,
		    struct GfxBase * GfxBase)
{

    struct pix_render_data prd;

    if(!CorrectDriverData (rp, GfxBase))
	return  -1L;
	
    prd.pixel = BM_PIXEL(rp->BitMap, rp->FgPen);

    return do_pixel_func(rp, x, y, pix_write, &prd, GfxBase);
}

void driver_PolyDraw (struct RastPort * rp, LONG count, WORD * coords,
		    struct GfxBase * GfxBase)
{
    int i;                              /* Loop variable */

    for(i = 0; i < count-1; i++)
    {
	Move(rp, coords[2*i], coords[2*i+1]);
	Draw(rp, coords[2*i+2], coords[2*i+3]);
    }
}

/******** SetRast() ************************************/


void driver_SetRast (struct RastPort * rp, ULONG color,
		    struct GfxBase * GfxBase)
{

    
    /* We have to use layers to perform clipping */
    struct BitMap *bm = rp->BitMap;
    HIDDT_Pixel pixval;
    
    ULONG width, height;
    
    width  = GetBitMapAttr(bm, BMA_WIDTH);
    height = GetBitMapAttr(bm, BMA_HEIGHT);
    pixval = BM_PIXEL(bm, color);
    
    
    fillrect_pendrmd(rp
    	, 0, 0
	, width  - 1
	, height - 1
	, pixval
	, vHidd_GC_DrawMode_Copy
	, GfxBase
    );
    

    ReturnVoid("driver_SetRast");

}


void driver_SetFont (struct RastPort * rp, struct TextFont * font,
		    struct GfxBase * GfxBase)
{
    CorrectDriverData (rp, GfxBase);
}


struct TextFont * driver_OpenFont (struct TextAttr * ta,
	struct GfxBase * GfxBase)
{
    struct TextFont *tf, *best_so_far = NULL;
    WORD bestmatch = 0;
   
    
    if (!ta->ta_Name)
	return NULL;
	
    /* Search for font in the fontlist */
    Forbid();
    ForeachNode(&GfxBase->TextFonts, tf)
    {
	if (0 == strcmp(tf->tf_Message.mn_Node.ln_Name, ta->ta_Name))
	{
	    UWORD match;
	    struct TagItem *tags = NULL;
	    struct TextAttr match_ta =
	    {
	    	tf->tf_Message.mn_Node.ln_Name,
		tf->tf_YSize,
		tf->tf_Style,
		tf->tf_Flags
	    };
	    
	    if (ExtendFont(tf, NULL))
	    {
	        tags = ((struct TextFontExtension *)tf->tf_Extension)->tfe_Tags;
		
	    }
	    else
	    	tags = NULL;
	    
	    match = WeighTAMatch(ta, &match_ta, tags);
	    if (match > bestmatch)
	    {
	    	bestmatch = match;
		best_so_far = tf;
	    }
	}
    }
    Permit();

    
    return best_so_far;
}

void driver_CloseFont (struct TextFont * tf, struct GfxBase * GfxBase)
{
    /* Nobody using the font anymore ? */
    if (    tf->tf_Accessors == 0
         && (tf->tf_Flags & FPF_ROMFONT) == 0) /* Don't free ROM fonts */
    {
        Forbid();
	
	Remove((struct Node *)tf);
	
	Permit();
	
	/* Free font data */
	
	/* !!! NOTE. FreeXXX functions has to match AllocXXX in
	   workbench/libs/diskfont/diskfont_io.c
	*/

	if (tf->tf_Style & FSF_COLORFONT)
	{
	    UWORD i;
	    struct ColorFontColors *cfc;
			
	    for (i = 0; i < 8; i ++)
	    {
		if (CTF(tf)->ctf_CharData[i])
		    FreeVec(CTF(tf)->ctf_CharData[i]);
	    }
	    
	    cfc = CTF(tf)->ctf_ColorFontColors;
	    if (cfc)
	    {
		if (cfc->cfc_ColorTable)
		    FreeVec(cfc->cfc_ColorTable);
				
		FreeVec(cfc);
	    }

	}
	else
	{
	    /* Not a colortextfont, only one plane */
	    FreeVec(tf->tf_CharData);
	}
	StripFont(tf);
	
	if (tf->tf_CharSpace)
	    FreeVec(tf->tf_CharSpace);
	    
	if (tf->tf_CharKern)
	    FreeVec(tf->tf_CharKern);
	    
	/* All fonts have a tf_CharLoc allocated */    
	FreeVec(tf->tf_CharLoc); 
	
	FreeVec(tf->tf_Message.mn_Node.ln_Name);
	FreeVec(tf);
	
    }
    return;
}

BOOL driver_ExtendFont(struct TextFont *tf, struct tfe_hashnode *hn, struct GfxBase *GfxBase)
{
    if (NULL != hn->font_bitmap)
    	return TRUE;
	
    hn->font_bitmap = fontbm_to_hiddbm(tf, GfxBase);
    if (NULL != hn->font_bitmap)
	return TRUE;
	    
    return FALSE;
}

void driver_StripFont(struct TextFont *tf, struct tfe_hashnode *hn, struct GfxBase *GfxBase)
{
    if (NULL != hn->font_bitmap)
    {
    	DisposeObject(hn->font_bitmap);
    }
    return;
}

int driver_InitRastPort (struct RastPort * rp, struct GfxBase * GfxBase)
{

   /* Do nothing */
   
/*    if (!rp->BitMap)
    {
	rp->BitMap = AllocMem (sizeof (struct BitMap), MEMF_CLEAR|MEMF_ANY);
	
	if (!rp->BitMap)
	{
	    return FALSE;
	}
    }

*/
/*    if(!GetDriverData(rp))
	InitDriverData (rp, GfxBase);
    else
	CorrectDriverData(rp, GfxBase);

*/

    return TRUE;
}

int driver_CloneRastPort (struct RastPort * newRP, struct RastPort * oldRP,
			struct GfxBase * GfxBase)
{
    /* Let CorrectDriverData() have a bitmap to use for the GC */
    newRP->BitMap = oldRP->BitMap;
    
    /* Creates a new GC. Hmmm, a general Copy method would've been nice */
    
    if (!CorrectDriverData (newRP, GfxBase))
    	return FALSE;
	
    /* copy rastports attributes */
    SetFont(newRP, oldRP->Font);
    SetABPenDrMd(newRP, GetAPen(oldRP), GetBPen(oldRP), GetDrMd(oldRP));
    Move(newRP, oldRP->cp_x, oldRP->cp_y);
    
#warning Some attributes not copied    
    return TRUE;
}

void driver_DeinitRastPort (struct RastPort * rp, struct GfxBase * GfxBase)
{
    D(bug("driver_DeInitRP()\n"));

    if ( rp->Flags & RPF_DRIVER_INITED )
    {
    	D(bug("RP inited, rp=%p, %flags=%d=\n", rp, rp->Flags));
		 
        if (GetDriverData(rp)->dd_RastPort == rp) 
	{
	    D(bug("Calling DeInitDriverData\n"));
	    DeinitDriverData (rp, GfxBase);
	}
    }
    return;
}

void driver_InitView(struct View * View, struct GfxBase * GfxBase)
{
  /* To Do */
  View->DxOffset = 0;
  View->DyOffset = 0;
} /* driver_InitView */

void driver_InitVPort(struct ViewPort * ViewPort, struct GfxBase * GfxBase)
{
  /* To Do (maybe even an unnecessary function) */
} /* driver_InitVPort */

ULONG driver_SetWriteMask (struct RastPort * rp, ULONG mask,
			struct GfxBase * GfxBase)
{

    CorrectDriverData (rp, GfxBase);

#warning TODO

    /* For now we do not support bit masking */
    return FALSE;
    
}

void driver_WaitTOF (struct GfxBase * GfxBase)
{
}

void driver_LoadRGB4 (struct ViewPort * vp, UWORD * colors, LONG count,
	    struct GfxBase * GfxBase)
{
    LONG t;

    for (t = 0; t < count; t ++ )
    {
	driver_SetRGB32 (vp, t
	    , (colors[t] & 0x0F00) << 20
	    , (colors[t] & 0x00F0) << 24
	    , (colors[t] & 0x000F) << 28
	    , GfxBase
	);
        
    }
} /* driver_LoadRGB4 */

void driver_LoadRGB32(struct ViewPort * vp, const ULONG * table,
	    struct GfxBase * GfxBase)
{
    LONG t;
    
    EnterFunc(bug("driver_LoadRGB32(vp=%p, table=%p)\n", vp, table));
    
    
    while (*table)
    {
        ULONG count, first;
	
	count = (*table) >> 16;
	first = *table & 0xFFFF;

	table ++;

	for (t=0; t<count; t++)
	{
	    driver_SetRGB32 (vp, t + first
		, table[0]
		, table[1]
		, table[2]
		, GfxBase
	    );

	    table += 3;
	}

    } /* while (*table) */
    ReturnVoid("driver_LoadRGB32");

} /* driver_LoadRGB32 */

struct BitMap * driver_AllocBitMap (ULONG sizex, ULONG sizey, ULONG depth,
	ULONG flags, struct BitMap * friend, struct GfxBase * GfxBase)
{
    struct BitMap * nbm;
    
    
    kprintf("driver_AllocBitMap(sizex=%d, sizey=%d, depth=%d, flags=%d, friend=%p)\n",
    	sizex, sizey, depth, flags, friend);

    nbm = AllocMem (sizeof (struct BitMap), MEMF_ANY|MEMF_CLEAR);
    if (nbm)
    {
    	
	    Object *bm_obj;
	    Object *gfxhidd;
	
	    struct TagItem bm_tags[] =
	    {
		{ aHidd_BitMap_Width,		0	},	/* 0 */
		{ aHidd_BitMap_Height,		0	},	/* 1 */
		{ aHidd_BitMap_Depth,		0	},	/* 2 */
		{ aHidd_BitMap_Displayable,	0	},	/* 3 */
		{ aHidd_BitMap_Friend,		0	},	/* 4 */
		{ aHidd_BitMap_StdPixFmt,	0	},	/* 5 */
		{ TAG_DONE, 0 }
	    };

kprintf("bitmap struct allocated\n");	    
	    /* Insert supplied values */
	    bm_tags[0].ti_Data = sizex;
	    bm_tags[1].ti_Data = sizey;
	    bm_tags[2].ti_Data = depth;
	    bm_tags[3].ti_Data = ((flags & BMF_DISPLAYABLE) ? TRUE : FALSE);
	    
	    if (NULL != friend)
	    {
		if (IS_HIDD_BM(friend))
		    bm_tags[4].ti_Data = (IPTR)HIDD_BM_OBJ(friend);
	    }
	    
	    if (flags & BMF_SPECIALFMT) {
	    	HIDDT_StdPixFmt stdpf;
		
		stdpf = cyber2hidd_pixfmt(DOWNSHIFT_PIXFMT(flags), GfxBase);
		bm_tags[5].ti_Data = (IPTR)stdpf;
		
	    } else {
	    	bm_tags[5].ti_Tag = TAG_IGNORE;
	    }

	
	    gfxhidd  = SDD(GfxBase)->gfxhidd;

	    /* Create HIDD bitmap object */
	    if (gfxhidd)
	    {
kprintf("Creating HIDD bitmap\n");	    
		bm_obj = HIDD_Gfx_NewBitMap(gfxhidd, bm_tags);
		if (NULL != bm_obj)
		{
		
		    Object *pf;
		    Object *colmap;
		    ULONG graphtype;

kprintf("HIDD bitmap created\n");		    
		    
		    /* 	It is possible that the HIDD had to allocate
		   	a larger depth than that supplied, so
		   	we should get back the correct depth.
		   	This is because layers.library might
		   	want to allocate offscreen bimaps to
		   	store obscured areas, and then those
		   	offscreen bitmaps should be of the same depth as
		   	the onscreen ones.
		    */
		   
		    pf = HIDD_BM_GetPixelFormat(bm_obj, vHidd_PixFmt_Native);
		   
		    GetAttr(pf, aHidd_PixFmt_Depth, &depth);
		    GetAttr(pf, aHidd_PixFmt_GraphType, &graphtype);
		    
		    GetAttr(bm_obj, aHidd_BitMap_ColorMap, (IPTR *)&colmap);
	    	    
		    	/* Store it in plane array */
		    HIDD_BM_OBJ(nbm) = bm_obj;
		    HIDD_BM_GRAPHTYPE(nbm) = graphtype;
		    HIDD_BM_COLMAP(nbm) = colmap;
		    nbm->Rows   = sizey;
		    nbm->BytesPerRow = WIDTH_TO_BYTES(sizex);
		    nbm->Depth  = depth;
		    nbm->Flags  = flags | BMF_AROS_HIDD;
		    
		    /* If this is a displayable bitmap, create a color table for it */

		    if (flags & BMF_DISPLAYABLE) {
		        /* Allcoate a pixtab */
			HIDD_BM_PIXTAB(nbm) = AllocVec(sizeof (HIDDT_Pixel) * AROS_PALETTE_SIZE, MEMF_ANY);
			if (NULL != HIDD_BM_PIXTAB(nbm)) {
			    /* Set this palette to all black by default */
			    
			    HIDDT_Color col;
			    ULONG i;
kprintf("Pixtab allocated\n");			
			
			    col.red     = 0;
			    col.green   = 0;
			    col.blue    = 0;
			    col.alpha   = 0;
			
			    if (vHidd_GT_Palette == graphtype || vHidd_GT_TrueColor == graphtype) {
			
			    	ULONG numcolors;
			
			    	numcolors = 1L << depth;
			    	if (numcolors > AROS_PALETTE_SIZE)
			    	    numcolors = AROS_PALETTE_SIZE;
			    	
			    	/* Set palette to all black */
			    	for (i = 0; i < numcolors; i ++) {
			    	    HIDD_BM_SetColors(HIDD_BM_OBJ(nbm), &col, i, 1);
				    HIDD_BM_PIXTAB(nbm)[i] = col.pixval;
			        }
			    }
			    ReturnPtr("driver_AllocBitMap", struct BitMap *, nbm);
			    
			    
			} /* if (pixtab successfully allocated) */
			
			
		    }
		    else
		    {
		    	if (friend)
			{
#warning Here we assume that the friend bitmap is a HIDD bitmap		    
			    /* We got a friend bitmap. We inherit its colormap
			       !!! NOTE !!! If this is used after the friend bitmap is freed
			       it means trouble, as the colortab mem
			       will no longer be valid
			    */

			    HIDD_BM_COLMAP(nbm) = HIDD_BM_COLMAP(friend);
			    HIDD_BM_PIXTAB(nbm) = HIDD_BM_PIXTAB(friend);

#if 0
kprintf("ALLOCBITMAP, NON DISPLAYABLE : %p\n", nbm);

kprintf("bm: %p, %d, %d, %d, %d\n"
	, nbm
	, nbm->BytesPerRow
	, nbm->Rows
	, nbm->Flags
	, nbm->Depth
);

{
int i;
for (i =0; i < 8; i ++)
	kprintf("plane[%d]: %p\n"
	   , i, nbm->Planes[i] );

}
#endif

			    ReturnPtr("driver_AllocBitMap", struct BitMap *, nbm);
			    
			}
		    }
		    
		    DisposeObject(bm_obj);
	    
	    
		} /* if (bitmap object allocated) */
		
	    } /* if (gfxhidd) */
	    
	    
	
	FreeMem(nbm, sizeof (struct BitMap));
	
    }

    ReturnPtr("driver_AllocBitMap", struct BitMap *, NULL);
}


struct blit_info
{
    struct BitMap *bitmap;
    ULONG minterm;
    ULONG planemask;
    UBYTE bmdepth;
    ULONG bmwidth;
    
};

#define BI(x) ((struct blit_info *)x)
static VOID bitmap_to_buf(APTR src_info
	, LONG x_src, LONG y_src
	, LONG x_dest, LONG y_dest
	, LONG width, LONG height
	, ULONG *bufptr
	, Object *dest_bm
	, HIDDT_Pixel *coltab
) /* destination HIDD bitmap */
{

    LONG y;
    
    /* Fill buffer with pixels from bitmap */
    for (y = 0; y < height; y ++)
    {
	LONG x;
	    
	for (x = 0; x < width; x ++)
	{
	    UBYTE pen;
	    
	    pen = getbitmappixel(BI(src_info)->bitmap
		, x + x_src
		, y + y_src
		, BI(src_info)->bmdepth
		, BI(src_info)->planemask);
		
		
		
	    *bufptr ++ = (coltab != NULL) ? coltab[pen] : pen;
//	    kprintf("(%d, %d) pen=%d buf=%d\n", x, y, pen, coltab[pen]);
			

	}
	
    }

}


static VOID buf_to_bitmap(APTR dest_info
	, LONG x_src, LONG y_src
	, LONG x_dest, LONG y_dest
	, ULONG width, ULONG height
	, UBYTE *bufptr
	, Object *src_bm
	, HIDDT_Pixel *coltab
)
{
	
    if (BI(dest_info)->minterm ==  0x00C0)
    {
	LONG y;
	for (y = 0; y < height; y ++)
	{
	    LONG x;
	    for (x = 0; x < width; x ++)
	    {
		setbitmappixel(BI(dest_info)->bitmap
		    	, x + x_dest
			, y + y_dest
			, *bufptr ++, BI(dest_info)->bmdepth, BI(dest_info)->planemask
		);


	    }
		
	}

    }
    else
    {
	LONG y;
	    
	for (y = 0; y < height; y ++)
	{
	    LONG x;
		
	    for (x = 0; x < width; x ++)
	    {
		ULONG src = *bufptr ++ , dest = 0;
		ULONG minterm = BI(dest_info)->minterm;

		/* Set the pixel using correct minterm */

		dest = getbitmappixel(BI(dest_info)->bitmap
			, x + x_dest
			, y + y_dest
			, BI(dest_info)->bmdepth
			, BI(dest_info)->planemask
		);

#warning Do reverse coltab lookup	    	
		if (minterm & 0x0010) dest  = ~src & ~dest;
		if (minterm & 0x0020) dest |= ~src & dest;
		if (minterm & 0x0040) dest |=  src & ~dest;
		if (minterm & 0x0080) dest |= src & dest;
		    
		setbitmappixel(BI(dest_info)->bitmap
			, x + x_dest
			, y + y_dest
			, dest, BI(dest_info)->bmdepth
			, BI(dest_info)->planemask
		);

	    }
		
	}
	    
    }
    return;

}

/* General functions for moving blocks of data to or from HIDDs, be it pixelarrays
  or bitmaps. They use a callback-function to get data from amiga/put data to amiga bitmaps/pixelarrays
  
*/	
static VOID amiga2hidd_fast(APTR src_info
	, Object *hidd_gc
	, LONG x_src , LONG	y_src
	, struct BitMap 	*hidd_bm
	, LONG x_dest, LONG y_dest
	, ULONG xsize, ULONG ysize
	, VOID (*fillbuf_hook)()
)
{
    
    
    ULONG tocopy_w,
    	  tocopy_h;
	  
    LONG pixels_left_to_process = xsize * ysize;
	  
    LONG current_x, current_y, next_x, next_y;
    Object *bm_obj;

    next_x = 0;
    next_y = 0;
    
    bm_obj = OBTAIN_HIDD_BM(hidd_bm);
    if (NULL == bm_obj)
    	return;
    

LOCK_PIXBUF    
    while (pixels_left_to_process)
    {

	/* Get some more pixels from the HIDD */

	current_x = next_x;
	current_y = next_y;
	
	if (NUMPIX < xsize)
	{
	   /* buffer can't hold a single horizontal line, and must 
	      divide each line into several copy-operations */
	    tocopy_w = xsize - current_x;
	    if (tocopy_w > NUMPIX)
	    {
	        /* Not quite finished with current horizontal pixel line */
	    	tocopy_w = NUMPIX;
		next_x += NUMPIX;
	    }
	    else
	    {	/* Start at a new line */
	    	next_x = 0;
		next_y ++;
	    }
	    tocopy_h = 1;
	    
    	}
	else /* We can copy one or several whole horizontal lines at a time */
	{
	    tocopy_h = MIN(NUMPIX / xsize, ysize - current_y);

	    tocopy_w = xsize;

	    next_x = 0;
	    next_y += tocopy_h;
	    
	
	}


	/* Get data */
	fillbuf_hook(src_info
		, current_x + x_src
		, current_y + y_src
		, current_x + x_dest
		, current_y + y_dest
		, tocopy_w, tocopy_h
		, pixel_buf
		, bm_obj
		, IS_HIDD_BM(hidd_bm) ? HIDD_BM_PIXTAB(hidd_bm) : NULL
	);
	
	/* Put it to the HIDD */
	D(bug("Putting box\n"));

	HIDD_BM_PutImage(bm_obj
		, hidd_gc
		, (UBYTE*)pixel_buf
		, tocopy_w * sizeof (HIDDT_Pixel)
		, x_dest + current_x
		, y_dest + current_y
		, tocopy_w, tocopy_h
		, vHidd_PixFmt_Native32
	);

	D(bug("Box put\n"));

	pixels_left_to_process -= (tocopy_w * tocopy_h);
	
	
    } /* while (pixels left to copy) */
    
ULOCK_PIXBUF    

    RELEASE_HIDD_BM(bm_obj, hidd_bm);
    
    return;
    
}
	

static VOID hidd2amiga_fast(struct BitMap *hidd_bm
	, LONG x_src , LONG y_src
	, APTR dest_info
	, LONG x_dest, LONG y_dest
	, ULONG xsize, ULONG ysize
	, VOID (*putbuf_hook)()
)
{

    ULONG tocopy_w, tocopy_h;
    
    LONG pixels_left_to_process = xsize * ysize;
    ULONG current_x, current_y, next_x, next_y;
    
#warning Src bitmap migh be user initialized so we should not use HIDD_BM_PIXTAB() below
    HIDDT_PixelLUT pixlut = { AROS_PALETTE_SIZE, HIDD_BM_PIXTAB(hidd_bm) };
    
    Object *bm_obj;
    
    next_x = 0;
    next_y = 0;
    
    bm_obj = OBTAIN_HIDD_BM(hidd_bm);
    if (NULL == bm_obj)
    	return;
	
LOCK_PIXBUF    

    while (pixels_left_to_process)
    {
	
	current_x = next_x;
	current_y = next_y;
	
	if (NUMLUTPIX < xsize)
	{
	   /* buffer cant hold a single horizontal line, and must 
	      divide each line into copies */
	    tocopy_w = xsize - current_x;
	    if (tocopy_w > NUMLUTPIX)
	    {
	        /* Not quite finished with current horizontal pixel line */
	    	tocopy_w = NUMLUTPIX;
		next_x += NUMLUTPIX;
	    }
	    else
	    {	/* Start at a new line */
	    
	    	next_x = 0;
		next_y ++;
	    }
	    tocopy_h = 1;
	    
    	}
    	else
    	{
	    tocopy_h = MIN(NUMLUTPIX / xsize, ysize - current_y);
	    tocopy_w = xsize;

	    next_x = 0;
	    next_y += tocopy_h;
	    
    	}
	
	
	/* Get some more pixels from the HIDD */
	HIDD_BM_GetImageLUT(bm_obj
		, (UBYTE *)pixel_buf
		, tocopy_w
		, x_src + current_x
		, y_src + current_y
		, tocopy_w, tocopy_h
		, &pixlut);


	/*  Write pixels to the destination */
	putbuf_hook(dest_info
		, current_x + x_src
		, current_y + y_src
		, current_x + x_dest
		, current_y + y_dest
		, tocopy_w, tocopy_h
		, (UBYTE *)pixel_buf
		, bm_obj
		, IS_HIDD_BM(hidd_bm) ? HIDD_BM_PIXTAB(hidd_bm) : NULL
	);
	
	pixels_left_to_process -= (tocopy_w * tocopy_h);

    }
    
ULOCK_PIXBUF

    RELEASE_HIDD_BM(bm_obj, hidd_bm);
    
    return;
    
}


#define FLG_PALETTE		( 1L << vHidd_GT_Palette	)
#define FLG_STATICPALETTE	( 1L << vHidd_GT_StaticPalette	)
#define FLG_TRUECOLOR		( 1L << vHidd_GT_TrueColor	)
#define FLG_HASCOLMAP		( 1L << num_Hidd_GT		)

#define GET_GT_FLAGS(bm) (1L << HIDD_BM_GRAPHTYPE(bm))


LONG driver_BltBitMap (struct BitMap * srcBitMap, LONG xSrc,
	LONG ySrc, struct BitMap * dstBitMap, LONG xDest,
	LONG yDest, LONG xSize, LONG ySize, ULONG minterm,
	ULONG mask, PLANEPTR tempA, struct GfxBase * GfxBase)
{
    
    ULONG wSrc, wDest;
    ULONG x;
    ULONG depth;
    
    Object *tmp_gc;

    EnterFunc(bug("driver_BltBitMap()\n"));
	

/* kprintf("BltBitMap(%p, %d, %d, %p, %d, %d, %d, %d, %x)\n"
		,srcBitMap, xSrc, ySrc, dstBitMap, xDest, yDest, xSize, ySize, minterm);

		
kprintf("Amiga to Amiga, wSrc=%d, wDest=%d\n",
		wSrc, wDest);
*/	
    wSrc  = GetBitMapAttr( srcBitMap, BMA_WIDTH);
    wDest = GetBitMapAttr(dstBitMap, BMA_WIDTH);

    /* Clip all blits */

    depth = GetBitMapAttr ( srcBitMap, BMA_DEPTH);
    x     = GetBitMapAttr (dstBitMap, BMA_DEPTH);
    if (x < depth)
	depth = x;

    /* Clip X and Y */
    if (xSrc < 0)
    {
	xDest += -xSrc;
	xSize -= -xSrc;
	xSrc = 0;
    }

    if (ySrc < 0)
    {
	yDest += -ySrc;
	ySize -= -ySrc;
	ySrc = 0;
    }

    /* Clip width and height for source and dest */
    if (ySrc + ySize > srcBitMap->Rows)
    {
	ySize = srcBitMap->Rows - ySrc;
    }

    if (yDest + ySize > dstBitMap->Rows)
    {
	ySize = dstBitMap->Rows - yDest;
    }

    if (xSrc + xSize >= wSrc)
    {
	xSize = wSrc - xSrc;
    }
        
    if (xDest + xSize >= wDest)
    {
    	xSize = wDest - xDest;
    }

    /* If the size is illegal or we need not copy anything, return */
    if (ySize <= 0 || xSize <= 0 || !mask)
	return 0;


    tmp_gc = obtain_cache_object(SDD(GfxBase)->gc_cache, GfxBase);
    if (NULL != tmp_gc) {
    
	Object *srcbm_obj;
    
    	srcbm_obj = OBTAIN_HIDD_BM(srcBitMap);
	if (NULL != srcbm_obj) {
	
	    Object *dstbm_obj;
	    
	    dstbm_obj = OBTAIN_HIDD_BM(dstBitMap);
	    if (NULL != dstbm_obj) {
	    
	    	int_bltbitmap(srcBitMap, srcbm_obj
			, xSrc, ySrc
			, dstBitMap, dstbm_obj
			, xDest, yDest
			, xSize, ySize
			, minterm
			, tmp_gc
			, GfxBase);
	    
	    	RELEASE_HIDD_BM(dstbm_obj, dstBitMap);
	    }
	
	    RELEASE_HIDD_BM(srcbm_obj, srcBitMap);
	}
	release_cache_object(SDD(GfxBase)->gc_cache, tmp_gc, GfxBase);
    }
    
    return 8;
}

static BOOL int_bltbitmap(struct BitMap *srcBitMap, Object *srcbm_obj
	, LONG xSrc, LONG ySrc
	, struct BitMap *dstBitMap, Object *dstbm_obj
	, LONG xDest, LONG yDest, LONG xSize, LONG ySize
	, ULONG minterm, Object *gc, struct GfxBase *GfxBase)
{
    HIDDT_DrawMode drmd;

    ULONG srcflags = 0;
    ULONG dstflags = 0;

    BOOL src_colmap_set = FALSE;
    BOOL dst_colmap_set = FALSE;
    BOOL success = TRUE;
    BOOL colmaps_ok = TRUE;

    drmd = MINTERM_TO_GCDRMD(minterm);
    
/* We must lock any HIDD_BM_SetColorMap calls */
LOCK_BLIT

    /* Try to get a CLUT for the bitmaps */
    if (IS_HIDD_BM(srcBitMap)) {
    	if (NULL != HIDD_BM_COLMAP(srcBitMap))
    	    srcflags |= FLG_HASCOLMAP;
    	dstflags |= GET_GT_FLAGS(srcBitMap);
    } else {
    	/* Amiga BM */
    	srcflags |= FLG_PALETTE;
    }

    if (IS_HIDD_BM(dstBitMap)) {
    	if (NULL != HIDD_BM_COLMAP(dstBitMap))
    	    dstflags |= FLG_HASCOLMAP;
    	dstflags |= GET_GT_FLAGS(dstBitMap);
    } else {
    	/* Amiga BM */
    	dstflags |= FLG_PALETTE;
    }
    	
//kprintf("BltBitMap: Checking, srcflags=%d, dstflags=%d\n", srcflags, dstflags);

    if (    (srcflags == FLG_PALETTE || srcflags == FLG_STATICPALETTE)) {
    	/* palettized with no colmap. Neew to get a colmap from dest*/
    	if (dstflags == FLG_TRUECOLOR) {
    	
    	    kprintf("!!! NO WAY GETTING PALETTE FOR src IN BltBitMap\n");
    	    colmaps_ok = FALSE;
	    success = FALSE;
    	    
    	} else if (dstflags == (FLG_TRUECOLOR | FLG_HASCOLMAP)) {
    	
    	    /* Use the dest colmap for src */
    	    HIDD_BM_SetColorMap(srcbm_obj, HIDD_BM_COLMAP(dstBitMap));

/* 		
kprintf("Colormap:\n");
{
ULONG idx;
for (idx = 0; idx < 256; idx ++)
	kprintf("[%d]=%d ", idx, HIDD_CM_GetPixel(HIDD_BM_COLMAP(dstBitMap), idx));
			src_colmap_set = TRUE;
}
*/
		    }
    }

    if (   (dstflags == FLG_PALETTE || dstflags == FLG_STATICPALETTE)) {
    	/* palettized with no pixtab. Nees to get a pixtab from dest*/
    	if (srcflags == FLG_TRUECOLOR) {
    	    kprintf("!!! NO WAY GETTING PALETTE FOR dst IN BltBitMap\n");
    	    colmaps_ok = FALSE;
	    success = FALSE;
    	    
    	} else if (srcflags == (FLG_TRUECOLOR | FLG_HASCOLMAP)) {
    	
    	    /* Use the src colmap for dst */
    	    HIDD_BM_SetColorMap(dstbm_obj, HIDD_BM_COLMAP(srcBitMap));
    	    
    	    dst_colmap_set = TRUE;
    	}
    }
    	    
    if (colmaps_ok) {
    	/* We need special treatment with drawmode Clear and
    	   truecolor bitmaps, in order to set it to
    	   colormap[0] instead of just 0
    	*/
    	if (	(drmd == vHidd_GC_DrawMode_Clear)
    	     && ( (dstflags & (FLG_TRUECOLOR | FLG_HASCOLMAP)) == (FLG_TRUECOLOR | FLG_HASCOLMAP) )) {
    	     
	    HIDDT_DrawMode old_drmd;
	    HIDDT_Pixel old_fg;
	    
    	    struct TagItem frtags[] = {
    		 { aHidd_GC_Foreground, 0 },
    		 { aHidd_GC_DrawMode, vHidd_GC_DrawMode_Copy },
    		 { TAG_DONE, 0UL }
    	    };
	    
	    GetAttr(gc, aHidd_GC_DrawMode, &old_drmd);
	    GetAttr(gc, aHidd_GC_Foreground, &old_fg);
    	    
    	    frtags[0].ti_Data = HIDD_BM_PIXTAB(dstBitMap)[0];
	    frtags[1].ti_Data = vHidd_GC_DrawMode_Copy;
	    
    	    SetAttrs(gc, frtags);
    	    
    	    HIDD_BM_FillRect(dstbm_obj, gc
    		    , xDest, yDest
    		    , xDest + xSize - 1
    		    , yDest + ySize - 1
    	    );

    	    frtags[0].ti_Data = old_fg;
	    frtags[1].ti_Data = old_drmd;
    	
    	} else {
	    HIDDT_DrawMode old_drmd;
	    
	    struct TagItem cbtags[] = {
    		{ aHidd_GC_DrawMode,	    0 },
    		{ TAG_DONE, 0 }
	    };
	    
	    GetAttr(gc, aHidd_GC_DrawMode, &old_drmd);
	    
	    cbtags[0].ti_Data = drmd;
	    
	    SetAttrs(gc, cbtags);
    	    HIDD_BM_CopyBox(srcbm_obj, gc
    		, xSrc, ySrc
    		, dstbm_obj
    		, xDest, yDest
    		, xSize, ySize
    	    );
	    
	    cbtags[0].ti_Data = drmd;
	    SetAttrs(gc, cbtags);
    	}
    }

    if (src_colmap_set)
    	HIDD_BM_SetColorMap(srcbm_obj, NULL);
    	
    if (dst_colmap_set)
    	HIDD_BM_SetColorMap(dstbm_obj, NULL);
	
ULOCK_BLIT
	
    return success;

}


void driver_FreeBitMap (struct BitMap * bm, struct GfxBase * GfxBase)
{
    Object *gfxhidd = SDD(GfxBase)->gfxhidd;
    
    HIDD_Gfx_DisposeBitMap(gfxhidd, (Object *)HIDD_BM_OBJ(bm));
    
    if (bm->Flags & BMF_DISPLAYABLE)
    {
    	FreeVec(HIDD_BM_PIXTAB(bm));
    }
    FreeMem(bm, sizeof (struct BitMap));
}


void driver_SetRGB32 (struct ViewPort * vp, ULONG color,
	    ULONG red, ULONG green, ULONG blue,
	    struct GfxBase * GfxBase)
{
    struct BitMap *bm;
   HIDDT_Color hidd_col;
   Object *pf;
   ULONG graphtype;
   
   EnterFunc(bug("driver_SetRGB32(vp=%p, color=%d, r=%x, g=%x, b=%x)\n",
   		vp, color, red, green, blue));

    /* This is cybergraphx. We only work wih HIDD bitmaps */
		
   /* Get bitmap object */
   bm = vp->RasInfo->BitMap;

   if (!IS_HIDD_BM(bm)) {
    	kprintf("!!!!! Trying to use SetRGB32() call on non-hidd bitmap!!!\n");
    	return;
   }
   if (NULL == HIDD_BM_COLMAP(bm)) {
    	kprintf("!!!!! Trying to use SetRGB32() call on bitmap with no CLUT !!!\n");
	return;
   }
   
   
   /* HIDDT_Color entries are UWORD */
   hidd_col.red   = red   >> 16;
   hidd_col.green = green >> 16 ;
   hidd_col.blue  = blue  >> 16;
   hidd_col.alpha = 0;
   
   pf = HIDD_BM_GetPixelFormat(HIDD_BM_OBJ(bm), vHidd_PixFmt_Native);
   
   GetAttr(pf, aHidd_PixFmt_GraphType, &graphtype);
   
   
   if (vHidd_GT_Palette == graphtype || vHidd_GT_TrueColor == graphtype) {
   	HIDD_BM_SetColors(HIDD_BM_OBJ(bm), &hidd_col, color, 1);

/*
kprintf("SetRGB32: col %d (%x %x %x %x) mapped to %x\n"
		, color
		, hidd_col.red, hidd_col.green, hidd_col.blue, hidd_col.alpha
		, hidd_col.pixval);
*/		
	HIDD_BM_PIXTAB(bm)[color] = hidd_col.pixval;
   }
	
   
   ReturnVoid("driver_SetRGB32");
   

} /* driver_SetRGB32 */

void driver_SetRGB4 (struct ViewPort * vp, ULONG color,
	    ULONG red, ULONG green, ULONG blue,
	    struct GfxBase * GfxBase)
{
 	driver_SetRGB32 (vp, color
	    , (ULONG)(red<<28)
	    , (ULONG)(green<<28)
	    , (ULONG)(blue<<28)
	    , GfxBase
	);
}



struct wp8_render_data {
    UBYTE *array;
    ULONG modulo;
    HIDDT_PixelLUT *pixlut;
};

static ULONG wp8_render(APTR wp8r_data
	, LONG srcx, LONG srcy
	, Object *dstbm_obj, Object *dst_gc
	, LONG x1, LONG y1, LONG x2, LONG y2
	, struct GfxBase *GfxBase)
{
    struct wp8_render_data *wp8rd;
    ULONG width, height;
    
    wp8rd = (struct wp8_render_data *)wp8r_data;
    
    width  = x2 - x1 + 1;
    height = y2 - y1 + 1;
    
    HIDD_BM_PutImageLUT(dstbm_obj
    	, dst_gc
	, wp8rd->array + CHUNKY8_COORD_TO_BYTEIDX(srcx, srcy, wp8rd->modulo)
	, wp8rd->modulo
	, x1, y1
	, width, height
	, wp8rd->pixlut
    );
    
    return width * height;
}

static LONG write_pixels_8(struct RastPort *rp, UBYTE *array
	, ULONG modulo
	, LONG xstart, LONG ystart
	, LONG xstop, LONG ystop
	, HIDDT_PixelLUT *pixlut
	, struct GfxBase *GfxBase)

{
	
    LONG pixwritten = 0;
    
    struct wp8_render_data wp8rd;
    struct Rectangle rr;
    
    Object *gc;
    HIDDT_DrawMode old_drmd;

    struct TagItem gc_tags[] =
    {
	{ aHidd_GC_DrawMode, vHidd_GC_DrawMode_Copy},
	{ TAG_DONE, 0}
    };
    
    
    if (!CorrectDriverData (rp, GfxBase))
	return 0;
	
    gc = GetDriverData(rp)->dd_GC;
    
    GetAttr(gc, aHidd_GC_DrawMode, &old_drmd);
    SetAttrs(gc, gc_tags);
    
    wp8rd.modulo	= modulo;
    wp8rd.array		= array;
    wp8rd.pixlut	= pixlut;
    
    rr.MinX = xstart;
    rr.MinY = ystart;
    rr.MaxX = xstop;
    rr.MaxY = ystop;
    
    pixwritten = do_render_func(rp, NULL, &rr, wp8_render, &wp8rd, FALSE, GfxBase);
    
    /* Reset to preserved drawmode */
    gc_tags[0].ti_Data = old_drmd;
    SetAttrs(gc, gc_tags);
    
    return pixwritten;
}


LONG driver_WritePixelArray8 (struct RastPort * rp, ULONG xstart,
	    ULONG ystart, ULONG xstop, ULONG ystop, UBYTE * array,
	    struct RastPort * temprp, struct GfxBase * GfxBase)
{

    LONG pixwritten;

#warning Do not use HIDD_BM_PIXTAB, because object might have no pixtab
    HIDDT_PixelLUT pixlut = { AROS_PALETTE_SIZE, HIDD_BM_PIXTAB(rp->BitMap) };
    
    EnterFunc(bug("driver_WritePixelArray8(%p, %d, %d, %d, %d)\n",
    	rp, xstart, ystart, xstop, ystop));
	
  
    pixwritten = write_pixels_8(rp, array
    	, xstop - xstart + 1 /* modulo */
	, xstart, ystart
	, xstop, ystop
	, &pixlut
	, GfxBase);

    ReturnInt("driver_WritePixelArray8", LONG, pixwritten);
    
} /* driver_WritePixelArray8 */



struct rp8_render_data {
    UBYTE *array;
    ULONG modulo;
    HIDDT_PixelLUT *pixlut;
};


static ULONG rp8_render(APTR rp8r_data
	, LONG srcx, LONG srcy
	, Object *srcbm_obj, Object *gc
	, LONG x1, LONG y1, LONG x2, LONG y2
	, struct GfxBase *GfxBase)
{
    struct rp8_render_data *rp8rd;
    ULONG width, height;
    
    rp8rd = (struct rp8_render_data *)rp8r_data;
    
    width  = x2 - x1 + 1;
    height = y2 - y1 + 1;
    
    HIDD_BM_GetImageLUT(srcbm_obj
	, rp8rd->array + CHUNKY8_COORD_TO_BYTEIDX(srcx, srcy, rp8rd->modulo)
	, rp8rd->modulo
	, x1, y1
	, width, height
	, rp8rd->pixlut
    );
    
    return width * height;
}

LONG driver_ReadPixelArray8 (struct RastPort * rp, ULONG xstart,
	    ULONG ystart, ULONG xstop, ULONG ystop, UBYTE * array,
	    struct RastPort * temprp, struct GfxBase * GfxBase)
{
    
    struct rp8_render_data rp8rd;
    struct Rectangle rr;

#warning Do not use HIDD_BM_PIXTAB() for non-hidd bitmaps
    HIDDT_PixelLUT pixlut = { AROS_PALETTE_SIZE, HIDD_BM_PIXTAB(rp->BitMap) };
    
    LONG pixread = 0;
    
    EnterFunc(bug("driver_ReadPixelArray8(%p, %d, %d, %d, %d)\n",
    	rp, xstart, ystart, xstop, ystop));
    
    if (!CorrectDriverData (rp, GfxBase))
	return 0;
	
    
    rp8rd.array  = array;
    rp8rd.modulo = xstop - xstart + 1;
    rp8rd.pixlut = &pixlut;
    
    rr.MinX = xstart;
    rr.MinY = ystart;
    rr.MaxX = xstop;
    rr.MaxY = ystop;
    
    pixread = do_render_func(rp, NULL, &rr, rp8_render, &rp8rd, FALSE, GfxBase);
	
    ReturnInt("driver_ReadPixelArray8", LONG, pixread);
    
} /* driver_ReadPixelArray8 */


struct template_info
{
    PLANEPTR source;
    LONG x_src;
    LONG modulo;
    BOOL invertsrc;
};

VOID template_to_buf(struct template_info *ti
	, LONG x_src, LONG y_src
	, LONG x_dest, LONG y_dest
	, ULONG xsize, ULONG ysize
	, ULONG *buf
	, struct BitMap *dest_bm)
{
    UBYTE *srcptr;
    LONG x, y;
    
    EnterFunc(bug("template_to_buf(%p, %d, %d, %d, %d, %p)\n"
    			, ti, x_src, y_src, xsize, ysize, buf));
    /* Find the exact startbyte */
    srcptr = ti->source + XCOORD_TO_BYTEIDX(x_src) + (ti->modulo * y_src);
    
    /* Find the exact startbit */
    x_src &= 0x07;

    for (y = 0; y < ysize; y ++)
    {
	UBYTE *byteptr = srcptr;
    	for (x = 0; x < xsize; x ++)
	{
	    UBYTE mask = XCOORD_TO_MASK(x + x_src);
	    BOOL is_set = ((*byteptr & mask) ? TRUE : FALSE);
	    
	    if (ti->invertsrc)
	    {
	    	is_set = ((is_set == TRUE) ? FALSE : TRUE);
	    }
	    
	    if (is_set)
		*buf = 1UL;
	    else
		*buf = 0UL;
	    buf ++;

	    /* Last pixel in this byte ? */
	    if (((x + x_src) & 0x07) == 0x07)
	    {
	    	byteptr ++;
	    }
		
	}
	srcptr += ti->modulo;
    }
    
    ReturnVoid("template_to_buf");
}

/********** BltTemplate() *************************************/


struct blttemplate_render_data {
     Object *template_bm;
};

static ULONG blttemplate_render(APTR btr_data
	, LONG srcx, LONG srcy
	, Object *dstbm_obj
	, Object *dst_gc
	, LONG x1, LONG y1, LONG x2, LONG y2
	, struct GfxBase *GfxBase)
{
    struct blttemplate_render_data *btrd;
    ULONG width, height;
    
    width  = x2 - x1 + 1;
    height = y2 - y1 + 1;
    
    btrd = (struct blttemplate_render_data *)btr_data;
    
    HIDD_BM_BlitColorExpansion( dstbm_obj
    	, dst_gc
	, btrd->template_bm
	, srcx, srcy
	, x1, y1
	, width, height
     );
     
     return width * height;
    
}

VOID driver_BltTemplate(PLANEPTR source, LONG xSrc, LONG srcMod, struct RastPort * destRP,
	LONG xDest, LONG yDest, LONG xSize, LONG ySize, struct GfxBase *GfxBase)
{
    Object *gc;
    struct BitMap template_bm;
    
    struct blttemplate_render_data btrd;
    struct Rectangle rr;
    
    struct template_info ti;
    
    struct TagItem bm_tags[] = 
    {
    	{ aHidd_BitMap_Width,	xSize },
	{ aHidd_BitMap_Height,	ySize },
	{ aHidd_BitMap_Depth,	1 },
	{ aHidd_BitMap_Displayable, FALSE },
	{ TAG_DONE, 0UL }
    };
    
    struct TagItem gc_tags[] = {
    	{ aHidd_GC_DrawMode,	vHidd_GC_DrawMode_Copy },
	{ TAG_DONE, 0UL }
    };
    HIDDT_DrawMode old_drmd;

    EnterFunc(bug("driver_BltTemplate(%d, %d, %d, %d, %d, %d)\n"
    	, xSrc, srcMod, xDest, yDest, xSize, ySize));
	
    if (!CorrectDriverData(destRP, GfxBase))
    	ReturnVoid("driver_BltTemplate");
	
    gc = GetDriverData(destRP)->dd_GC;
    
    
    HIDD_BM_PIXTAB(&template_bm)	= NULL;
    template_bm.Rows		= ySize;
    template_bm.BytesPerRow	= WIDTH_TO_BYTES(xSize);
    template_bm.Depth		= 1;
    template_bm.Flags		= BMF_AROS_HIDD;
    
    
    /* Create an offscreen HIDD bitmap of depth 1 to use in color expansion */
    HIDD_BM_OBJ(&template_bm) = HIDD_Gfx_NewBitMap(SDD(GfxBase)->gfxhidd, bm_tags);
    if (!HIDD_BM_OBJ(&template_bm))
    	ReturnVoid("driver_BltTemplate");
	
    /* Copy contents from Amiga bitmap to the offscreen HIDD bitmap */
    ti.source	 = source;
    ti.modulo	 = srcMod;
    ti.invertsrc = ((GetDrMd(destRP) & INVERSVID) ? TRUE : FALSE);

D(bug("Copying template to HIDD offscreen bitmap\n"));


    /* Preserve state */
    GetAttr(gc, aHidd_GC_DrawMode, &old_drmd);
    SetAttrs(gc, gc_tags);

    amiga2hidd_fast( (APTR)&ti
    	, gc
    	, xSrc, 0
	, &template_bm
	, 0, 0
	, xSize, ySize
	, template_to_buf
    );
    
    /* Reset to preserved state */
    gc_tags[0].ti_Data = old_drmd;
    SetAttrs(gc, gc_tags);
    
    btrd.template_bm = HIDD_BM_OBJ(&template_bm);
    
    rr.MinX = xDest;
    rr.MinY = yDest;
    rr.MaxX = xDest + xSize - 1;
    rr.MaxY = yDest + ySize - 1;
    
    do_render_func(destRP, NULL, &rr, blttemplate_render, &btrd, FALSE, GfxBase);

    HIDD_Gfx_DisposeBitMap(SDD(GfxBase)->gfxhidd, HIDD_BM_OBJ(&template_bm));
	
    ReturnVoid("driver_BltTemplate");
}


struct pattern_info
{
    PLANEPTR mask;
    struct RastPort *rp;
    LONG mask_bpr; /* Butes per row */
    
    LONG orig_xmin;
    LONG orig_ymin;
    
    UBYTE dest_depth;
    
    struct GfxBase * gfxbase;
    
};

#define GfxBase (pi->gfxbase)
struct bltpattern_render_data {
     struct render_special_info rsi;
     struct pattern_info *pi;
};

static VOID pattern_to_buf(struct pattern_info *pi
	, LONG x_src, LONG y_src
	, LONG x_dest, LONG y_dest
	, ULONG xsize, ULONG ysize
	, ULONG *buf
	, Object *dest_bm
	, HIDDT_Pixel *coltab
)
{

    /* x_src, y_src is the coordinates into the layer. */
    LONG y;
    struct RastPort *rp;
    ULONG apen;
    ULONG bpen;
    UBYTE *apt;
    LONG mask_x, mask_y;
    
    rp = pi->rp;
    apen = GetAPen(rp);
    bpen = GetBPen(rp);
    apt = (UBYTE *)rp->AreaPtrn;
    
    if (pi->mask)
    {
    	mask_x = x_src - pi->orig_xmin;
	mask_y = y_src - pi->orig_ymin;
    }

    EnterFunc(bug("pattern_to_buf(%p, %d, %d, %d, %d, %d, %d, %p)\n"
    			, pi, x_src, y_src, x_dest, y_dest, xsize, ysize, buf ));
			

    HIDD_BM_GetImage(dest_bm
    	, (UBYTE *)buf
	, xsize * sizeof (HIDDT_Pixel)
	, x_dest, y_dest
	, xsize, ysize
	, vHidd_PixFmt_Native32
    );

    
    for (y = 0; y < ysize; y ++)
    {
        LONG x;
	
	for (x = 0; x < xsize; x ++)
	{
	    ULONG set_pixel;
	    ULONG pixval;
	    
	    /* Mask supplied ? */
	    if (pi->mask)
	    {
		ULONG idx, mask;


		idx = COORD_TO_BYTEIDX(x + mask_x, y + mask_y, pi->mask_bpr);
		mask = XCOORD_TO_MASK(x + mask_x);
		 
		set_pixel = pi->mask[idx] & mask;
		 
	    }
	    else
	        set_pixel = 1UL;
		
		
	    if (set_pixel)
	    {
		if (apt)
		{
		   set_pixel = pattern_pen(rp, x + x_src, y + y_src, apen, bpen, &pixval, GfxBase);
		   if (set_pixel)
		   {
		   	D(bug(" s"));
		    	*buf = (coltab != NULL) ? coltab[pixval] : pixval;
		   }
		    
		}
		else
		    *buf = apen;
	    
	    } /* if (pixel should be set */
	    

	    D(bug("(%d, %d): %d", x, y, *buf));
	    buf ++;
	    
	} /* for (each column) */
	
    } /* for (each row) */
    
    
    ReturnVoid("pattern_to_buf");
}


#undef GfxBase



static ULONG bltpattern_render(APTR bpr_data
	, LONG srcx, LONG srcy
	, Object *dstbm_obj
	, Object *dst_gc
	, LONG x1, LONG y1, LONG x2, LONG y2
	, struct GfxBase *GfxBase)
{
    struct bltpattern_render_data *bprd;
    
    
    ULONG width, height;
    
    bprd = (struct bltpattern_render_data *)bpr_data;
    
    width  = x2 - x1 + 1;
    height = y2 - y1 + 1;
    
    amiga2hidd_fast( (APTR)bprd->pi
    	, dst_gc
	, bprd->rsi.layer_rel_srcx
	, bprd->rsi.layer_rel_srcy
	, bprd->rsi.curbm
	, x1, y1
	, width, height
	, pattern_to_buf
    );
    
    return width * height;
    
}

VOID driver_BltPattern(struct RastPort *rp, PLANEPTR mask, LONG xMin, LONG yMin,
		LONG xMax, LONG yMax, ULONG byteCnt, struct GfxBase *GfxBase)
{

    struct pattern_info pi;
    struct bltpattern_render_data bprd;
    struct Rectangle rr;
    
    Object *gc;
    HIDDT_DrawMode old_drmd;
    struct TagItem gc_tags[] = {
	{ aHidd_GC_DrawMode,	vHidd_GC_DrawMode_Copy	},
	{ TAG_DONE, 0UL }
    };
    
    
    EnterFunc(bug("driver_BltPattern(%d, %d, %d, %d, %d)\n"
    	, xMin, yMin, xMax, yMax, byteCnt));
	
    if (!CorrectDriverData(rp, GfxBase))
    	ReturnVoid("driver_BltPattern");

    pi.mask	= mask;
    pi.rp	= rp;
    pi.gfxbase	= GfxBase;
    pi.mask_bpr = byteCnt;
    pi.dest_depth	= GetBitMapAttr(rp->BitMap, BMA_DEPTH);

    pi.orig_xmin = xMin;
    pi.orig_ymin = yMin;
    
    bprd.pi = &pi;
	
    gc = GetDriverData(rp)->dd_GC;
    
    GetAttr(gc, aHidd_GC_DrawMode, &old_drmd);
    SetAttrs(gc, gc_tags);
    
    rr.MinX = xMin;
    rr.MinY = yMin;
    rr.MaxX = xMax;
    rr.MaxY = yMax;
    
    do_render_func(rp, NULL, &rr, bltpattern_render, &bprd, TRUE, GfxBase);
    
    gc_tags[0].ti_Data = old_drmd;
    SetAttrs(gc, gc_tags);

    ReturnVoid("driver_BltPattern");
}

VOID driver_WriteChunkyPixels(struct RastPort * rp, ULONG xstart, ULONG ystart,
		ULONG xstop, ULONG ystop, UBYTE * array,
		LONG bytesperrow, struct GfxBase *GfxBase)
{
    driver_WritePixelArray8(rp 
		, xstart, ystart
		, xstop, ystop
		, array, NULL
		, GfxBase
    );

}

LONG driver_ReadPixelLine8 (struct RastPort * rp, ULONG xstart,
			    ULONG ystart, ULONG width,
			    UBYTE * array, struct RastPort * temprp,
			    struct GfxBase *GfxBase)
{
    /* We are lazy, and waste som cycles to be able to reuse what we've
       allready done
     */
    
    return driver_ReadPixelArray8(rp
    	, xstart, ystart
	, xstart + width - 1, ystart
	, array, temprp
	, GfxBase
    );
}		    


LONG driver_WritePixelLine8 (struct RastPort * rp, ULONG xstart,
			    ULONG ystart, ULONG width,
			    UBYTE * array, struct RastPort * temprp,
			    struct GfxBase *GfxBase)
{
    /* We are lazy, and waste som cycles to be able to reuse what we've
       allready done
     */
    return driver_WritePixelArray8(rp
    	, xstart, ystart
	, xstart + width - 1, ystart
	, array, temprp
	, GfxBase
    );
}


struct layerhookmsg
{
    struct Layer *Layer;
/*  struct Rectangle rect; (replaced by the next line!) */
    WORD MinX, MinY, MaxX, MaxY;
    LONG OffsetX, OffsetY;
};


VOID calllayerhook(struct Hook *h, struct RastPort *rp, struct layerhookmsg *msg)
{
    struct BitMap *bm = rp->BitMap;
    Object *gc;
    
    if (!CorrectDriverData(rp, GfxBase)) 
    	return;
	
	
    gc = GetDriverData(rp)->dd_GC;
    
    if(h == LAYERS_BACKFILL)
    {
	Object *bm_obj;
	
	bm_obj = OBTAIN_HIDD_BM(bm);
	if (NULL != bm_obj) {
	
	     HIDDT_DrawMode old_drmd;
	     HIDDT_Pixel old_fg;
	     
	     struct TagItem gc_tags[] =
	     {
	     	{aHidd_GC_Foreground, 0UL},
		{aHidd_GC_DrawMode,	  vHidd_GC_DrawMode_Copy},
		{TAG_DONE, 0UL}
	     };
	     
	     GetAttr(gc, aHidd_GC_DrawMode,   &old_drmd);
	     GetAttr(gc, aHidd_GC_Foreground, &old_fg);

	     gc_tags[0].ti_Data = BM_PIXEL(rp->BitMap, 0);

	     SetAttrs(gc, gc_tags);
		    
	     /* Cliprect not obscured, so we may render directly into the display */
	     HIDD_BM_FillRect(bm_obj
	     	, gc
		, msg->MinX, msg->MinY
		, msg->MaxX, msg->MaxY
	     );
	     
	     gc_tags[0].ti_Data = old_fg;
	     gc_tags[1].ti_Data = old_drmd;
	     
	     SetAttrs(gc, gc_tags);
	     
	     RELEASE_HIDD_BM(bm_obj, bm);
	     
	}
    }

    else if(h != LAYERS_NOBACKFILL)
    {
	/* Call user specified hook */
	AROS_UFC3(void, h->h_Entry,
	    AROS_UFCA(struct Hook *,         h,    A0),
	    AROS_UFCA(struct RastPort *,     rp,   A2),
	    AROS_UFCA(struct layerhookmsg *, msg, A1)
	);
    }
}



struct eraserect_render_data {
    struct render_special_info rsi;
    struct RastPort *origrp;
    struct RastPort *fakerp;
};

static ULONG eraserect_render(APTR err_data
	, LONG srcx, LONG srcy
	, Object *dstbm_obj, Object *gc
	, LONG x1, LONG y1, LONG x2, LONG y2
	, struct GfxBase *GfxBase)
{

    struct layerhookmsg msg;
    struct eraserect_render_data *errd;
    struct RastPort *rp;
    
    errd = (struct eraserect_render_data *)err_data;
    
    rp = errd->origrp;
     
    msg.Layer	= rp->Layer;
    msg.MinX	= x1;
    msg.MinY	= y1;
    msg.MaxX	= x2;
    msg.MaxY	= y2;
     
#warning What should these be set to ?
    msg.OffsetX = 0;
    msg.OffsetY = 0;
     
    if (NULL != msg.Layer) {
    	struct RastPort *rp = NULL;
	
        if (!errd->rsi.onscreen) {
	    if (NULL == errd->fakerp)
		errd->fakerp = CreateRastPort();
	    if (NULL == errd->fakerp)
		return 0;
		
	    rp = errd->fakerp;
	    rp->BitMap = errd->rsi.curbm;
	    
	} else {
	    rp = errd->origrp;
	}
        
     	calllayerhook(msg.Layer->BackFill, rp, &msg);
    }
    
    return 0;
}

void driver_EraseRect (struct RastPort * rp, LONG x1, LONG y1, LONG x2, LONG y2,
		    struct GfxBase * GfxBase)
{

    struct eraserect_render_data errd;
    struct Rectangle rr;
    
    EnterFunc(bug("driver_EraseRect(%d, %d, %d, %d)\n", x1, y1, x2, y2));
    if (!CorrectDriverData(rp, GfxBase))
    	ReturnVoid("driver_EraseRect(No driverdata)");
	
    errd.origrp = rp;
    errd.fakerp = NULL;
    
    rr.MinX = x1;
    rr.MinY = y1;
    rr.MaxX = x2;
    rr.MaxY = y2;
    
    do_render_func(rp, NULL, &rr, eraserect_render, &errd, TRUE, GfxBase);
    
    if (NULL != errd.fakerp)
    	FreeRastPort(errd.fakerp);
    
    return;
}

/*********** BltMaskBitMapRastPort() ***************************/

struct bltmask_info
{
    PLANEPTR mask;
    LONG mask_xmin;
    LONG mask_ymin;
    ULONG mask_bpr;
    struct BitMap *srcbm;
    struct GfxBase *GfxBase;
};


static VOID bltmask_to_buf(struct bltmask_info *bmi
	, LONG x_src, LONG y_src
	, LONG x_dest, LONG y_dest
	, ULONG xsize, ULONG ysize
	, ULONG *buf
	, Object *dest_bm
	, HIDDT_Pixel *coltab
)
{	
    /* x_src, y_src is the coordinates int the layer. */
    LONG y;
    UBYTE src_depth;

    EnterFunc(bug("bltmask_to_buf(%p, %d, %d, %d, %d, %p)\n"
    			, bmi, x_src, y_src, xsize, ysize, buf ));

    src_depth = GetBitMapAttr(bmi->srcbm, BMA_DEPTH);
    
    /* We must get the data from the destination bitmap */
    HIDD_BM_GetImage(dest_bm
    	, (UBYTE *)buf
	, xsize * sizeof (HIDDT_Pixel)
	, x_dest, y_dest
	, xsize, ysize
	, vHidd_PixFmt_Native32
    );
			
    
    for (y = 0; y < ysize; y ++)
    {
        LONG x;
	
	for (x = 0; x < xsize; x ++)
	{
	    ULONG set_pixel;
	    
	    ULONG idx, mask;
	    idx = COORD_TO_BYTEIDX(x + bmi->mask_xmin, y + bmi->mask_ymin, bmi->mask_bpr);
	    mask = XCOORD_TO_MASK(x + bmi->mask_xmin);
		 
	    set_pixel = bmi->mask[idx] & mask;
		
	    if (set_pixel)
	    {
		  UBYTE pen;
		  
		  pen = getbitmappixel(bmi->srcbm
		  	, x + x_src
			, y + y_src
			, src_depth
			, 0xFF
		   );
		   
		  *buf = (coltab != NULL) ? coltab[pen] : pen;
	    }
	    
	    buf ++;
	    
	} /* for (each column) */
	
    } /* for (each row) */

    
    ReturnVoid("bltmask_to_buf");
}


#define APPLY_MINTERM(pen, src, dest, minterm) \
	pen = 0;	\
	if ((minterm) & 0x0010)	pen = (~(src) & ~(dest));	\
	if ((minterm) & 0x0020)	pen = (~(src) &  (dest));	\
	if ((minterm) & 0x0040)	pen = ( (src) & ~(dest));	\
	if ((minterm) & 0x0080)	pen = ( (src) &  (dest));
	

VOID driver_BltMaskBitMapRastPort(struct BitMap *srcBitMap
    		, LONG xSrc, LONG ySrc
		, struct RastPort *destRP
		, LONG xDest, LONG yDest
		, ULONG xSize, ULONG ySize
		, ULONG minterm
		, PLANEPTR bltMask
		, struct GfxBase *GfxBase )
{
    ULONG width, height;
    struct Layer *L = destRP->Layer;
    struct BitMap *bm = destRP->BitMap;
    struct bltmask_info bmi;
    
    HIDDT_DrawMode old_drmd;
    Object *gc;
    
    struct TagItem gc_tags[] =
    {
	{ aHidd_GC_DrawMode, 0UL },
	{ TAG_DONE, 0UL }
    };
    
    
    EnterFunc(bug("driver_BltMaskBitMapRastPort(%d, %d, %d, %d, %d, %d)\n"
    		, xSrc, ySrc, xDest, yDest, xSize, ySize));

    if (!CorrectDriverData(destRP, GfxBase))
    	ReturnVoid("driver_BltMaskBitMapRastPort");
    
    gc = GetDriverData(destRP)->dd_GC;
    
    gc_tags[0].ti_Data = MINTERM_TO_GCDRMD(minterm);
    
    GetAttr(gc, aHidd_GC_DrawMode, &old_drmd);
    SetAttrs(gc, gc_tags);

    bmi.mask	= bltMask;
    bmi.srcbm	= srcBitMap;
    bmi.GfxBase	= GfxBase;
    
    bmi.mask_bpr = WIDTH_TO_WORDS(xSize);
    
    /* Set minterm bitmap object */
    
    if (NULL == L)
    {
        /* No layer, probably a screen */
	
	bmi.mask_xmin = 0;
	bmi.mask_ymin = 0;
	
	amiga2hidd_fast( (APTR) &bmi
		, gc
		, xSrc, ySrc
		, bm
		, xDest, yDest
		, xSize
		, ySize
		, bltmask_to_buf
	);
	
    }
    else
    {
        struct ClipRect *CR;
	WORD xrel;
        WORD yrel;
	struct Rectangle toblit, intersect;
	
	LockLayerRom( L );
	
	CR = L->ClipRect;
	xrel = L->bounds.MinX;
	yrel = L->bounds.MinY;
	
	toblit.MinX = xDest + xrel;
	toblit.MinY = yDest + yrel;
	toblit.MaxX = (xDest + xSize - 1) + xrel;
	toblit.MaxY = (yDest + ySize - 1) + yrel;
	
	
	for (;NULL != CR; CR = CR->Next)
	{
	    D(bug("Cliprect (%d, %d, %d, %d), lobs=%p\n",
	    	CR->bounds.MinX, CR->bounds.MinY, CR->bounds.MaxX, CR->bounds.MaxY,
		CR->lobs));
		
	    /* Does this cliprect intersect with area to blit ? */
	    if (andrectrect(&CR->bounds, &toblit, &intersect))
	    {
	        ULONG xoffset = intersect.MinX - toblit.MinX;
		ULONG yoffset = intersect.MinY - toblit.MinY;
		
		bmi.mask_xmin = xoffset;
		bmi.mask_ymin = yoffset;
	    
	        if (NULL == CR->lobs)
		{
		    
		    /* Cliprect not obscured, so we may render directly into the display */
		    amiga2hidd_fast( (APTR) &bmi
		    	, gc
			, xSrc + xoffset, ySrc + yoffset
			, bm
			, intersect.MinX, intersect.MinY
			, width
			, height
			, bltmask_to_buf
		     );
		}
		else
		{
		    /* Render into offscreen cliprect bitmap */
		    if (L->Flags & LAYERSIMPLE)
		    	continue;
		    else if (L->Flags & LAYERSUPER)
		    	kprintf("driver_BltMaskBitMapRastPort(): Superbitmap not handled yet\n");
		    else
		    {
		    
		    
		    
		    	amiga2hidd_fast( (APTR) &bmi
				, gc
				, xSrc + xoffset, ySrc + yoffset
				, CR->BitMap
		    		, intersect.MinX - CR->bounds.MinX + ALIGN_OFFSET(CR->bounds.MinX)
				, intersect.MinY - CR->bounds.MinY
				, width
				, height
				, bltmask_to_buf
		     	);

		    }
		}
	    }
	}
	UnlockLayerRom( L );
	
    }
    
    /* Reset to told drawmode value */
    gc_tags[0].ti_Data = (IPTR)old_drmd;
    SetAttrs(gc, gc_tags);
	

    ReturnVoid("driver_BltMaskBitMapRastPort");
}


static Object *fontbm_to_hiddbm(struct TextFont *font, struct GfxBase *GfxBase)
{
    ULONG width, height;
    Object *bm_obj;
    Object *tmp_gc;
    /* Caclulate sizes for the font bitmap */
    struct TagItem bm_tags[] = {
	{ aHidd_BitMap_Width,		0	},
	{ aHidd_BitMap_Height,		0	},
	{ aHidd_BitMap_Displayable,	FALSE	},
	{ aHidd_BitMap_Depth,		1	},
	{ TAG_DONE,	0UL }
    };
    
    tmp_gc = obtain_cache_object(SDD(GfxBase)->gc_cache, GfxBase);
    if (NULL == tmp_gc)
    	return NULL;

    width  = font->tf_Modulo * 8;
    height = font->tf_YSize;
    
    bm_tags[0].ti_Data = width;
    bm_tags[1].ti_Data = height;
	    
#warning Handle color textfonts
	    
    bm_obj = HIDD_Gfx_NewBitMap(SDD(GfxBase)->gfxhidd, bm_tags);
    if (NULL != bm_obj)
    {
    	struct template_info ti;
    	struct BitMap bm;
	struct TagItem gc_tags[] = {
	    { aHidd_GC_DrawMode,	vHidd_GC_DrawMode_Copy },
	    { TAG_DONE, 0UL }
	};
	
	HIDD_BM_OBJ(&bm)	= bm_obj;
	HIDD_BM_COLMAP(&bm)	= NULL;
	HIDD_BM_GRAPHTYPE(&bm)	= vHidd_GT_Palette;
	
	bm.Rows		= height;
	bm.BytesPerRow	= WIDTH_TO_BYTES(width);
	bm.Depth	= 1;
	bm.Flags	= BMF_AROS_HIDD;
	
	ti.source	= font->tf_CharData;
	ti.x_src	= 0;
	ti.modulo	= font->tf_Modulo;
	ti.invertsrc	= FALSE;
		
    	/* Copy the character data into the bitmap */
	SetAttrs(tmp_gc, gc_tags);
	
	amiga2hidd_fast((APTR)&ti
		, tmp_gc
		, 0, 0
		, &bm
		, 0, 0
		, width, height
		, template_to_buf
	);
		
    }
    
    release_cache_object(SDD(GfxBase)->gc_cache, tmp_gc, GfxBase);
    
    return bm_obj;
}


static inline Object *get_planarbm_object(struct BitMap *bitmap, struct GfxBase *GfxBase)
{
    Object *pbm_obj;

kprintf("get_planarbm_object()\n");    
    pbm_obj = obtain_cache_object(SDD(GfxBase)->planarbm_cache, GfxBase);
kprintf("Got cache object %p, class=%s, domethod=%p, instoffset=%d\n"
	, pbm_obj
	, OCLASS(pbm_obj)->ClassNode.ln_Name
	, OCLASS(pbm_obj)->DoMethod
	, OCLASS(pbm_obj)->InstOffset
);
    
    if (NULL != pbm_obj) {
    	HIDD_PlanarBM_SetBitMap(pbm_obj, bitmap);
    }
    
    return pbm_obj;
}

ULONG do_pixel_func(struct RastPort *rp
	, LONG x, LONG y
	, LONG (*render_func)(APTR, Object *, Object *, LONG, LONG, struct GfxBase *)
	, APTR funcdata
	, struct GfxBase *GfxBase)
{
    struct BitMap *bm = rp->BitMap;
    struct Layer *L = rp->Layer;
    Object *gc;
    ULONG retval = -1;
   
    gc = GetDriverData(rp)->dd_GC;
   
    if (NULL == L) {
	Object *bm_obj;
	ULONG width, height;
	
	bm_obj = OBTAIN_HIDD_BM(bm);
	
	
	if (NULL == bm_obj)
	    return -1;
	    
	GetAttr(bm_obj, aoHidd_BitMap_Width,  &width);
	GetAttr(bm_obj, aoHidd_BitMap_Height, &height);

	/* Check whether we it is inside the rastport */
	if (	x <  0
	     || x >= width
	     || y <  0
	     || y >= height) {
	     
	     RELEASE_HIDD_BM(bm_obj, bm);
	     return -1;
	     
	}
	
    	/* This is a screen */
	retval = render_func(funcdata, bm_obj, gc, x, y, GfxBase);
	
	RELEASE_HIDD_BM(bm_obj, bm);
	
    } else {
        struct ClipRect *CR;
	LONG absx, absy;
	
	LockLayerRom( L );
	
	CR = L->ClipRect;
	
	absx = x + L->bounds.MinX;
	absy = y + L->bounds.MinY;
	
	for (;NULL != CR; CR = CR->Next) {
	
	    if (    absx >= CR->bounds.MinX
	         && absy >= CR->bounds.MinY
		 && absx <= CR->bounds.MaxX
		 && absy <= CR->bounds.MaxY ) {


	
	        if (NULL == CR->lobs) {
		    retval = render_func(funcdata
		    	, HIDD_BM_OBJ(bm), gc
			, absx, absy
			, GfxBase
		    );
		} else {
		    /* This is the tricky one: render into offscreen cliprect bitmap */
		    if (L->Flags & LAYERSIMPLE) {
		    	/* We cannot do anything */
		    	retval =  0;
		
		    } else if (L->Flags & LAYERSUPER)
		    	kprintf("driver_WriteRGBPixel(): Superbitmap not handled yet\n");
		    else
		    {
			retval = render_func(funcdata
				, HIDD_BM_OBJ(CR->BitMap), gc
				, absx - CR->bounds.MinX + ALIGN_OFFSET(CR->bounds.MinX)
				, absy - CR->bounds.MinY
				, GfxBase
			); 


		    } /* If (SMARTREFRESH cliprect) */
		    
		    
		}   /* if (intersecton inside hidden cliprect) */
		
		/* The pixel was found and put inside one of the cliprects, just exit */
		break;

	    } /* if (cliprect intersects with area we want to draw to) */
	    
	} /* while (cliprects to examine) */
	
	UnlockLayerRom( L );
    
    }
    
    return retval;

}

#define RSI(x) ((struct render_special_info *)x)
ULONG do_render_func(struct RastPort *rp
	, Point *src
	, struct Rectangle *rr
	, ULONG (*render_func)(APTR, LONG, LONG, Object *, Object *, LONG, LONG, LONG, LONG, struct GfxBase *)
	, APTR funcdata
	, BOOL get_special_info
	, struct GfxBase *GfxBase)
{

    struct BitMap *bm = rp->BitMap;
    struct Layer *L = rp->Layer;
    Object *gc;
    ULONG width, height;
    LONG srcx, srcy;
    
    LONG pixwritten = 0;
    
    gc = GetDriverData(rp)->dd_GC;
	
    width  = rr->MaxX - rr->MinX + 1;
    height = rr->MaxY - rr->MinY + 1;
    
    if (NULL != src) {
        srcx = src->x;
	srcy = src->y;
    } else {
    	srcx = 0;
	srcy = 0;
    }
    
    if (NULL == L)
    {
        /* No layer, probably a screen, but may be a user inited bitmap */
	Object *bm_obj;
	
	bm_obj = OBTAIN_HIDD_BM(bm);
	if (NULL == bm_obj)
	    return 0;
	    
	if (get_special_info) {
	    RSI(funcdata)->curbm    = rp->BitMap;
	    RSI(funcdata)->onscreen = TRUE;
	    RSI(funcdata)->layer_rel_srcx = srcx;
	    RSI(funcdata)->layer_rel_srcy = srcy;
	}
	    
	pixwritten = render_func(funcdata
		, srcx, srcy
		, bm_obj, gc
		, rr->MinX, rr->MinY
		, rr->MaxX, rr->MaxY
		, GfxBase
	);

    }
    else
    {
        struct ClipRect *CR;
	WORD xrel;
        WORD yrel;
	struct Rectangle torender, intersect;
	
	LockLayerRom(L);
	
	xrel = L->bounds.MinX;
	yrel = L->bounds.MinY;

	torender.MinX = rr->MinX + xrel;
	torender.MinY = rr->MinY + yrel;
	torender.MaxX = rr->MaxX + xrel;
	torender.MaxY = rr->MaxY + yrel;
	
	
	CR = L->ClipRect;
	
	for (;NULL != CR; CR = CR->Next)
	{
	    D(bug("Cliprect (%d, %d, %d, %d), lobs=%p\n",
	    	CR->bounds.MinX, CR->bounds.MinY, CR->bounds.MaxX, CR->bounds.MaxY,
		CR->lobs));
		
	    /* Does this cliprect intersect with area to rectfill ? */
	    if (andrectrect(&CR->bounds, &torender, &intersect))
	    {
	    	LONG xoffset, yoffset;
		
		xoffset = intersect.MinX - torender.MinX;
		yoffset = intersect.MinY - torender.MinY;
		
		if (get_special_info) {
		     RSI(funcdata)->layer_rel_srcx = intersect.MinX - L->bounds.MinX;
		     RSI(funcdata)->layer_rel_srcy = intersect.MinY - L->bounds.MinY;
		}
		
	        if (NULL == CR->lobs)
		{
		    if (get_special_info) {
			RSI(funcdata)->curbm = bm;
			RSI(funcdata)->onscreen = TRUE;
		    }
		    
		    pixwritten += render_func(funcdata
		    	, srcx + xoffset
			, srcy + yoffset
		        , HIDD_BM_OBJ(bm)
		    	, gc
		    	, intersect.MinX
			, intersect.MinY
			, intersect.MaxX
			, intersect.MaxY
			, GfxBase
		    );
		
		
		}
		else
		{
		    /* Render into offscreen cliprect bitmap */
		    if (L->Flags & LAYERSIMPLE)
		    	continue;
		    else if (L->Flags & LAYERSUPER)
		    	kprintf("do_render_func(): Superbitmap not handled yet\n");
		    else
		    {

		    	if (get_special_info) {
			    RSI(funcdata)->curbm = CR->BitMap;
			    RSI(funcdata)->onscreen = FALSE;
		    	}
			pixwritten += render_func(funcdata
				, srcx + xoffset, srcy + yoffset
		        	, HIDD_BM_OBJ(CR->BitMap)
		    		, gc
		    		, intersect.MinX - CR->bounds.MinX + ALIGN_OFFSET(CR->bounds.MinX)
				, intersect.MinY - CR->bounds.MinY
				, intersect.MaxX - CR->bounds.MinX + ALIGN_OFFSET(CR->bounds.MinX) 
				, intersect.MaxY - CR->bounds.MinY
				, GfxBase
		    	);
		    }
		    
		} /* if (CR->lobs == NULL) */
		
	    } /* if (cliprect intersects with area to render into) */
	    
	} /* for (each cliprect in the layer) */
	
        UnlockLayerRom(L);
    } /* if (rp->Layer) */
    
	
    return pixwritten;

}

/***********************************************/
/* CYBERGFX CALLS                            ***/


#include <proto/cybergraphics.h>

struct wpa_render_data {
    UBYTE *array;
    HIDDT_StdPixFmt pixfmt;
    ULONG modulo;
    ULONG bppix;
};

static ULONG wpa_render(APTR wpar_data
	, LONG srcx, LONG srcy
	, Object *dstbm_obj
	, Object *dst_gc
	, LONG x1, LONG y1, LONG x2, LONG y2
	, struct GfxBase *GfxBase)
{
    struct wpa_render_data *wpard;
    ULONG width, height;
    UBYTE *array;
    
    width  = x2 - x1 + 1;
    height = y2 - y1 + 1;
    
    wpard = (struct wpa_render_data *)wpar_data;
    
    array = wpard->array + wpard->modulo * srcy + wpard->bppix * srcx;
    
    HIDD_BM_PutImage(dstbm_obj
    	, dst_gc, wpard->array
	, wpard->modulo
	, x1, y1
	, width, height
	, wpard->pixfmt
    );
    
    return width * height;
}


struct rpa_render_data {
    UBYTE *array;
    HIDDT_StdPixFmt pixfmt;
    ULONG modulo;
    ULONG bppix;
};

static ULONG rpa_render(APTR rpar_data
	, LONG srcx, LONG srcy
	, Object *dstbm_obj
	, Object *dst_gc
	, LONG x1, LONG y1, LONG x2, LONG y2
	, struct GfxBase *GfxBase)
{
    struct rpa_render_data *rpard;
    ULONG width, height;
    UBYTE *array;
    
    width  = x2 - x1 + 1;
    height = y2 - y1 + 1;
    
    rpard = (struct rpa_render_data *)rpar_data;
    
    array = rpard->array + rpard->modulo * srcy + rpard->bppix * srcx;
    
    HIDD_BM_GetImage(dstbm_obj
    	, rpard->array
	, rpard->modulo
	, x1, y1
	, width, height
	, rpard->pixfmt
    );
    
    return width * height;
}

static LONG pix_write(APTR pr_data
	, Object *bm, Object *gc
	, LONG x, LONG y
	, struct GfxBase *GfxBase)
{
    struct pix_render_data *prd;
    prd = (struct pix_render_data *)pr_data;
    
    HIDD_BM_PutPixel(bm, x, y, prd->pixel);
    
    return 0;
}

static LONG pix_read(APTR pr_data
	, Object *bm, Object *gc
	, LONG x, LONG y
	, struct GfxBase *GfxBase)
{
    struct pix_render_data *prd;
    
    prd = (struct pix_render_data *)pr_data;
    
    prd->pixel = HIDD_BM_GetPixel(bm, x, y);

    
    return 0;
}    



#include "cybergraphics_intern.h"



LONG driver_WriteLUTPixelArray(APTR srcrect, 
	UWORD srcx, UWORD srcy,
	UWORD srcmod, struct RastPort *rp, APTR ctable,
	UWORD destx, UWORD desty,
	UWORD sizex, UWORD sizey,
	UBYTE ctabformat,
	struct Library *CyberGfxBase)
{
    ULONG depth;
    
    HIDDT_PixelLUT pixlut;
    HIDDT_Pixel pixtab[256];
    
    HIDDT_Color col;
    ULONG i;
    
    LONG pixwritten = 0;
    UBYTE *buf;
    
    /* This is cybergraphx. We only work wih HIDD bitmaps */
    if (!IS_HIDD_BM(rp->BitMap)) {
    	kprintf("!!!!! Trying to use CGFX call on non-hidd bitmap in WriteLUTPixelArray()!!!\n");
    	return 0;
    }
    
    pixlut.entries	= 256;
    pixlut.pixels	= pixtab;
    
    depth = GetBitMapAttr(rp->BitMap, BMA_DEPTH);
    
    /* This call does only support bitmaps with depth > 8. Use WritePixelArray8
       for other bitmaps
    */
    
    if (depth <= 8) {
    	kprintf("!!! TRYING TO USE WriteLUTPixelArray() ON BITMAP WITH DEPTH < 8\n");
    	return 0;
    }
	
    /* Curently only one format is supported */
    if (CTABFMT_XRGB8 != ctabformat) {
    	kprintf("!!! WriteLUTPixelArray() CALLED WITH UNSUPPORTED CTAB FORMAT %d\n"
		, ctabformat);
    	return 0;
    }
    col.alpha	= 0;
	
    /* Convert the coltab into native pixels */
    for (i = 0; i < 256; i ++) {
    	register ULONG rgb = ((ULONG *)ctable)[i];
    	col.red		= (HIDDT_ColComp)((rgb & 0x00FF0000) >> 8);
	col.green	= (HIDDT_ColComp)(rgb & 0x0000FF00);
	col.blue	= (HIDDT_ColComp)((rgb & 0x000000FF) << 8);
	
	pixtab[i] = HIDD_BM_MapColor(HIDD_BM_OBJ(rp->BitMap), &col);
    }
    
    buf = (UBYTE *)srcrect;
    
    buf += CHUNKY8_COORD_TO_BYTEIDX(srcx, srcy, srcmod);
    
    pixwritten = write_pixels_8(rp
    	, buf
	, srcmod
	, destx, desty
	, destx + sizex - 1, desty + sizey - 1
	, &pixlut
	, GfxBase);
    
    
    /* Now blit the colors onto the screen */
    
    return pixwritten;
}


LONG driver_WritePixelArray(APTR src, UWORD srcx, UWORD srcy
	, UWORD srcmod, struct RastPort *rp, UWORD destx, UWORD desty
	, UWORD width, UWORD height, UBYTE srcformat, struct Library *CyberGfxBase)
{
     
    Object *pf;
    HIDDT_StdPixFmt srcfmt_hidd;
    ULONG start_offset, bppix;
    
    LONG pixwritten = 0;
    
    struct wpa_render_data wpard;
    struct Rectangle rr;

    /* This is cybergraphx. We only work wih HIDD bitmaps */
    if (!IS_HIDD_BM(rp->BitMap)) {
    	kprintf("!!!!! Trying to use CGFX call on non-hidd bitmap in WritePixelArray() !!!\n");
    	return 0;
    }
    
    if (!CorrectDriverData (rp, GfxBase))
	return 0;
	
    if (RECTFMT_LUT8 == srcformat) {
    
	HIDDT_PixelLUT pixlut = { 256, HIDD_BM_PIXTAB(rp->BitMap) };
	UBYTE * array = (UBYTE *)src;
	
	if (rp->BitMap->Flags & BMF_SPECIALFMT) {
	    kprintf("!!! No CLUT in driver_WritePixelArray\n");
	    return 0;
	}
	
	array += CHUNKY8_COORD_TO_BYTEIDX(srcx, srcy, srcmod);
	
    	pixwritten = write_pixels_8(rp
		, array, srcmod
		, destx, desty
		, destx + width - 1, desty + height - 1
		, &pixlut
		, GfxBase);
		
	return pixwritten;
    }
    
    if (RECTFMT_GREY8 == srcformat) {
    	kprintf("!!! RECTFMT_GREY8 not yet handled in driver_WritePixelArray\n");
	return 0;
    }
    
    switch (srcformat) {
	case RECTFMT_RGB  : srcfmt_hidd = vHidd_PixFmt_RGB24;  break;
	case RECTFMT_RGBA : srcfmt_hidd = vHidd_PixFmt_RGBA32; break;
	case RECTFMT_ARGB : srcfmt_hidd = vHidd_PixFmt_ARGB32; break;
    }

    /* Compute the start of the array */

#warning Get rid of the below code ?
/* This can be done by passing the srcx and srcy parameters on to
   the HIDD bitmap and let it take care of it itself.
   This means that HIDD_BM_PutImage() gets a lot of parameters,
   which may not be necessary in real life.
   
   Compromise: convert from *CyberGfx* pixfmt to bppix using a table lookup.
   This is faster
*/
    pf = HIDD_BM_GetPixelFormat(HIDD_BM_OBJ(rp->BitMap), srcfmt_hidd);
    GetAttr(pf, aHidd_PixFmt_BytesPerPixel, &bppix);
    
    start_offset = srcy * srcmod + srcx * bppix;
        
    wpard.array	 = ((UBYTE *)src) + start_offset;
    wpard.pixfmt = srcfmt_hidd;
    wpard.modulo = srcmod;
    wpard.bppix	 = bppix;
    
    rr.MinX = destx;
    rr.MinY = desty;
    rr.MaxX = destx + width  - 1;
    rr.MaxY = desty + height - 1;
    
    pixwritten = do_render_func(rp, NULL, &rr, wpa_render, &wpard, FALSE, GfxBase);
    
    return pixwritten;
}

LONG driver_ReadPixelArray(APTR dst, UWORD destx, UWORD desty
	, UWORD dstmod, struct RastPort *rp, UWORD srcx, UWORD srcy
	, UWORD width, UWORD height, UBYTE dstformat, struct Library *CyberGfxBase)
{
     
    Object *pf;    
    HIDDT_StdPixFmt dstfmt_hidd;
    
    ULONG start_offset, bppix;
    
    LONG pixread = 0;
    HIDDT_DrawMode old_drmd;
    Object *gc;
    
    struct Rectangle rr;
    struct rpa_render_data rpard;

    struct TagItem gc_tags[] = {
	{ aHidd_GC_DrawMode, vHidd_GC_DrawMode_Copy},
	{ TAG_DONE, 0}
    };
    
    /* This is cybergraphx. We only work wih HIDD bitmaps */
    if (!IS_HIDD_BM(rp->BitMap)) {
    	kprintf("!!!!! Trying to use CGFX call on non-hidd bitmap in ReadPixelArray() !!!\n");
    	return 0;
    }
    
    if (!CorrectDriverData (rp, GfxBase))
	return 0;
	
    gc = GetDriverData(rp)->dd_GC;

   /* Preserve old drawmode */
    GetAttr(gc, aHidd_GC_DrawMode, &old_drmd);
    SetAttrs(gc, gc_tags);
    
    
    switch (dstformat) {
	case RECTFMT_RGB  : dstfmt_hidd = vHidd_PixFmt_RGB24;  break;
	case RECTFMT_RGBA : dstfmt_hidd = vHidd_PixFmt_RGBA32; break;
	case RECTFMT_ARGB : dstfmt_hidd = vHidd_PixFmt_ARGB32; break;
    }

#warning Get rid of the below code ?
/* This can be done by passing the srcx and srcy parameters on to
   the HIDD bitmap and let it take care of it itself.
   This means that HIDD_BM_PutImage() gets a lot of parameters,
   which may not be necessary in real life.
   
   Compromise: convert from *CyberGfx* pixfmt to bppix using a table lookup.
   This is faster
*/
    pf = HIDD_BM_GetPixelFormat(HIDD_BM_OBJ(rp->BitMap), dstfmt_hidd);
    GetAttr(pf, aHidd_PixFmt_BytesPerPixel, &bppix);
    
    start_offset = srcy * dstmod + srcx * bppix;
        
    rpard.array	 = ((UBYTE *)dst) + start_offset;
    rpard.pixfmt = dstfmt_hidd;
    rpard.modulo = dstmod;
    rpard.bppix	 = bppix;
    
    rr.MinX = srcx;
    rr.MinY = srcy;
    rr.MaxX = srcx + width  - 1;
    rr.MaxY = srcy + height - 1;
    
    pixread = do_render_func(rp, NULL, &rr, rpa_render, &rpard, FALSE, GfxBase);
    
    /* restore old gc values */
    gc_tags[0].ti_Data = (IPTR)old_drmd;
    SetAttrs(gc, gc_tags);
    
    return pixread;
}

LONG driver_InvertPixelArray(struct RastPort *rp
	, UWORD destx, UWORD desty, UWORD width, UWORD height
	, struct Library *CyberGfxBase)
{

    /* This is cybergraphx. We only work wih HIDD bitmaps */
    if (!IS_HIDD_BM(rp->BitMap)) {
    	kprintf("!!!!! Trying to use CGFX call on non-hidd bitmap InvertPixelArray() !!!\n");
    	return 0;
    }

   return (LONG)fillrect_pendrmd(rp
   	, destx, desty
	, destx + width  - 1
	, desty + height - 1
	, 0	/* Source pen does not matter */
	, vHidd_GC_DrawMode_Invert
	, GfxBase);
}

LONG driver_FillPixelArray(struct RastPort *rp
	, UWORD destx, UWORD desty, UWORD width, UWORD height
	, ULONG pixel, struct Library *CyberGfxBase) 
{
    HIDDT_Color col;
    HIDDT_Pixel pix;
    
    /* HIDDT_ColComp are 16 Bit */
    col.alpha	= (HIDDT_ColComp)((pixel >> 16) & 0x0000FF00);
    col.red	= (HIDDT_ColComp)((pixel >> 8) & 0x0000FF00);
    col.green	= (HIDDT_ColComp)(pixel & 0x0000FF00);
    col.blue	= (HIDDT_ColComp)((pixel << 8) & 0x0000FF00);
    
    pix = HIDD_BM_MapColor(HIDD_BM_OBJ(rp->BitMap), &col);

    return (LONG)fillrect_pendrmd(rp
	, destx, desty
	, destx + width  - 1
	, desty + height - 1
	, pix
	, vHidd_GC_DrawMode_Copy
	, GfxBase
    );
}

LONG driver_MovePixelArray(UWORD srcx, UWORD srcy, struct RastPort *rp
	, UWORD destx, UWORD desty, UWORD width, UWORD height
	, struct Library *CyberGfxBase)
{

    kprintf("driver_MovePixelArray() not implemented yet\n");
    
    return 0;
}



LONG driver_WriteRGBPixel(struct RastPort *rp, UWORD x, UWORD y
	, ULONG pixel, struct Library *CyberGfxBase)
{
    
    struct pix_render_data prd;
    
    /* Get the HIDD pixel val */
    HIDDT_Color col;
    
    /* This is cybergraphx. We only work wih HIDD bitmaps */
    if (!IS_HIDD_BM(rp->BitMap)) {
    	kprintf("!!!!! Trying to use CGFX call on non-hidd bitmap in WriteRGBPixel() !!!\n");
    	return 0;
    }

    /* HIDDT_ColComp are 16 Bit */
    
    col.alpha	= (HIDDT_ColComp)((pixel >> 16) & 0x0000FF00);
    col.red	= (HIDDT_ColComp)((pixel >> 8) & 0x0000FF00);
    col.green	= (HIDDT_ColComp)(pixel & 0x0000FF00);
    col.blue	= (HIDDT_ColComp)((pixel << 8) & 0x0000FF00);
    
    prd.pixel = HIDD_BM_MapColor(HIDD_BM_OBJ(rp->BitMap), &col);
    
    return do_pixel_func(rp, x, y, pix_write, &prd, GfxBase);
   
}


ULONG driver_ReadRGBPixel(struct RastPort *rp, UWORD x, UWORD y
	, struct Library *CyberGfxBase)
{
    struct pix_render_data prd;
    
    /* Get the HIDD pixel val */
    HIDDT_Color col;
    HIDDT_Pixel pix;
    LONG ret;
    
    /* This is cybergraphx. We only work wih HIDD bitmaps */
    if (!IS_HIDD_BM(rp->BitMap)) {
    	kprintf("!!!!! Trying to use CGFX call on non-hidd bitmap in ReadRGBPixel()!!!\n");
    	return (ULONG)-1;
    }
    
    ret = do_pixel_func(rp, x, y, pix_read, &prd, GfxBase);
    if (-1 == ret)
    	return (ULONG)-1;

    HIDD_BM_UnmapPixel(HIDD_BM_OBJ(rp->BitMap), prd.pixel, &col);

    /* HIDDT_ColComp are 16 Bit */
    
    pix =	  ((col.alpha & 0xFF00) << 16)
    		| ((col.red & 0xFF00) << 8)
		| (col.green & 0xFF00)
		| ((col.blue & 0xFF00) >> 8);
    
    return pix;
}


#warning Implement Display mode attributes in the below function
APTR driver_AllocCModeListTagList(struct TagItem *taglist, struct Library *CyberGfxBase )
{
    struct List *hiddmlist;
    struct TagItem *tag, *tstate;
    
    ULONG minwidth = 0;
    ULONG maxwidth = 0xFFFFFFFF;
    ULONG minheight = 0;
    ULONG maxheight = 0xFFFFFFFF;
    
    struct List *cybermlist = NULL;
    
    Object *gfxhidd;
    
    /* GetModeInfo tags */
    struct TagItem qgmtags[] =  {
    	{ tHidd_GfxMode_MinWidth,	0x0 },
	{ tHidd_GfxMode_MaxWidth,	0xFFFFFFFF },
	{ tHidd_GfxMode_MinHeight,	0x0 },
	{ tHidd_GfxMode_MaxHeight,	0xFFFFFFFF },
	{ tHidd_GfxMode_PixFmts,	(IPTR)NULL },
	{ TAG_DONE, 0UL }
    };
    
    UWORD *cmodelarray = NULL;
    
    gfxhidd = SDD(GfxBase)->gfxhidd;
    
    for (tstate = taglist; (tag = NextTagItem((const struct TagItem **)&tstate)); ) {
	switch (tag->ti_Tag) {
	    case CYBRMREQ_MinWidth:
	    	minwidth = (ULONG)tag->ti_Data;
		break;
		
	    case CYBRMREQ_MaxWidth:
	     	maxwidth = (ULONG)tag->ti_Data;
		break;
		
	    case CYBRMREQ_MinHeight:
	    	minheight = (ULONG)tag->ti_Data;
		break;
		
	    case CYBRMREQ_MaxHeight:
	    	maxheight = (ULONG)tag->ti_Data;
		break;
		
	    case CYBRMREQ_CModelArray:
	    	cmodelarray = (UWORD *)tag->ti_Data;
		break;
		
	    default:
	    	kprintf("!!! UNKNOWN TAG PASSED TO AllocCModeListTagList\n");
		break;
		
	
	} 	
    }
    
    /* Build taglist for the QueryGfxModes call */
    qgmtags[0].ti_Data = minwidth;
    qgmtags[1].ti_Data = maxwidth;
    qgmtags[2].ti_Data = minheight;
    qgmtags[3].ti_Data = maxheight;
    
    /* Ask the HIDD for the modes */
    hiddmlist = HIDD_Gfx_QueryGfxModes(gfxhidd, qgmtags);
    
    if (NULL != hiddmlist) {
	
	/* Allocate the exec list */
	cybermlist = AllocMem(sizeof (struct List), MEMF_CLEAR);
	if (NULL != cybermlist) {
    	    struct ModeNode *hmnode;
	    NEWLIST(cybermlist);
	    /* Convert the modeinfo into an exec list */
	
	    /* NOTE: If HIDD_GfxGetModeInfo returned != NULL, it means
	           we have at least one mode, ie. ine tHidd_GfxMode_Start/Stop pair
	    */	    
	    ForeachNode(hiddmlist, hmnode) {
	    
	    	struct CyberModeNode *cmnode;
		
		UWORD *cyberpixfmts;
		ULONG width, height, depth;
		
		
		/* Check whether the gfxmode is the correct pixel format */
		if (NULL != cmodelarray) {
		    HIDDT_StdPixFmt stdpf;
		    UWORD cyberpf;
		    BOOL found = FALSE;
			/* Get the gfxmode pixelf format */
		    GetAttr(hmnode->gfxMode, aHidd_PixFmt_StdPixFmt, &stdpf);
		
		    cyberpf = hidd2cyber_pixfmt(stdpf, GfxBase);
		    if (cyberpf == (UWORD)-1)
		    	continue;	/* Unknown format */
			
		    for (cyberpixfmts = cmodelarray; *cyberpixfmts; cyberpixfmts ++) {
		    	/* See if the stdpixfmt is present in the array */
			if (*cyberpixfmts == cyberpf) {
			    found = TRUE;
			    break;
			}
			
 		    } /* for (each supplied pixelformat in the cmodelarray) */
		    
		    if (!found)
		    	continue; /* PixFmt not wanted, just continue with next node */
		
		} /* if (cmodelarray supplied in the taglist) */
		
	    	/* Allocate a cybergfx modeinfo struct */
	    
	    	cmnode = AllocMem(sizeof (struct CyberModeNode), MEMF_CLEAR);
	    	if (NULL == cmnode)
		    goto failexit;
		    
		/* Get some info from the HIDD object */
		
		GetAttr(hmnode->gfxMode, aHidd_GfxMode_Width,  &width);
		GetAttr(hmnode->gfxMode, aHidd_GfxMode_Height, &height);
		GetAttr(hmnode->gfxMode, aHidd_PixFmt_Depth,   &depth);
		
		cmnode->Width	= width;
		cmnode->Height	= height;
		cmnode->Depth	= depth;
		cmnode->DisplayTagList = NULL;
		strncpy(cmnode->ModeText, "Blah", DISPLAYNAMELEN);
		
		/* Keep track of the node */
		AddTail(cybermlist, (struct Node *)cmnode);
		
		/* Get a display ID for the mode */
	
	    } /* while (hidd modes to process( */
	
	} /* if (NULL != cybermlist) */
	
    	HIDD_Gfx_ReleaseGfxModes(gfxhidd, hiddmlist);
    } /* if (NULL != hiddmlist) */

    
    return cybermlist;
    
failexit:
    if (NULL != hiddmlist)
     	HIDD_Gfx_ReleaseGfxModes(gfxhidd, hiddmlist);
	
    if (NULL != cybermlist)
     	FreeCModeList(cybermlist);
    
    return NULL;
}


VOID driver_FreeCModeList(struct List *modeList, struct Library *CyberGfxBase)
{
    struct CyberModeNode *node, *safe;
    
    ForeachNodeSafe(modeList, node, safe) {
	Remove((struct Node *)node);
	FreeMem(node, sizeof (struct CyberModeNode));
    }
    
    FreeMem(modeList, sizeof (struct List));
}


ULONG driver_GetCyberMapAttr(struct BitMap *bitMap, ULONG attribute, struct Library *CyberGfxBase)
{
    Object *bm_obj;
    Object *pf;
    
    IPTR retval;
    
    /* This is cybergraphx. We only work wih HIDD bitmaps */
    if (!IS_HIDD_BM(bitMap)) {
    	kprintf("!!!!! Trying to use CGFX call on non-hidd bitmap in GetCyberMapAttr() !!!\n");
    	return 0;
    }
	
    bm_obj = HIDD_BM_OBJ(bitMap);
    
    pf = HIDD_BM_GetPixelFormat(bm_obj, vHidd_PixFmt_Native);
    
    switch (attribute) {
   	case CYBRMATTR_XMOD:
	    GetAttr(bm_obj, aHidd_BitMap_BytesPerRow, &retval);
	    break;
	
   	case CYBRMATTR_BPPIX:
	    GetAttr(pf, aHidd_PixFmt_BytesPerPixel, &retval);
	    break;
	
   	case CYBRMATTR_PIXFMT: {
	    HIDDT_StdPixFmt stdpf;
	    UWORD cpf;
	    GetAttr(pf, aHidd_PixFmt_StdPixFmt, (IPTR *)&stdpf);
	    
	    /* Convert to cybergfx */
	    cpf = hidd2cyber_pixfmt(stdpf, GfxBase);
	    
	    if (cpf == (UWORD)-1) {
	    	kprintf("!!! UNKNOWN PIXEL FORMAT IN GetCyberMapAttr()\n");
	    }
	    
	    retval = (IPTR)cpf;
	    break;
	    
	}
	
   	case CYBRMATTR_WIDTH:
	    GetAttr(bm_obj, aHidd_BitMap_Width, &retval);
	    break;
	
   	case CYBRMATTR_HEIGHT:
	    GetAttr(bm_obj, aHidd_BitMap_Height, &retval);
	    break;
	
   	case CYBRMATTR_DEPTH:
	    GetAttr(pf, aHidd_PixFmt_Depth, &retval);
	    break;
	
   	case CYBRMATTR_ISCYBERGFX: {
	    IPTR depth;
	    
	    GetAttr(pf, aHidd_PixFmt_Depth, &depth);
	    
	    if (depth < 8) {
	    	retval = FALSE;
	    } else {
	    /* We allways have a HIDD bitmap */
	    	retval = TRUE;
	    }
	    break; }
	
   	case CYBRMATTR_ISLINEARMEM:
	    GetAttr(bm_obj, aHidd_BitMap_IsLinearMem, &retval);
	    break;
	
	default:
	kprintf("!!! UNKNOWN ATTRIBUTE PASSED TO GetCyberMapAttr()\n");
	break;
	
	
    } /* switch (attribute) */
    
    return retval;
}


/******************************************/
/* Support stuff for cybergfx             */
/******************************************/

#undef GfxBase
static UWORD hidd2cyber_pixfmt(HIDDT_StdPixFmt stdpf, struct GfxBase *GfxBase)
{
     UWORD cpf = (UWORD)-1;
     
     switch (stdpf) {
	case vHidd_PixFmt_RGB16:
	    cpf = PIXFMT_RGB16;
	    break;
	
	case vHidd_PixFmt_RGB24:
	    cpf = PIXFMT_RGB24;
	    break;
	
	case vHidd_PixFmt_ARGB32:
	    cpf = PIXFMT_ARGB32;
	    break;
	
	case vHidd_PixFmt_RGBA32:
	    cpf = PIXFMT_RGBA32;
	    break;
	
	case vHidd_PixFmt_LUT8:
	    cpf = PIXFMT_LUT8;
	    break;
	    
	default:
	    kprintf("UNKNOWN CYBERGRAPHICS PIXFMT IN cyber2hidd_pixfmt\n");
	    break;
     
    }

    return cpf;     
     
}

static HIDDT_StdPixFmt cyber2hidd_pixfmt(UWORD cpf, struct GfxBase *GfxBase)
{
    HIDDT_StdPixFmt stdpf = vHidd_PixFmt_Unknown;

    switch (cpf) {
	case PIXFMT_RGB16:
	    stdpf = vHidd_PixFmt_RGB16;
	    break;
	
	case PIXFMT_RGB24:
	    stdpf = vHidd_PixFmt_RGB24;
	    break;
	
	case PIXFMT_ARGB32:
	    stdpf = vHidd_PixFmt_ARGB32;
	    break;
	
	case PIXFMT_RGBA32:
	    stdpf = vHidd_PixFmt_RGBA32;
	    break;
	
	case PIXFMT_LUT8:
	    stdpf = vHidd_PixFmt_LUT8;
	    break;
	    
	default:
	    kprintf("UNKNOWN CYBERGRAPHICS PIXFMT IN cyber2hidd_pixfmt\n");
	    break;
	
    }
    
    return stdpf;
    
}

