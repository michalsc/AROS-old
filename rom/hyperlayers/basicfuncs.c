/*
    (C) 1995-96 AROS - The Amiga Research OS
    $Id$

    Desc: Basic support functions for layers.library.
    Lang: English.
*/

#include <aros/config.h>
#include <aros/asmcall.h>
#include <exec/memory.h>
#include <graphics/rastport.h>
#include <graphics/clip.h>
#include <graphics/regions.h>
#include <graphics/layers.h>
#include <graphics/gfx.h>
#include <utility/hooks.h>
#include <setjmp.h>

#include <proto/exec.h>
#include <proto/alib.h>
#include <proto/graphics.h>
#include <proto/layers.h>

#include "layers_intern.h"
#include "basicfuncs.h"

extern struct ExecBase * SysBase;

/*
 *  Sections:
 *
 *  + Blitter
 *  + Hook
 *  + Layer
 *  + LayerInfo
 *  + Rectangle
 *  + Resource Handling
 *  + Miscellaneous
 *
 */

/***************************************************************************/
/*                                 BLITTER                                 */
/***************************************************************************/

#define CR2NR_NOBITMAP 0
#define CR2NR_BITMAP   1

#if !(AROS_FLAVOUR & AROS_FLAVOUR_NATIVE)
/*
 * These functions cause the infamous "fixed or forbidden register was spilled"
 * bug/feature in m68k gcc, so these were written straight in asm. They can be
 * found in config/m68k-native/layers, for the m68k AROSfA target. Other targets,
 * that use stack passing, can use these versions.
 */

void BltRPtoCR(struct RastPort *    rp,
               struct ClipRect *    cr,
               ULONG                Mode)
{
    BltBitMap(rp->BitMap, 
              cr->bounds.MinX, 
              cr->bounds.MinY,
	      cr->BitMap, 
	      ALIGN_OFFSET(cr->bounds.MinX), 0,
	      cr->bounds.MaxX - cr->bounds.MinX + 1,
	      cr->bounds.MaxY - cr->bounds.MinY + 1,
	      Mode, 
	      ~0, 
	      NULL);
}

void BltCRtoRP(struct RastPort *   rp,
               struct ClipRect *   cr,
               ULONG               Mode)
{
    BltBitMap(cr->BitMap, 
              ALIGN_OFFSET(cr->bounds.MinX), 
              0,
	      rp->BitMap, 
	      cr->bounds.MinX, 
	      cr->bounds.MinY,
	      cr->bounds.MaxX - cr->bounds.MinX + 1,
	      cr->bounds.MaxY - cr->bounds.MinY + 1,
	      Mode, 
	      ~0, 
	      NULL);
}

#endif /* if !native */

/***************************************************************************/
/*                                  HOOK                                   */
/***************************************************************************/

struct layerhookmsg
{
    struct Layer *l;
/*  struct Rectangle rect; (replaced by the next line!) */
    WORD MinX, MinY, MaxX, MaxY;
    LONG OffsetX, OffsetY;
};

void _CallLayerHook(struct Hook * h,
                    struct RastPort * rp,
                    struct Layer * L,
                    struct Rectangle * R,
                    WORD offsetX,
                    WORD offsetY)
{
  struct BitMap * bm = rp->BitMap;
  if (h == LAYERS_BACKFILL)
  {
    /* Use default backfill, which means that I will clear the area */
    BltBitMap(bm,
              0,
              0,
              bm,
              R->MinX,
              R->MinY,
              R->MaxX - R->MinX + 1,
              R->MaxY - R->MinY + 1,
              0x000,
              0xff,
              NULL);
    /* that's it */
    return;
  }
  
  if (h != LAYERS_NOBACKFILL)
  {
    struct layerhookmsg msg;
    msg.l    = L;
    msg.MinX = R->MinX;
    msg.MinY = R->MinY;
    msg.MaxX = R->MaxX;
    msg.MaxY = R->MaxY;
    msg.OffsetX = offsetX;
    msg.OffsetY = offsetY;
    
    AROS_UFC3(void, h->h_Entry,
        AROS_UFCA(struct Hook *,         h   ,A0),
        AROS_UFCA(struct RastPort *,     rp  ,A2),
        AROS_UFCA(struct layerhookmsg *, &msg,A1)
    );  
  }
  
}         


