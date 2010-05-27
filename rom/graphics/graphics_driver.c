/*
    Copyright � 1995-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Driver for using gfxhidd for gfx output
    Lang: english
*/

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/layers.h>
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
#include <graphics/scale.h>

#include <oop/oop.h>
#include <utility/tagitem.h>
#include <aros/asmcall.h>

#include <intuition/intuition.h>

#include <hidd/graphics.h>

#include <cybergraphx/cybergraphics.h>

#include <stdio.h>
#include <string.h>

#include "graphics_intern.h"
#include "fakegfxhidd.h"
#include "intregions.h"
#include "dispinfo.h"
#include "gfxfuncsupport.h"
#include "fontsupport.h"

#define SDEBUG 0
#define DEBUG 0
#include <aros/debug.h>

#define DEBUG_INIT(x)
#define DEBUG_LOADVIEW(x)

/* Define this if you wish to enforce using software mouse sprite
   even for drivers that support hardware one. Useful for debugging
   and testing
#define FORCE_SOFTWARE_SPRITE */

struct ETextFont
{
    struct TextFont	etf_Font;
};


/* InitDriverData() just allocates memory for the struct. To use e.g.   */
/* AreaPtrns, UpdateAreaPtrn() has to allocate the memory for the       */
/* Pattern itself (and free previously used memory!)                    */

static inline void AddDriverDataToList(struct gfx_driverdata *dd, struct GfxBase * GfxBase)
{
    LONG hash;
    
    hash = CalcHashIndex((IPTR)dd, DRIVERDATALIST_HASHSIZE);
    
//  ObtainSemaphore(&PrivGBase(GfxBase)->driverdatasem);
    AddTail((struct List *)&PrivGBase(GfxBase)->driverdatalist[hash], (struct Node *)&dd->dd_Node);    
//  ReleaseSemaphore(&PrivGBase(GfxBase)->driverdatasem);
    
}

static inline void RemoveDriverDataFromList(struct gfx_driverdata *dd, struct GfxBase *GfxBase)
{
//  ObtainSemaphore(&PrivGBase(GfxBase)->driverdatasem);
    Remove((struct Node *)&dd->dd_Node);    
//  ReleaseSemaphore(&PrivGBase(GfxBase)->driverdatasem);
}

static inline BOOL FindDriverData(struct gfx_driverdata *dd, struct RastPort *rp, struct GfxBase *GfxBase)
{
    struct gfx_driverdata *hn = NULL;
    LONG    	     	  hash;
    BOOL    	    	  retval = FALSE;
    
    hash = CalcHashIndex((IPTR)dd, DRIVERDATALIST_HASHSIZE);

//  ObtainSemaphore(&PrivGBase(GfxBase)->driverdatasem);    
    ForeachNode((struct List *)&PrivGBase(GfxBase)->driverdatalist[hash], hn)
    {
    	if ((hn == dd) && (hn->dd_RastPort == rp))
	{
	    retval = TRUE;
	    break;
	}
    }
//  ReleaseSemaphore(&PrivGBase(GfxBase)->driverdatasem);
    
    return retval;
}

