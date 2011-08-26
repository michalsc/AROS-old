/*
    Copyright � 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Graphics planar bitmap class implementation.
    Lang: english
*/

/****************************************************************************************/

#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/oop.h>

#include <exec/memory.h>
#include <utility/tagitem.h>
#include <graphics/gfx.h>
#include <oop/oop.h>

#include <hidd/graphics.h>

#include <string.h>

#include "graphics_intern.h"

#include <string.h>

#define DEBUG 0
#include <aros/debug.h>

/****************************************************************************************/

OOP_Object *PBM__Root__New(OOP_Class *cl, OOP_Object *o, struct pRoot_New *msg)
{
    IPTR height, bytesperrow;
    UBYTE depth;
    BOOL ok = FALSE;
     
#if 0
    	/* Set the bitmaps' pixelformat */
    struct TagItem pf_tags[] =
    {
      	{ aHidd_PixFmt_ColorModel   , vHidd_ColorModel_Palette	}, /* 0 */
      	{ aHidd_PixFmt_Depth	    , 0				}, /* 1 */
      	{ aHidd_PixFmt_BytesPerPixel, 0				}, /* 2 */
      	{ aHidd_PixFmt_BitsPerPixel , 0				}, /* 3 */
      	{ aHidd_PixFmt_StdPixFmt    , 0				}, /* 4 */
      	{ aHidd_PixFmt_CLUTShift    , 0				}, /* 5 */
	{ aHidd_PixFmt_CLUTMask     , 0x000000FF		}, /* 6 */
	{ aHidd_PixFmt_RedMask	    , 0x00FF0000		}, /* 7 */
	{ aHidd_PixFmt_GreenMask    , 0x0000FF00		}, /* 8 */
	{ aHidd_PixFmt_BlueMask     , 0x000000FF		}, /* 9 */
	{ aHidd_PixFmt_BitMapType   , vHidd_BitMapType_Planar	},
	{ TAG_DONE  	    	    , 0UL   	    	    	}
    };
#endif

    struct planarbm_data *data;
#if 0
    OOP_Object      	 *pf;
    APTR		 p_pf = &pf;
#endif

    o =(OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
    if (NULL == o)
    	return NULL;
	
    data = OOP_INST_DATA(cl, o);

    /* Get some data about the dimensions of the bitmap */

    data->planes_alloced = (BOOL)GetTagData(aHidd_PlanarBM_AllocPlanes, TRUE, msg->attrList);

    /* FIXME: Fix this hack */
    /* Because this class is used to emulate Amiga bitmaps, we
       have to see if it should have late initialisation
    */
    if (!data->planes_alloced)
	return o; /* Late initialization */

    /* By default we create 1-plane bitmap */
    depth = GetTagData(aHidd_BitMap_Depth, 1, msg->attrList);

    /* Not late initialization. Get some info on the bitmap */	
    OOP_GetAttr(o, aHidd_BitMap_Height,	&height);
    OOP_GetAttr(o, aHidd_BitMap_BytesPerRow, &bytesperrow);
#if 0
    OOP_GetAttr(o,  aHidd_BitMap_PixFmt, (IPTR *)p_pf);
    OOP_GetAttr(pf, aHidd_PixFmt_Depth, (IPTR *)&depth);
#endif

    /* We cache some info */
    data->rows        = height;
    data->bytesperrow = bytesperrow;
    data->depth       = depth;

    /* Allocate memory for plane array */
    data->planes = AllocVec(sizeof (UBYTE *) * depth, MEMF_ANY|MEMF_CLEAR);
    if (data->planes)
    {
	ok = TRUE;
	UBYTE i;

	data->planebuf_size = depth;

	/* Allocate all the planes */
	for ( i = 0; i < depth && ok; i ++)
	{
	    data->planes[i] = AllocVec(height * data->bytesperrow, MEMF_ANY|MEMF_CLEAR);
	    if (NULL == data->planes[i])
	    {
	        ok = FALSE;
	        break;
	    }
	}
    }

    if (!ok)
    {
	OOP_MethodID dispose_mid;

	dispose_mid = OOP_GetMethodID(IID_Root, moRoot_Dispose);
	OOP_CoerceMethod(cl, o, (OOP_Msg)&dispose_mid);

	o = NULL;
    }
    	
    return o;
}

/****************************************************************************************/

VOID PBM__Root__Dispose(OOP_Class *cl, OOP_Object *o, OOP_Msg msg)
{
    struct planarbm_data    *data;
    UBYTE   	    	    i;
    
    data = OOP_INST_DATA(cl, o);
    
    if (data->planes_alloced)
    {
	if (NULL != data->planes)
	{
    	    for (i = 0; i < data->depth; i ++)
	    {
		if (NULL != data->planes[i])
		{
		    FreeVec(data->planes[i]);
		}
	    }
	    FreeVec(data->planes);
	}
    }
    
    OOP_DoSuperMethod(cl, o, msg);
    
    return;
}

/****************************************************************************************/

VOID PBM__Root__Get(OOP_Class *cl, OOP_Object *obj, struct pRoot_Get *msg)
{
    struct planarbm_data *data = OOP_INST_DATA(cl, obj);
    ULONG idx;

    if (IS_BITMAP_ATTR(msg->attrID, idx))
    {
        switch(idx)
        {
        case aoHidd_BitMap_Depth:
            /* Planar bitmaps may have a variable depth. */
	    *msg->storage = data->depth;
	    return;
	}
    }

    OOP_DoSuperMethod(cl, obj, &msg->mID);
}

/****************************************************************************************/

VOID PBM__Hidd_BitMap__PutPixel(OOP_Class *cl, OOP_Object *o,
				struct pHidd_BitMap_PutPixel *msg)
{
    UBYTE   	    	    **plane;
    struct planarbm_data    *data;
    ULONG   	    	    offset;
    ULONG   	    	    mask;
    UBYTE   	    	    pixel, notpixel;
    UBYTE   	    	    i;
    
    data = OOP_INST_DATA(cl, o);

    /* bitmap in plane-mode */
    plane     = data->planes;
    offset    = msg->x / 8 + msg->y * data->bytesperrow;
    pixel     = 128 >> (msg->x % 8);
    notpixel  = ~pixel;
    mask      = 1;

    for(i = 0; i < data->depth; i++, mask <<=1, plane ++)
    {  
    	if ((*plane != NULL) && (*plane != (UBYTE *)-1))
	{	
	    if(msg->pixel & mask)
	    {
		*(*plane + offset) = *(*plane + offset) | pixel;
	    }
	    else
	    {
		*(*plane + offset) = *(*plane + offset) & notpixel;
	    }
        }
    }
}

/****************************************************************************************/

ULONG PBM__Hidd_BitMap__GetPixel(OOP_Class *cl, OOP_Object *o,
				 struct pHidd_BitMap_GetPixel *msg)
{
    struct planarbm_data    *data;
    UBYTE   	    	    **plane;
    ULONG   	    	    offset;
    ULONG   	    	    i;
    UBYTE   	    	    pixel;
    ULONG   	    	    retval;
         
    data = OOP_INST_DATA(cl, o);

    plane     = data->planes;
    offset    = msg->x / 8 + msg->y * data->bytesperrow;
    pixel     = 128 >> (msg->x % 8);
    retval    = 0;

    for(i = 0; i < data->depth; i++, plane ++)
    {
    
        if (*plane == (UBYTE *)-1)
	{
	    retval = retval | (1 << i);
	}
	else if (*plane != NULL)
	{	
	    if(*(*plane + offset) & pixel)
	    {
		retval = retval | (1 << i);
	    }
	}
    }
    
    return retval; 
}

/****************************************************************************************/

VOID PBM__Hidd_BitMap__PutImage(OOP_Class *cl, OOP_Object *o,
				struct pHidd_BitMap_PutImage *msg)
{
    WORD    	    	    x, y, d;
    UBYTE   	    	    *pixarray = (UBYTE *)msg->pixels;
    UBYTE   	    	    **plane;
    ULONG   	    	    planeoffset;
    struct planarbm_data    *data;  

    if ((msg->pixFmt != vHidd_StdPixFmt_Native) &&
    	(msg->pixFmt != vHidd_StdPixFmt_Native32))
    {
    	OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
	return;
    }
    
    data = OOP_INST_DATA(cl, o);
    
    planeoffset = msg->y * data->bytesperrow + msg->x / 8;
    
    for(y = 0; y < msg->height; y++)
    {
    	switch(msg->pixFmt)
	{
	    case vHidd_StdPixFmt_Native:
	    {
	     	UBYTE *src = pixarray;
	
    		plane = data->planes;

    		for(d = 0; d < data->depth; d++)
		{
		    ULONG dmask = 1L << d;
		    ULONG pmask = 0x80 >> (msg->x & 7);
		    UBYTE *pl = *plane;

		    if (pl == (UBYTE *)-1) continue;
		    if (pl == NULL) continue;

		    pl += planeoffset;

    		    for(x = 0; x < msg->width; x++)
		    {
	    		if (src[x] & dmask)
			{
			    *pl |= pmask;
			}
			else
			{
			    *pl &= ~pmask;
			}

			if (pmask == 0x1)
			{
			    pmask = 0x80;
			    pl++;
			}
			else
			{
			    pmask >>= 1;
			}

		    } /* for(x = 0; x < msg->width; x++) */

		    plane++;

		} /* for(d = 0; d < data->depth; d++) */

		pixarray += msg->modulo;
		planeoffset += data->bytesperrow;
	    }
	    break;

	    case vHidd_StdPixFmt_Native32:
	    {
	     	HIDDT_Pixel *src = (HIDDT_Pixel *)pixarray;
	
    		plane = data->planes;

    		for(d = 0; d < data->depth; d++)
		{
		    ULONG dmask = 1L << d;
		    ULONG pmask = 0x80 >> (msg->x & 7);
		    UBYTE *pl = *plane;

		    if (pl == (UBYTE *)-1) continue;
		    if (pl == NULL) continue;

		    pl += planeoffset;

    		    for(x = 0; x < msg->width; x++)
		    {
	    		if (src[x] & dmask)
			{
			    *pl |= pmask;
			}
			else
			{
			    *pl &= ~pmask;
			}

			if (pmask == 0x1)
			{
			    pmask = 0x80;
			    pl++;
			}
			else
			{
			    pmask >>= 1;
			}

		    } /* for(x = 0; x < msg->width; x++) */

		    plane++;

		} /* for(d = 0; d < data->depth; d++) */

		pixarray += msg->modulo;
		planeoffset += data->bytesperrow;
	    }
	    
	    break;
	    
	} /* switch(msg->pixFmt) */    
	
    } /* for(y = 0; y < msg->height; y++) */
}

/****************************************************************************************/

VOID PBM__Hidd_BitMap__PutImageLUT(OOP_Class *cl, OOP_Object *o,
				   struct pHidd_BitMap_PutImageLUT *msg)
{
    WORD    	    	    x, y, d;
    UBYTE   	    	    *pixarray = (UBYTE *)msg->pixels;
    UBYTE   	    	    **plane;
    ULONG   	    	    planeoffset;
    struct planarbm_data   *data;  
    
    data = OOP_INST_DATA(cl, o);
    
    planeoffset = msg->y * data->bytesperrow + msg->x / 8;
    
    for(y = 0; y < msg->height; y++)
    {
    	UBYTE *src = pixarray;
	
    	plane = data->planes;
	
    	for(d = 0; d < data->depth; d++)
	{
	    ULONG dmask = 1L << d;
	    ULONG pmask = 0x80 >> (msg->x & 7);
	    UBYTE *pl = *plane;
	    
	    if (pl == (UBYTE *)-1) continue;
	    if (pl == NULL) continue;
	    
	    pl += planeoffset;

    	    for(x = 0; x < msg->width; x++)
	    {
	    	if (src[x] & dmask)
		{
		    *pl |= pmask;
		}
		else
		{
		    *pl &= ~pmask;
		}
		
		if (pmask == 0x1)
		{
		    pmask = 0x80;
		    pl++;
		}
		else
		{
		    pmask >>= 1;
		}
		
	    } /* for(x = 0; x < msg->width; x++) */
	    
	    plane++;
	    
	} /* for(d = 0; d < data->depth; d++) */
	
	pixarray += msg->modulo;
	planeoffset += data->bytesperrow;
	
    } /* for(y = 0; y < msg->height; y++) */
}

/****************************************************************************************/

VOID PBM__Hidd_BitMap__GetImageLUT(OOP_Class *cl, OOP_Object *o,
				   struct pHidd_BitMap_GetImageLUT *msg)
{
    WORD    	    	    x, y, d;
    UBYTE   	    	    *pixarray = (UBYTE *)msg->pixels;
    UBYTE   	    	    **plane;
    ULONG   	    	    planeoffset;
    struct planarbm_data    *data;  
    UBYTE   	    	    prefill;
    
    data = OOP_INST_DATA(cl, o);
    
    planeoffset = msg->y * data->bytesperrow + msg->x / 8;
    
    prefill = 0;
    for(d = 0; d < data->depth; d++)
    {
    	if (data->planes[d] == (UBYTE *)-1)
	{
	    prefill |= (1L << d);
	}
    }
    
    for(y = 0; y < msg->height; y++)
    {
    	UBYTE *dest = pixarray;
	
    	plane = data->planes;
	
	for(x = 0; x < msg->width; x++)
	{
	    dest[x] = prefill;
	}
	
    	for(d = 0; d < data->depth; d++)
	{
	    ULONG dmask = 1L << d;
	    ULONG pmask = 0x80 >> (msg->x & 7);
	    UBYTE *pl = *plane;
	    
	    if (pl == (UBYTE *)-1) continue;
	    if (pl == NULL) continue;
	    
	    pl += planeoffset;

    	    for(x = 0; x < msg->width; x++)
	    {
	    	if (*pl & pmask)
		{
		    dest[x] |= dmask;
		}
		else
		{
		    dest[x] &= ~dmask;
		}
		
		if (pmask == 0x1)
		{
		    pmask = 0x80;
		    pl++;
		}
		else
		{
		    pmask >>= 1;
		}
		
	    } /* for(x = 0; x < msg->width; x++) */
	    
	    plane++;
	    
	} /* for(d = 0; d < data->depth; d++) */
	
	pixarray += msg->modulo;
	planeoffset += data->bytesperrow;
	
    } /* for(y = 0; y < msg->height; y++) */
    
}

/****************************************************************************************/

BOOL PBM__Hidd_PlanarBM__SetBitMap(OOP_Class *cl, OOP_Object *o,
				   struct pHidd_PlanarBM_SetBitMap *msg)
{
    struct planarbm_data *data;
    struct BitMap   	 *bm;
    struct TagItem  	 pftags[] =
    {
    	{ aHidd_PixFmt_Depth	    , 0UL			},	/* 0 */
    	{ aHidd_PixFmt_BitsPerPixel , 0UL			},	/* 1 */
    	{ aHidd_PixFmt_BytesPerPixel, 1UL			},	/* 2 */
    	{ aHidd_PixFmt_ColorModel   , vHidd_ColorModel_Palette	},	/* 3 */
    	{ aHidd_PixFmt_BitMapType   , vHidd_BitMapType_Planar	},	/* 4 */
    	{ aHidd_PixFmt_CLUTShift    , 0UL			},	/* 5 */
    	{ aHidd_PixFmt_CLUTMask     , 0x000000FF		},	/* 6 */
	{ aHidd_PixFmt_RedMask	    , 0x00FF0000		}, 	/* 7 */
	{ aHidd_PixFmt_GreenMask    , 0x0000FF00		}, 	/* 8 */
	{ aHidd_PixFmt_BlueMask     , 0x000000FF		}, 	/* 9 */
	{ aHidd_PixFmt_StdPixFmt    , vHidd_StdPixFmt_Plane	},
    	{ TAG_DONE  	    	    , 0UL   	    	    	}
    };
    struct TagItem  	bmtags[] =
    {
	{ aHidd_BitMap_Width	    , 0 },
	{ aHidd_BitMap_Height	    , 0 },
	{ aHidd_BitMap_Depth	    , 0 },
	{ aHidd_BitMap_BytesPerRow  , 0 },
    	{ aHidd_BitMap_PixFmtTags   , 0 },
	{ TAG_DONE  	    	    , 0 }
    };
    struct pHidd_BitMap_SetBitMapTags;
    ULONG i;

    data = OOP_INST_DATA(cl, o);
    bm = msg->bitMap;
    
    if (data->planes_alloced)
    {
    	D(bug(" !!!!! PlanarBM: Trying to set bitmap in one that already has planes allocated\n"));
	return FALSE;
    }
    
    /* Check if plane array already allocated */
    if (NULL != data->planes)
    {
    	if (bm->Depth > data->planebuf_size)
	{
	    FreeVec(data->planes);
	    data->planes = NULL;
	}
    }
    
    if (NULL == data->planes)
    {
	data->planes = AllocVec(sizeof (UBYTE *) * bm->Depth, MEMF_CLEAR);

	if (NULL == data->planes)
	     return FALSE;
	     
	data->planebuf_size = bm->Depth;
    }
    
    
    /* Update the planes */
    for (i = 0; i < data->planebuf_size; i ++)
    {
    	if (i < bm->Depth) 
   	    data->planes[i] = bm->Planes[i];
	else
	    data->planes[i] = NULL;
    }

    data->depth		= bm->Depth;
    data->bytesperrow	= bm->BytesPerRow;
    data->rows		= bm->Rows;
    pftags[0].ti_Data = bm->Depth;	/* PixFmt_Depth */
    pftags[1].ti_Data = bm->Depth;	/* PixFmt_BitsPerPixel */
    bmtags[0].ti_Data = bm->BytesPerRow * 8;
    bmtags[1].ti_Data = bm->Rows;
    bmtags[2].ti_Data = bm->Depth;
    bmtags[3].ti_Data = bm->BytesPerRow;
    bmtags[4].ti_Data = (IPTR)pftags;

    /* Call private bitmap method to update superclass */
    if (!BM__Hidd_BitMap__SetBitMapTags(CSD(cl)->bitmapclass, o, bmtags))
    {
    	ULONG i;
	
	for (i = 0; i < data->planebuf_size; i ++)
	{
	    data->planes[i] = NULL;
	}
    }

    return TRUE;
}

/****************************************************************************************/

BOOL PBM__Hidd_PlanarBM__GetBitMap(OOP_Class *cl, OOP_Object *o,
				   struct pHidd_PlanarBM_GetBitMap *msg)
{
    struct planarbm_data *data = OOP_INST_DATA(cl, o);
    ULONG i;

    msg->bitMap->Depth	     = data->depth;
    msg->bitMap->BytesPerRow = data->bytesperrow;
    msg->bitMap->Rows	     = data->rows;
    msg->bitMap->Flags	     = BMF_STANDARD|BMF_MINPLANES; /* CHECKME */
    msg->bitMap->pad	     = 0;

    for (i = 0; i < data->planebuf_size; i++)
        msg->bitMap->Planes[i] = data->planes[i];

    return TRUE;
}