/***************************************************************************/
/*                                 LAYER                                   */
/***************************************************************************/

/* Set the priorities of the layer. The farther in the front it is the
   higher its priority will be.
 */
 
void SetLayerPriorities(struct Layer_Info * li)
{
  struct Layer * L = li -> top_layer;
  UWORD pri = 10000;
  while (NULL != L)
  {
    L -> priority = pri;
    pri--;
    L = L->back;
  }
}

struct Layer * internal_WhichLayer(struct Layer * l, WORD x, WORD y)
{
  while(l != NULL)
  {
    if(x >= l->bounds.MinX && x <= l->bounds.MaxX &&
       y >= l->bounds.MinY && y <= l->bounds.MaxY)
	     return l;
    l = l->back;
  }

  return NULL;
}


/***************************************************************************/
/*                               LAYERINFO                                 */
/***************************************************************************/

/*-------------------------------------------------------------------------*/
/*
 * Allocate LayerInfo_extra and initialize its resource list. Layers uses
 * this resource list to keep track of various memory allocations it makes
 * for the layers. See ResourceNode and ResData in layers_intern.h for the
 * node structure. See AddLayersResource for more information on the basic
 * operation.
 */
BOOL _AllocExtLayerInfo(struct Layer_Info * li)
{
    if(++li->fatten_count != 0)
	return TRUE;

    if(!(li->LayerInfo_extra = AllocMem(sizeof(struct LayerInfo_extra),MEMF_PUBLIC|MEMF_CLEAR)))
	return FALSE;

    NewList((struct List *)&((struct LayerInfo_extra *)li->LayerInfo_extra)->lie_ResourceList);

    return TRUE;
}

/*
 * Free LayerInfo_extra.
 */
void _FreeExtLayerInfo(struct Layer_Info * li)
{
    if(--li->fatten_count >= 0)
	return;

    if(li->LayerInfo_extra == NULL)
	return;

    FreeMem(li->LayerInfo_extra, sizeof(struct LayerInfo_extra));

    li->LayerInfo_extra = NULL;
}

/*
 * Initialize LayerInfo_extra and save the current environment.
 */
ULONG _InitLIExtra(struct Layer_Info * li,
                   struct LayersBase * LayersBase)
{
    struct LayerInfo_extra *lie = li->LayerInfo_extra;

    LockLayerInfo(li);

    /*
     * Initialize the ResourceList contained in the LayerInfo_extra.
     * This list is used to keep track of Layers' resource (memory/
     * bitmaps/etc.) allocations.
     */
    NewList((struct List *)&lie->lie_ResourceList);

    /*
     * Save the current environment, so we can drop back in case of
     * an error.
     */
    // return setjmp(lie->lie_JumpBuf);
    return 0;
}

void ExitLIExtra(struct Layer_Info * li,
                 struct LayersBase * LayersBase)
{
    struct LayerInfo_extra *lie = li->LayerInfo_extra;

    /* Free all resources associated with the layers. */
    FreeLayerResources(li, TRUE);

    UnlockLayerInfo(li);

    longjmp(lie->lie_JumpBuf, 1);
}

/*
 * Dynamically allocate LayerInfo_extra if it isn't already there.
 */
BOOL SafeAllocExtLI(struct Layer_Info * li,
                    struct LayersBase * LayersBase)
{
    LockLayerInfo(li);

    /* Check to see if we can ignore the rest of this call. :-) */
    if(li->Flags & NEWLAYERINFO_CALLED)
	return TRUE;

    if(_AllocExtLayerInfo(li))
	return TRUE;

    UnlockLayerInfo(li);

    return FALSE;
}

/*
 * Free LayerInfo_extra if it was dynamically allocated, and unlock the LI.
 */
void SafeFreeExtLI(struct Layer_Info * li,
                   struct LayersBase * LayersBase)
{
    if(!(li->Flags & NEWLAYERINFO_CALLED))
	_FreeExtLayerInfo(li);

    UnlockLayerInfo(li);
}

/***************************************************************************/
/*                                RECTANGLE                                */
/***************************************************************************/

/*
  Within the linked list of rectangles search for the rectangle that
  contains the given coordinates.
 */