BOOL ObtainDriverData(struct RastPort *rp, struct GfxBase *GfxBase)
{
    struct gfx_driverdata *dd;
    
    if (RP_BACKPOINTER(rp) == rp)
    {
    	if (rp->Flags & RPF_SELF_CLEANUP)
	{
	    if (RP_DRIVERDATA(rp)) return TRUE;
	}
    }
    else
    {
    	/* We have detected a manually cloned rastport. Mark it as 'valid'
	   (backpointer to itself) but with NULL driverdata and non-self
	   cleanup */
    	RP_DRIVERDATA(rp) = NULL;
	rp->Flags &= ~RPF_SELF_CLEANUP;
	RP_BACKPOINTER(rp) = rp;
    }
    
    ObtainSemaphore(&PrivGBase(GfxBase)->driverdatasem);    
    dd = RP_DRIVERDATA(rp);
    if (dd)
    {
    	if (!(rp->Flags & RPF_SELF_CLEANUP) && FindDriverData(dd, rp, GfxBase))
	{
	    dd->dd_LockCount++;
	}
	else
	{
	    RP_DRIVERDATA(rp) = NULL;
	    dd = NULL;
	}
    }
    
    if (!dd)
    {
    	dd = AllocPooled(PrivGBase(GfxBase)->driverdatapool, sizeof(*dd));
	if (dd)
	{
	    struct monitor_driverdata *sdd;
	    struct TagItem gc_tags[] = {{ TAG_DONE}};

	    if (rp->BitMap)
	        sdd = GET_BM_DRIVERDATA(rp->BitMap);
	    else
	        sdd = (struct monitor_driverdata *)CDD(GfxBase);

	    dd->dd_GC = HIDD_Gfx_NewGC(sdd->gfxhidd, gc_tags);
	    if (dd->dd_GC)
	    {
   	    	dd->dd_RastPort = rp;
		dd->dd_LockCount = 1;
		
    	    	RP_DRIVERDATA(rp) = dd;
	    	rp->Flags |= RPF_DRIVER_INITED;

    	    	if (!(rp->Flags & RPF_SELF_CLEANUP)) AddDriverDataToList(dd, GfxBase);
		
		if (rp->BitMap) SetABPenDrMd(rp, (UBYTE)rp->FgPen, (UBYTE)rp->BgPen, rp->DrawMode);
	    }
    	    else
	    {
	    	FreePooled(PrivGBase(GfxBase)->driverdatapool, dd, sizeof(*dd));
		dd = NULL;
	    }
	    
	} /* if (dd) */
	
    } /* if (!dd) */
    
    ReleaseSemaphore(&PrivGBase(GfxBase)->driverdatasem);    
    
    return dd ? TRUE : FALSE;
}

void ReleaseDriverData(struct RastPort *rp, struct GfxBase *GfxBase)
{
    struct gfx_driverdata *dd = GetDriverData(rp);
    
    if (!(rp->Flags & RPF_SELF_CLEANUP))
    {
    /* FIXME: stegerg 23 jan 2004: needs semprotection, too! */
	/* CHECKME: stegerg 23 feb 2005: really?? */
	
    dd->dd_LockCount--;
    
    	// Don't do this:
	//
//    if (!dd->dd_LockCount) KillDriverData(rp, GfxBase);
	//
	// some garbage collection should later hunt for dd's with
	// 0 lockcount and possibly non-usage for a certain amount
	// of time and get rid of them.
    }
}

void KillDriverData(struct RastPort *rp, struct GfxBase *GfxBase)
{
    if (RP_BACKPOINTER(rp) == rp)
    {
        struct gfx_driverdata *dd = NULL;
    
    	if (rp->Flags & RPF_SELF_CLEANUP)
	{
	    dd = RP_DRIVERDATA(rp);
	}
    	else
    	{
    ObtainSemaphore(&PrivGBase(GfxBase)->driverdatasem);    
    	    if (FindDriverData(RP_DRIVERDATA(rp), rp, GfxBase))
	    {
	    	dd = RP_DRIVERDATA(rp);
	    	RemoveDriverDataFromList(dd, GfxBase);
	    }
    	    ReleaseSemaphore(&PrivGBase(GfxBase)->driverdatasem);    
    	}
    
    if (dd)
    {
    	struct monitor_driverdata *sdd;

	if (rp->BitMap)
	    sdd = GET_BM_DRIVERDATA(rp->BitMap);
	else
	    sdd = (struct monitor_driverdata *)CDD(GfxBase);

    	HIDD_Gfx_DisposeGC(sdd->gfxhidd, dd->dd_GC);
	FreePooled(PrivGBase(GfxBase)->driverdatapool, dd, sizeof(*dd)); 
	    RP_DRIVERDATA(rp) = NULL;
    }
    
    }
        
}