struct ClipRect * internal_WhichClipRect(struct Layer * L, WORD x, WORD y)
{
  struct ClipRect * CR = L->ClipRect;
  while (NULL != CR)
  {
    if (x >= CR->bounds.MinX &&
        x <= CR->bounds.MaxX &&
        y >= CR->bounds.MinY &&
        y <= CR->bounds.MaxY)
      return CR;
    CR = CR->Next;
  }
  return NULL;
}

#define MAX(a,b)    ((a) > (b) ? (a) : (b))
#define MIN(a,b)    ((a) < (b) ? (a) : (b))



/***************************************************************************/
/*                            RESOURCE HANDLING                            */
/***************************************************************************/

/*
 * Add a resource to the LayerInfo resource list, dynamically allocating
 * extra storage space if needed.
 */
BOOL AddLayersResource(struct Layer_Info * li,
                       void *              ptr,
                       ULONG               Size)
{
    struct ResourceNode *rn;
    struct ResData      *rd;

    if(!li)
	return TRUE;

    if(IsListEmpty(&((struct LayerInfo_extra *)li->LayerInfo_extra)->lie_ResourceList))
	if(!(rn = AddLayersResourceNode(li)))
	    return FALSE;

    /* Check to see if there are some entries left */
    if(--rn->rn_FreeCnt < 0)
    {
	/* If all entries are full, we have none left. Logic. :-) */
	rn->rn_FreeCnt = 0;

	/* So we add some more space for resources... */
	if(!(rn = AddLayersResourceNode(li)))
	    return FALSE;

	/* ...and decrement it for the following operations. */
	rn->rn_FreeCnt--;
    }

    rd = rn->rn_FirstFree++;

    rd->ptr  = ptr;
    rd->Size = Size;

    return TRUE;
}

/*
 * Add a new node to the LayerInfo resource list.
 */
struct ResourceNode * AddLayersResourceNode(struct Layer_Info * li)
{
    struct ResourceNode *rn;

    if(!(rn = (struct ResourceNode *)AllocMem(sizeof(struct ResourceNode), MEMF_ANY)))
	return NULL;

    /*
     * We keep 48 entries in this list. Could change depending on resource
     * allocation going on in Layers. For every n*48 allocations, a new node
     * must be allocated. This can (slightly) slow down operations if this
     * happens a lot.
     */
    rn->rn_FreeCnt   = 48;

    /* Point the cached pointer to the first free vector. */
    rn->rn_FirstFree = &rn->rn_Data[0];

    AddHead((struct List *)&((struct LayerInfo_extra *)li->LayerInfo_extra)->lie_ResourceList, &rn->rn_Link);

    return rn;
}

/*
 * Allocate memory for a ClipRect.
 */

struct ClipRect * _AllocClipRect(struct Layer * L)
{
  struct ClipRect * CR;
  
  CR =  L->SuperSaveClipRects;

  if (NULL != CR)
  {
   /* I want to access the list of free ClipRects alone */
    L->SuperSaveClipRects = CR->Next;
    L->SuperSaveClipRectCounter--;

    CR->Flags  = 0;
    CR->Next   = NULL; 
    CR->lobs   = NULL;
    CR->BitMap = NULL;
    return CR;
  }
  
  CR = (struct ClipRect *) AllocMem(sizeof(struct ClipRect), MEMF_PUBLIC|MEMF_CLEAR);
  return CR;
}

/*
 * Return memory of a ClipRect for later use.
 */

void _FreeClipRect(struct ClipRect   * CR,
                   struct Layer      * L)
{
  if (L->SuperSaveClipRectCounter < MAXSUPERSAVECLIPRECTS)
  {
    /* Add the ClipRect to the front of the list */
    CR -> Next = L -> SuperSaveClipRects;
    L -> SuperSaveClipRects = CR;
    L -> SuperSaveClipRectCounter++;
  }
  else
    FreeMem(CR, sizeof(struct ClipRect));
}

/*
 * Free a whole list of cliprects including the allocated bitmaps (if any)
 */

void _FreeClipRectListBM(struct Layer * L,
                         struct ClipRect * CR)
{
  struct ClipRect * _CR = CR;
  BOOL isSmart; 
  if ((L->Flags & (LAYERSUPER|LAYERSMART)) == LAYERSMART)
    isSmart = TRUE;
  else
    isSmart = FALSE;
 
  /*
   * This function is not watching for the upper limit of 
   * pre allocated cliprects.
   */
  L->SuperSaveClipRectCounter++;
  
  while (TRUE)
  {
    if (NULL != _CR->BitMap && TRUE == isSmart)
    {
      FreeBitMap(_CR->BitMap);
      _CR->BitMap = NULL;
    }
    if (NULL != _CR->Next)
    {
      L->SuperSaveClipRectCounter++;
      _CR = _CR->Next;
    }
    else
      break;
  }
  /* _CR is the last ClipRect in the list. I concatenate the
     currently preallocated list of ClipRects with that list. */
  _CR->Next = L->SuperSaveClipRects;
  
  /* CR is the head of the ClipRect list now */
  L->SuperSaveClipRects = CR;
}

/*
 * Allocate memory of a given size and enter it into the LayerInfo's
 * resource list.
 */
void * AllocLayerStruct(ULONG               Size,
                        ULONG               Flags,
                        struct Layer_Info * li,
                        struct LayersBase * LayersBase)
{
    void *mem;

    mem = AllocMem(Size, Flags);

    /* If there is no LayerInfo, this is just a straight AllocMem(). */
    if(li)
    {
	/* But if there is a LI, and there was an error, drop back to the
	   previous environment. */
	if(!mem)
	    ExitLIExtra(li, LayersBase);

	/* If not, enter the memory into the layers resource list. */
	if(!AddLayersResource(li, mem, Size))
	{
	    FreeMem(mem, Size);

	    /* Again, drop back in case of an error. */
	    ExitLIExtra(li, LayersBase);
	}
    }

    return mem;
}

/*
 * Traverse the ResourceList associated with the LayerInfo, and free all
 * allocated resources.
 */
void FreeLayerResources(struct Layer_Info * li,
                        BOOL                flag)
{
    struct ResourceNode *rn;
    struct ResData      *rd;
    ULONG                count;

    while( (rn = (struct ResourceNode *)
	RemHead((struct List *)&((struct LayerInfo_extra *)li->LayerInfo_extra)->lie_ResourceList)) )
    {
	if(flag)
	{
	    count = 48 - rn->rn_FreeCnt;

	    for(rd = &rn->rn_Data[0]; count-- != 0; rd++)
	    {
		if(rd->Size == RD_REGION)
		    DisposeRegion(rd->ptr);
		else if(rd->Size == RD_BITMAP)
		    FreeBitMap(rd->ptr);
		else
		    FreeMem(rd->ptr, rd->Size);
	    }

	    FreeMem(rn, sizeof(struct ResourceNode));
	}
    }

}

/***************************************************************************/
/*                              MISCELLANEOUS                              */
/***************************************************************************/

/*
 * Makes sure that the top most layer only consists of one cliprect 
 */

void CleanTopLayer(struct Layer_Info * LI)
{
  struct Layer * L_top = LI->top_layer;
  struct ClipRect * CR;
  if (NULL == L_top)
    return;
  
  L_top->Flags &= ~LAYERUPDATING;
  
  CR = L_top->ClipRect;
  if (NULL != CR->Next)
  {
    struct ClipRect * _CR = CR->Next;
    /* the topmost layer should only have one ClipRect */
    CR->bounds = L_top->bounds;
    CR->Next = NULL;
    
    CR = _CR;
    while (CR != NULL)
    {
      _CR = CR->Next;
      _FreeClipRect(CR, L_top);
      
      CR = _CR;
    }
  }
}

/* 
 * When a clipregion is installed a secondary list of cliprects
 * exists that represents those parts where the paint operations may
 * be performed on.
 * This function copies all BitMaps of the cliprect list where
 * the region is into  the usual list of cliprects.
 * It also frees the bitmaps and cliprects of the region cliprect
 * list.
 */

void CopyAndFreeClipRectsClipRects(struct Layer * L,
                                   struct ClipRect * srcCR,
                                   struct ClipRect * destCR)
{
  struct ClipRect * sCR = srcCR;

  /* if it's a SUPERBITMAP layer then just free the cliprects */  
  if (LAYERSUPER == (L->Flags & (LAYERSMART|LAYERSUPER)))
  {
    _FreeClipRectListBM(L, L->ClipRect);
    return;
  }
  