int driver_init(struct GfxBase * GfxBase)
{

    EnterFunc(bug("driver_init()\n"));
    
    /* Initialize the semaphore used for the chunky buffer */
    InitSemaphore(&(PrivGBase(GfxBase)->pixbuf_sema));
    InitSemaphore(&(PrivGBase(GfxBase)->blit_sema));
    
    /* Init the needed attrbases */

    __IHidd_BitMap  	= OOP_ObtainAttrBase(IID_Hidd_BitMap);
    __IHidd_GC      	= OOP_ObtainAttrBase(IID_Hidd_GC);
    __IHidd_Sync    	= OOP_ObtainAttrBase(IID_Hidd_Sync);
    __IHidd_PixFmt  	= OOP_ObtainAttrBase(IID_Hidd_PixFmt);
    __IHidd_PlanarBM 	= OOP_ObtainAttrBase(IID_Hidd_PlanarBM);
    __IHidd_Gfx     	= OOP_ObtainAttrBase(IID_Hidd_Gfx);
    __IHidd_FakeGfxHidd = OOP_ObtainAttrBase(IID_Hidd_FakeGfxHidd);
    

    if (__IHidd_BitMap   &&
        __IHidd_GC       &&
	__IHidd_Sync     &&
	__IHidd_PixFmt   &&
	__IHidd_PlanarBM &&
	__IHidd_Gfx      &&
	__IHidd_FakeGfxHidd)
    {
	PrivGBase(GfxBase)->pixel_buf=AllocMem(PIXELBUF_SIZE,MEMF_ANY);
	if (PrivGBase(GfxBase)->pixel_buf) {

	    /* Init display mode database */
	    InitSemaphore(&CDD(GfxBase)->displaydb_sem);
	    CDD(GfxBase)->last_id = AROS_RTG_MONITOR_ID;

	    /* Init memory driver */
	    CDD(GfxBase)->memorygfx = OOP_NewObject(NULL, CLID_Hidd_Gfx, NULL);
	    DEBUG_INIT(bug("[driver_init] Memory driver object 0x%p\n", CDD(GfxBase)->memorygfx));
	    if (CDD(GfxBase)->memorygfx) {
		struct TagItem bm_create_tags[] = {
		    { aHidd_BitMap_GfxHidd	, (IPTR)CDD(GfxBase)->memorygfx },
		    { aHidd_PlanarBM_AllocPlanes, FALSE				 },
		    { TAG_DONE			, 0UL				 }
		};

	    	CDD(GfxBase)->planarbm_cache = create_object_cache(NULL, CLID_Hidd_PlanarBM, bm_create_tags, GfxBase);
		DEBUG_INIT(bug("[driver_init] Planar bitmap cache 0x%p\n", CDD(GfxBase)->planarbm_cache));
		if (CDD(GfxBase)->planarbm_cache) {
		    struct TagItem gc_create_tags[] = { { TAG_DONE, 0UL } };

		    CDD(GfxBase)->gc_cache = create_object_cache(NULL, CLID_Hidd_GC, gc_create_tags, GfxBase);
		    DEBUG_INIT(bug("[driver_init] GC cache 0x%p\n", CDD(GfxBase)->planarbm_cache));
		    if (CDD(GfxBase)->gc_cache) {
		        /* Get method id here, to avoid any possible semaphore involment when calling the method */ 
		        CDD(GfxBase)->hiddGfxShowImminentReset_MethodID = OOP_GetMethodID(IID_Hidd_Gfx, moHidd_Gfx_ShowImminentReset);

		        ReturnInt("driver_init", int, TRUE);
		    }
		    delete_object_cache(CDD(GfxBase)->planarbm_cache, GfxBase);
		    
		}
		OOP_DisposeObject(CDD(GfxBase)->memorygfx);
	    }
	    FreeMem(PrivGBase(GfxBase)->pixel_buf, PIXELBUF_SIZE);
	}
    }

    ReturnInt("driver_init", int, FALSE);
}

/* Called after DOS is up & running */
static OOP_Object *create_framebuffer(OOP_Object *gfxhidd, struct GfxBase *GfxBase)
{
    struct TagItem fbtags[] = {
    	{ aHidd_BitMap_FrameBuffer,	TRUE	},
	{ aHidd_BitMap_ModeID,		0	},
	{ TAG_DONE, 0 }
    };

    HIDDT_ModeID hiddmode;
    OOP_Object *fb = NULL;

    /* Get the highest available resolution at the best possible depth */
    hiddmode = get_best_resolution_and_depth(gfxhidd, GfxBase);
    if (vHidd_ModeID_Invalid == hiddmode) {
    	D(bug("!!! create_framebuffer(): COULD NOT GET HIDD MODEID !!!\n"));
    } else {
    	/* Create the framebuffer object */
	fbtags[1].ti_Data = hiddmode;
	fb = HIDD_Gfx_NewBitMap(gfxhidd, fbtags);
    }

    return fb;
}

/*
 * Set up graphics display driver.
 * Creates necessary OS structures around it.
 *
 * gfxhidd - newly created driver object
 * result  - master driver structure
 */

struct monitor_driverdata *driver_Setup(OOP_Object *gfxhidd, struct GfxBase *GfxBase)
{
    ULONG cnt = 0;
    IPTR hwcursor = 0;
    IPTR noframebuffer = 0;
    BOOL ok = TRUE;
    ULONG i;
    HIDDT_ModeID *modes, *m;
    struct monitor_driverdata *mdd;

    D(bug("[driver_Setup] gfxhidd=0x%p\n", gfxhidd));

    modes = HIDD_Gfx_QueryModeIDs(gfxhidd, NULL);
    if (!modes)
        return NULL;

    /* Count number of display modes */
    for (m = modes; *m != vHidd_ModeID_Invalid; m ++)
	cnt++;

    mdd = AllocVec(sizeof(struct monitor_driverdata) + cnt * sizeof(struct DisplayInfoHandle), MEMF_PUBLIC|MEMF_CLEAR);
    D(bug("[driver_Setup] Allocated driverdata at 0x%p\n", mdd));
    if (!mdd) {
        HIDD_Gfx_ReleaseModeIDs(gfxhidd, modes);
        return NULL;
    }

    mdd->gfxhidd_orig = gfxhidd;

    /* Fill in ModeID database in the driverdata */
    for (i = 0; i <= cnt; i++) {
        mdd->modes[i].id = modes[i];
	mdd->modes[i].drv = mdd;
    }

    HIDD_Gfx_ReleaseModeIDs(gfxhidd, modes);

#ifndef FORCE_SOFTWARE_SPRITE
    OOP_GetAttr(gfxhidd, aHidd_Gfx_SupportsHWCursor, &hwcursor);
#endif
    OOP_GetAttr(gfxhidd, aHidd_Gfx_NoFrameBuffer, &noframebuffer);

    if (hwcursor) {
        mdd->gfxhidd = gfxhidd;
    } else {
	D(bug("[driver_Setup] Hardware mouse cursor is not supported, using fakegfx.hidd\n"));

	mdd->gfxhidd = init_fakegfxhidd(gfxhidd, GfxBase);
	if (mdd->gfxhidd)
	    mdd->fakegfx_inited = TRUE;
	else
	    ok = FALSE;
    }

    if (ok) {
	struct TagItem gc_create_tags[] = { { TAG_DONE, 0UL } };

	D(bug("[driver_Setup] Ok\n"));

        /* FIXME: perhaps driver should be able to supply own GC class? */
	mdd->gc_cache = create_object_cache(NULL, CLID_Hidd_GC, gc_create_tags, GfxBase);
	if (mdd->gc_cache) {
	    D(bug("[driver_Setup] GC Cache created\n"));

	    if (!noframebuffer)
	        /* Note that we perform this operation on fakegfx.hidd if it was
		   plugged in */
		mdd->framebuffer = create_framebuffer(mdd->gfxhidd, GfxBase);

	    if (noframebuffer || mdd->framebuffer) {
		D(bug("[driver_OpenMonitor] FRAMEBUFFER OK: %p\n", mdd->framebuffer));
		return mdd;
	    }

	    if (mdd->framebuffer)
		OOP_DisposeObject(mdd->framebuffer);

	    delete_object_cache(mdd->gc_cache, GfxBase);
	} /* if (gc object cache ok) */
    } /* if (fake gfx stuff ok) */

    if (mdd->fakegfx_inited)
	OOP_DisposeObject(mdd->gfxhidd);

    FreeVec(mdd);

    return NULL;
}

/*
 * Insert the ready-to-use driver with all its modes into the display mode database
 *
 * mdd    - A pointer to driver descriptor
 * numIDs - A number of subsequent monitor IDs to reserve
 */

void driver_Add(struct monitor_driverdata *mdd, ULONG numIDs, struct GfxBase *GfxBase)
{
    struct monitor_driverdata *last;

    ObtainSemaphore(&CDD(GfxBase)->displaydb_sem);

    mdd->id = CDD(GfxBase)->last_id;
    CDD(GfxBase)->last_id += (numIDs << AROS_MONITOR_ID_SHIFT); /* Next available monitor ID */

    /* Insert the driverdata into chain, sorted by ID */
    for (last = (struct monitor_driverdata *)CDD(GfxBase); last->next; last = last->next) {
	if (mdd->id < last->next->id)
	    break;
    }
    mdd->next = last->next;
    last->next = mdd;
    
    ReleaseSemaphore(&CDD(GfxBase)->displaydb_sem);
}