  while (TRUE)
  {
    /* only copy from ClipRects that are hidden as the visible
       ones have all info in the screens rastport anyway 
    */
    if (NULL != sCR->lobs)
    {
      struct ClipRect * dCR = destCR;
      int area = (sCR->bounds.MaxX - sCR->bounds.MinX + 1) *
                 (sCR->bounds.MaxY - sCR->bounds.MinY + 1);
      int areacopied = 0;
                 
      while (NULL != dCR && areacopied != area)
      {   
        if (! (sCR->bounds.MinX > dCR->bounds.MaxX ||
               sCR->bounds.MinY > dCR->bounds.MaxY ||
               sCR->bounds.MaxX < dCR->bounds.MinX ||
               sCR->bounds.MaxY < dCR->bounds.MinY) &&
               NULL != dCR->BitMap)
        { 
          /* these two overlap */
          int a;
          ULONG srcX, srcY;
          ULONG destX, destY;
          ULONG width, height;
          
          width = sCR->bounds.MaxX - sCR->bounds.MinX + 1;
          height= sCR->bounds.MaxY - sCR->bounds.MinY + 1;
          
          if (sCR->bounds.MinX > dCR->bounds.MinX)
          {
            srcX  = ALIGN_OFFSET(sCR->bounds.MinX);
            destX = sCR->bounds.MinX - dCR->bounds.MinX + ALIGN_OFFSET(dCR->bounds.MinX);
          }
          else
          {
            srcX   = dCR->bounds.MinX - sCR->bounds.MinX + ALIGN_OFFSET(sCR->bounds.MinX);
            destX  = ALIGN_OFFSET(dCR->bounds.MinX);
            width -= (dCR->bounds.MinX - sCR->bounds.MinX);
          }
          
          if (sCR->bounds.MinY > dCR->bounds.MinY)
          {
            srcY  = 0;
            destY = sCR->bounds.MinY - dCR->bounds.MinY;
          }
          else
          {
            srcY    = dCR->bounds.MinY - sCR->bounds.MinY;
            destY   = 0;
            height -= srcY;
          }
          
          if (sCR->bounds.MaxX > dCR->bounds.MaxX)
            width  -= (sCR->bounds.MaxX - dCR->bounds.MaxX);
            
          if (sCR->bounds.MaxY > dCR->bounds.MaxY)
            height -= (sCR->bounds.MaxY - dCR->bounds.MaxY);
            
          a = width * height;
           
          if (a == area && 
              dCR->bounds.MaxX - dCR->bounds.MinX + 1 == width &&
              dCR->bounds.MaxY - dCR->bounds.MinY + 1 == height )
          {
            FreeBitMap(dCR->BitMap);
            dCR -> BitMap = sCR -> BitMap;
            break;
          }
          else
          {
            areacopied += a;
            
            BltBitMap(sCR->BitMap,
            	      srcX,
            	      srcY,
            	      dCR->BitMap,
            	      destX,
            	      destY,
            	      width,
            	      height,
            	      0x0c0,
            	      0xff,
            	      NULL);
            FreeBitMap(sCR->BitMap);
          }
        }
        dCR = dCR -> Next;
      } /* walk through all destination ClipRects */
    } /* if (NULL != sCR->lobs) */
  
    if (NULL == sCR->Next)
      break;
  
    sCR = sCR -> Next;
  }
  
  sCR->Next = L->SuperSaveClipRects;
  L->SuperSaveClipRects = srcCR;
  
}

/* 
** Uninstalls the ClipRegion ClipRects form all the layers that are
** found, if they haven't alreay been uninstalled.
*/

void UninstallClipRegionClipRects(struct Layer_Info * LI)
{
  struct Layer * L = LI->top_layer;
  while (NULL != L)
  {
    /* does this one have a ClipRegion and are the ClipRegion ClipRects 
       still installed? 
    */
    if (NULL != L->ClipRegion && NULL != L->_cliprects)
    {
      CopyAndFreeClipRectsClipRects(L, L->ClipRect, L->_cliprects);
      /* 
      ** make the regular (not clipped cliprects) cliprects the
      ** actual cliprects.
      */
      L->ClipRect = L->_cliprects;
      L->_cliprects = NULL;
    }
    L = L->back;
  }
}


/*-----------------------------------END-----------------------------------*/


/*
 * Backup any parts of the layer that overlap with the backup_region
 * and that are not already backed up. Create the cliprects and
 * bitmaps if necessary.
 * Assumption: Only visible parts become invisible,
 *             invisible parts will not become visible.
 *
 * This function MUST not manipulate hide_region!!!!
 */