/*
 * Remove the driver from the database
 *
 * mdd - Driver structure to remove
 */

void driver_Remove(struct monitor_driverdata *mdd, struct GfxBase *GfxBase)
{
    struct monitor_driverdata *prev;

    ObtainSemaphore(&CDD(GfxBase)->displaydb_sem);

    /* Insert the driverdata into chain, sorted by ID */
    for (prev = (struct monitor_driverdata *)CDD(GfxBase); prev->next; prev = prev->next) {
        if (prev->next == mdd) {
	    prev->next = mdd->next;
	    break;
	}
    }
    
    ReleaseSemaphore(&CDD(GfxBase)->displaydb_sem);
}    

/*
 * Completely remove a driver from the OS.
 *
 * mdd - Driver structure to remove.
 *
 * Note that removing a driver is very unsafe operation. You must be
 * sure that no bitmaps of this driver exist at the moment.
 *
 * It is unclear whether unloading drivers will be supported in future.
 */

void driver_Expunge(struct monitor_driverdata *mdd, struct GfxBase *GfxBase)
{
    ULONG i;

    /* Remove the driver from the list. This will remove its modes from the database. */
    driver_Remove(mdd, GfxBase);

    if (mdd->framebuffer)
	OOP_DisposeObject(mdd->framebuffer);

    if (mdd->gc_cache)
	delete_object_cache(mdd->gc_cache, GfxBase );

    if (mdd->fakegfx_inited)
        OOP_DisposeObject(mdd->gfxhidd);

    /* Dispose driver object. This will take care about syncs etc */
    if (mdd->gfxhidd_orig)
	OOP_DisposeObject(mdd->gfxhidd_orig );

    FreeVec(mdd);
}

#if 0 /* Unused? */