int _BackupPartsOfLayer(struct Layer * l, 
                        struct Region * hide_region,
                        struct BitMap * display_bm)
{
  struct ClipRect * cr, * firstcr = NULL, * oldcr;
  struct RegionRectangle * rr;
  struct Region * r = NewRegion();
  int invisible = TRUE;
  
#warning Write function to copy a region
  OrRegionRegion(l->shape,r);
  AndRegionRegion(hide_region,r);
  
  /*
   * From region r create cliprects
   */
  while (1)
  {
    rr = r->RegionRectangle;
    while (rr)
    {
      cr = _AllocClipRect(l);
      cr->bounds.MinX = rr->bounds.MinX + r->bounds.MinX;
      cr->bounds.MinY = rr->bounds.MinY + r->bounds.MinY;
      cr->bounds.MaxX = rr->bounds.MaxX + r->bounds.MinX;
      cr->bounds.MaxY = rr->bounds.MaxY + r->bounds.MinY;
      cr->lobs  = invisible;
      cr->Next  = firstcr;

      firstcr = cr;

      rr = rr->Next;
    }

    if (TRUE == invisible)
    {
      /*
       * Flip the shape to the visible part
       */
      XorRegionRegion(l->shape,r);
      invisible = FALSE;
    }
    else
      break;
  }
  
  DisposeRegion(r);
  /*
   * firstcr holds all new cliprects.
   * flags = TRUE means that that cr will be visible.
   */
  oldcr = l->ClipRect;

  while (NULL != oldcr)
  {
    struct ClipRect * _cr = firstcr;
    int area = RECTAREA(&oldcr->bounds);
    while (_cr && 0 != area)
    {
      /*
       * Do the two rectangles overlap?
       */
      if (DO_OVERLAP(&_cr->bounds,&oldcr->bounds))
      {
        /*
         * Is this new one supposed to be invisible?
         */
        if (NULL != _cr->lobs)
        {
          struct BitMap * srcbm;
          if (IS_SIMPLEREFRESH(l))
          {
            /*
             * Nothing to do
             */
          }
          else
          {
            LONG xSrc, xDest;
            LONG ySrc, yDest;
            LONG xSize, ySize;
            
            xSize = oldcr->bounds.MaxX - oldcr->bounds.MinX + 1;
            ySize = oldcr->bounds.MaxY - oldcr->bounds.MinY + 1;

            /*
             * Does the sourc rect have a bitmap (off screen)
             * or is it on the screen.
             */            
            if (oldcr->BitMap)
            {
              xSrc = (oldcr->bounds.MinX - _cr->bounds.MinX);
              if (xSrc < 0)
              {
                xSize += xSrc;
                xSrc = 0;
              }
              
              ySrc = (oldcr->bounds.MinY - _cr->bounds.MinY);
              if (ySrc < 0)
              {
                ySize += ySrc;
                ySrc = 0;
              }
kprintf("Using olc cr's BitMap!\n");
              srcbm = oldcr->BitMap;
            }
            else
            {
              xSrc = (oldcr->bounds.MinX - _cr->bounds.MinX);
              if (xSrc < 0)
              {
                xSize += xSrc;
                xSrc = _cr->bounds.MinX;
              }
              
              ySrc = (oldcr->bounds.MinY - _cr->bounds.MinY);
              if (ySrc < 0)
              {
                ySize += ySrc;
                ySrc = _cr->bounds.MinY;
              }

              srcbm = l->rp->BitMap;
kprintf("Using bitmap of screen!\n");
            }

            if (oldcr->bounds.MaxX - _cr->bounds.MaxX > 0)
              xSize = xSize - (oldcr->bounds.MaxX - _cr->bounds.MaxX);

            if (oldcr->bounds.MaxY - _cr->bounds.MaxY > 0)
              ySize = ySize - (oldcr->bounds.MaxY - _cr->bounds.MaxY);

            xDest = (_cr->bounds.MinX > oldcr->bounds.MinX) ?
                    0 :
                    (_cr->bounds.MinX - oldcr->bounds.MinX);

            yDest = (_cr->bounds.MinY > oldcr->bounds.MinY) ?
                    0 :
                    (_cr->bounds.MinY - oldcr->bounds.MinY);
            
            
            if (IS_SMARTREFRESH(l))
            {
              /*
               * Get a bitmap (if not there) and make a backup
               */
              if (NULL == _cr->BitMap)
              {
kprintf("Alloc bitmap!\n");
                _cr->BitMap = AllocBitMap(
                   _cr->bounds.MaxX - _cr->bounds.MinX + 1,
                   _cr->bounds.MaxY - _cr->bounds.MinY + 1,
                   display_bm->Depth,
                   BMF_CLEAR,
                   display_bm);
              }
            }

kprintf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            BltBitMap(srcbm,
                      xSrc,
                      ySrc,
                      _cr->BitMap,
                      xDest,
                      yDest,
                      xSize,
                      ySize,
                      0x0c0,
                      0xff,
                      NULL);
kprintf("!!!!!!backing up: from %d/%d to %d/%d  width:%d, height: %d\n",
        xSrc,
        ySrc,
        xDest,
        yDest,
        xSize,
        ySize);

            area-= (xSize * ySize);
            
          }
        }
        
      }
      _cr = _cr->Next;
    }
    
    _cr = oldcr->Next;
    if (oldcr->BitMap)
      FreeBitMap(oldcr->BitMap);
    _FreeClipRect(oldcr, l);
    oldcr = _cr;
  }
  