static ULONG getbitmappixel(struct BitMap *bm
	, LONG x
	, LONG y
	, UBYTE depth
	, UBYTE plane_mask)
{
    UBYTE i;
    ULONG idx;

    ULONG mask;
    ULONG pen = 0;

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

#endif


#define dumprect(rect) \
        kprintf(#rect " : (%d,%d) - (%d,%d)\n",(rect)->MinX,(rect)->MinY,(rect)->MaxX,(rect)->MaxY)


struct bgf_render_data {
    WORD fbm_xsrc;
    OOP_Object *fbm;
};

static ULONG bgf_render(APTR bgfr_data
	, LONG srcx, LONG srcy
	, OOP_Object *dstbm_obj, OOP_Object *dst_gc
	, LONG x1, LONG y1, LONG x2, LONG y2
	, struct GfxBase *GfxBase)
{
    struct bgf_render_data *bgfrd;
    ULONG width, height;
    
    width  = x2 - x1 + 1;
    height = y2 - y1 + 1;
    
    ASSERT(width > 0 && height > 0);

    bgfrd = (struct bgf_render_data *)bgfr_data;
    
    HIDD_BM_BlitColorExpansion( dstbm_obj
    	, dst_gc
	, bgfrd->fbm
	, srcx + bgfrd->fbm_xsrc, srcy /* stegerg: instead of srcy there was a 0 */
	, x1, y1
	, width, height
     );
     
     return width * height;
    
}
    
void blit_glyph_fast(struct RastPort *rp, OOP_Object *fontbm, WORD xsrc
	, WORD destx, WORD desty, UWORD width, UWORD height
	, struct GfxBase * GfxBase)
{
    
    struct Rectangle rr;
    struct bgf_render_data bgfrd;

    EnterFunc(bug("blit_glyph_fast(%d, %d, %d, %d, %d)\n"
    	, xsrc, destx, desty, width, height));
	
    
    ASSERT(width > 0 && height > 0);
    
    bgfrd.fbm_xsrc = xsrc;
    bgfrd.fbm	   = fontbm;
    
    rr.MinX = destx;
    rr.MinY = desty;
    rr.MaxX = destx + width  - 1;
    rr.MaxY = desty + height - 1;
	
    if (!OBTAIN_DRIVERDATA(rp, GfxBase))
    	ReturnVoid("blit_glyph_fast");
	
    do_render_func(rp, NULL, &rr, bgf_render, &bgfrd, TRUE, FALSE, GfxBase);
	
    RELEASE_DRIVERDATA(rp, GfxBase);
    
    ReturnVoid("blit_glyph_fast");
}

    

#define NUMCHARS(tf) ((tf->tf_HiChar - tf->tf_LoChar) + 2)
#define CTF(x) ((struct ColorTextFont *)x)

void driver_Text (struct RastPort * rp, CONST_STRPTR string, LONG len,
		  struct GfxBase * GfxBase)
{

#warning Does not handle color textfonts
    WORD  render_y;
    struct TextFont *tf;
    WORD current_x;
    OOP_Object *fontbm = NULL;
    
    if (!OBTAIN_DRIVERDATA(rp, GfxBase))
    	return;

    if ((rp->DrawMode & ~INVERSVID) == JAM2)
    {
    	struct TextExtent te;
    	ULONG 	    	  old_drmd = GetDrMd(rp);

    	/* This is actually needed, because below only the
	   part of the glyph which contains data is rendered:
	   
	   ...1100...
	   ...1100...
	   ...1100...
	   ...1111...
	   
	   '.' at left side can be there because of kerning.
	   '.' at the right side can be there because of
	   CharSpace being bigger than glyph bitmap data
	   width.
	*/
	
	TextExtent(rp, string, len, &te);
	SetDrMd(rp, old_drmd ^ INVERSVID);
	RectFill(rp, rp->cp_x + te.te_Extent.MinX,
	    	     rp->cp_y + te.te_Extent.MinY,
		     rp->cp_x + te.te_Extent.MaxX,
		     rp->cp_y + te.te_Extent.MaxY);
	SetDrMd(rp, old_drmd);
    }
    
    /* does this rastport have a layer. If yes, lock the layer it.*/
    if (NULL != rp->Layer)
      LockLayerRom(rp->Layer);	
    
    tf = rp->Font;
    
    if (ExtendFont(tf, NULL))
    {
    	fontbm = ((struct TextFontExtension_intern *)(tf->tf_Extension))->hash->font_bitmap;
    }
    
    /* Check if font has character data as a HIDD bitmap */
    
    if (NULL == fontbm)
    {
    	D(bug("FONT HAS NO HIDD BITMAP ! Won't render text\n"));
	RELEASE_DRIVERDATA(rp, GfxBase);
	return;
    }


    /* Render along font's baseline */
    render_y = rp->cp_y - tf->tf_Baseline;
    current_x = rp->cp_x;
    
    while ( len -- )
    {
	ULONG charloc;
	ULONG idx;
	
	if (*string < tf->tf_LoChar || *string > tf->tf_HiChar )
	{
	     /* A character which there is no glyph for. We just
	        draw the last glyph in the font
	     */
	     idx = NUMCHARS(tf) - 1;
	}
	else
	{
	    idx = *string - tf->tf_LoChar;
	}
	
	charloc = ((ULONG *)tf->tf_CharLoc)[idx];
	
	if (tf->tf_CharKern)
	{
    	    current_x += ((WORD *)tf->tf_CharKern)[idx];
	}
	    
	if ((tf->tf_Style & FSF_COLORFONT) &&
	    !(CTF(tf)->ctf_Flags & CT_ANTIALIAS))
	{
    	    #warning Handle color fonts	
	}
	else
	{
	       WORD
	    glyphXOffset = charloc >> 16,
        glyphWidth = charloc & 0xFFFF;

	    ASSERT(tf->tf_YSize > 0);

/* blit the glypth if it has data in the bitmap */
        if (glyphWidth > 0)
        {
            blit_glyph_fast(rp, fontbm, glyphXOffset, current_x, render_y,
                glyphWidth, tf->tf_YSize, GfxBase);
        }
	}
	
	if (tf->tf_CharSpace)
	    current_x += ((WORD *)tf->tf_CharSpace)[idx];
	else
	    current_x += tf->tf_XSize; /* Add glyph width */
	
	current_x += rp->TxSpacing;
	
	string ++;
    } /* for (each character to render) */
    
    Move(rp, current_x, rp->cp_y);
    
    if (NULL != rp->Layer)
      UnlockLayerRom(rp->Layer);
    
    RELEASE_DRIVERDATA(rp, GfxBase);
    
    return;

}

void driver_LoadView(struct View *view, struct GfxBase *GfxBase)
{
    struct ViewPort *vp;
    struct HIDD_ViewPortData *vpd = NULL;
    struct BitMap *bitmap = NULL;
    OOP_Object *cmap, *pf;
    HIDDT_ColorModel colmod;
    OOP_Object *fb;

    #warning THIS IS NOT THREADSAFE

    /* To make this threadsafe we have to lock
       all gfx access in all the rendering calls
    */

    /* Find a ViewPortData of the first visible ViewPort. It will be a start of
       bitmaps chain to show.
       In future when AROS supports several monitors we will have several such chains,
       one per monitor. */
    if (view) {
        for (vp = view->ViewPort; vp; vp = vp->Next) {
            if (!(vp->Modes & VP_HIDE)) {
	        /* We don't check against vpe == NULL because MakeVPort() has already took care about this */
	        vpd = VPE_DATA((struct ViewPortExtra *)GfxLookUp(vp));
		bitmap = vp->RasInfo->BitMap;
		break;
	    }
	}
    }
    DEBUG_LOADVIEW(bug("[driver_LoadView] Showing ViewPort 0x%p, data 0x%p, BitMap 0x%p, BitMap object 0x%p\n", vp, vpd, bitmap, vpd->Bitmap));

    /* First try the new method */
    if (HIDD_Gfx_ShowViewPorts(SDD(GfxBase)->gfxhidd, vpd)) {
        DEBUG_LOADVIEW(bug("[driver_LoadView] ShowViewPorts() worked\n"));
        return;
    }

    /* If it failed, we may be working with a framebuffer. First check if the bitmap
       is already displayed. If so, do nothing (because displaying the same bitmap twice may
       cause some problems */
    DEBUG_LOADVIEW(bug("[driver_LoadView] Old displayed bitmap: 0x%p\n", SDD(GfxBase)->frontbm));
    if (SDD(GfxBase)->frontbm == bitmap)
        return;

    fb = HIDD_Gfx_Show(SDD(GfxBase)->gfxhidd, vpd ? vpd->Bitmap : NULL, fHidd_Gfx_Show_CopyBack);
    DEBUG_LOADVIEW(bug("[driver_LoadView] Show() returned 0x%p\n", fb));

    if (fb) {
    	IPTR width, height;

	DEBUG_LOADVIEW(bug("[driver_LoadView] Replacing framebuffer\n"));
	Forbid();

	 /* Set this as the active screen */
    	if (NULL != SDD(GfxBase)->frontbm)
	{
    	    struct BitMap *oldbm;
    	    /* Put back the old values into the old bitmap */
	    oldbm = SDD(GfxBase)->frontbm;
	    HIDD_BM_OBJ(oldbm)		= SDD(GfxBase)->bm_bak;
	    HIDD_BM_COLMOD(oldbm)	= SDD(GfxBase)->colmod_bak;
	    HIDD_BM_COLMAP(oldbm)	= SDD(GfxBase)->colmap_bak;
	}

	SDD(GfxBase)->frontbm		= bitmap;
	SDD(GfxBase)->bm_bak		= bitmap ? HIDD_BM_OBJ(bitmap) : NULL;
	SDD(GfxBase)->colmod_bak	= bitmap ? HIDD_BM_COLMOD(bitmap) : 0;
	SDD(GfxBase)->colmap_bak	= bitmap ? HIDD_BM_COLMAP(bitmap) : NULL;

	if (bitmap)
	{
	    /* Insert the framebuffer in its place */
	    OOP_GetAttr(fb, aHidd_BitMap_ColorMap, (IPTR *)&cmap);
	    OOP_GetAttr(fb, aHidd_BitMap_PixFmt, (IPTR *)&pf);
	    OOP_GetAttr(pf, aHidd_PixFmt_ColorModel, &colmod);

	    HIDD_BM_OBJ(bitmap)	= fb;
	    HIDD_BM_COLMOD(bitmap)	= colmod;
	    HIDD_BM_COLMAP(bitmap)	= cmap;
	}

        OOP_GetAttr(fb, aHidd_BitMap_Width, &width);
    	OOP_GetAttr(fb, aHidd_BitMap_Height, &height);
	DEBUG_LOADVIEW(bug("[driver_LoadView] Updating framebuffer, new size: %d x %d\n", width, height));

	HIDD_BM_UpdateRect(fb, 0, 0, width, height);
	Permit();
    }
}