  l->ClipRect = firstcr;

  /*
   * The hid region must be subtracted from
   * the visible region of this layer.
   */
  ClearRegionRegion(hide_region, l->VisibleRegion);

  return TRUE;
}

int _ShowLayer(struct Layer * l, struct BitMap * display_bm)
{
  struct Region * r = NewRegion();
  struct RegionRectangle * rr;
  struct ClipRect * prevcr = NULL;
  int invisible = FALSE;
  if (NULL == r)
    return FALSE;
    
   OrRegionRegion(l->VisibleRegion, r);
  AndRegionRegion(l->shape, r);

  while (1)
  {
    rr = r->RegionRectangle;

if (NULL == rr)
  kprintf("\t\t empty region! invisible: %d\n",invisible);

    while (NULL != rr)
    {
      struct ClipRect * cr = AllocMem(sizeof(struct ClipRect), MEMF_CLEAR);

kprintf("\t\tinvisible: %d !!!!!!!!!!!!\n",invisible);
      cr->bounds.MinX = rr->bounds.MinX + r->bounds.MinX;
      cr->bounds.MinY = rr->bounds.MinY + r->bounds.MinY;
      cr->bounds.MaxX = rr->bounds.MaxX + r->bounds.MinX;
      cr->bounds.MaxY = rr->bounds.MaxY + r->bounds.MinY;
      cr->lobs = invisible;
      
      if (prevcr)
        prevcr->Next = cr;
      else
        l->ClipRect = cr;
       
      prevcr = cr;
      
      if (FALSE == invisible)
      {

kprintf("\t\tClearing background! %d/%d-%d/%d\n",
         cr->bounds.MinX,
         cr->bounds.MinY,
         cr->bounds.MaxX,
         cr->bounds.MaxY
         );
          _CallLayerHook(l->BackFill,
                         l->rp,
                         l,
                         &cr->bounds,
                         cr->bounds.MinX,
                         cr->bounds.MinY);
      }
      else
      {
        /*
         * This part is to be invisible!
         */
        if (IS_SMARTREFRESH(l))
        {
          cr->BitMap = AllocBitMap(
             cr->bounds.MaxX - cr->bounds.MinX + 1,
             cr->bounds.MaxY - cr->bounds.MinY + 1,
             display_bm->Depth,
             BMF_CLEAR,
             display_bm);
        }
      }
      
      rr=rr->Next;
    }
    
    if (FALSE == invisible)
    {
      XorRegionRegion(l->shape, r);
      invisible = TRUE;
    }
    else
      break;
  }
  
  return TRUE;
}


ClearRegionRegion(struct Region * rd, struct Region * r)
{
  struct RegionRectangle * rr = rd->RegionRectangle;
  while (rr) 
  {
    struct Rectangle rect;
    rect.MinX = r->bounds.MinX + rr->bounds.MinX;   
    rect.MinY = r->bounds.MinY + rr->bounds.MinY;   
    rect.MaxX = r->bounds.MinX + rr->bounds.MaxX;   
    rect.MaxY = r->bounds.MinY + rr->bounds.MaxY;   
    ClearRectRegion(r, &rect);
    rr = rr->Next;
  }
}
