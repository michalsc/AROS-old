/*
    (C) 1997 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: english
*/
#include <aros/libcall.h>
#include <graphics/clip.h>
#include <graphics/layers.h>
#include <utility/tagitem.h>
#include "basicfuncs.h"

/*****************************************************************************

    NAME */
#include <proto/layers.h>
	AROS_LH9(struct Layer *, CreateBehindHookLayer,

/*  SYNOPSIS */
	AROS_LHA(struct Layer_Info *, li, A0),
	AROS_LHA(struct BitMap     *, bm, A1),
	AROS_LHA(LONG               , x0, D0),
	AROS_LHA(LONG               , y0, D1),
	AROS_LHA(LONG               , x1, D2),
	AROS_LHA(LONG               , y1, D3),
	AROS_LHA(LONG               , flags, D4),
	AROS_LHA(struct Hook       *, hook, A3),
	AROS_LHA(struct BitMap     *, bm2, A2),

/*  LOCATION */
	struct LayersBase *, LayersBase, 32, Layers)

/*  FUNCTION
        Create a new layer at the given position and with the
        given size. The new layer will be in front of all other
        layers. If it is a backdrop layer it will be created
        in front of all other backdrop layers and behind all
        non backdrop layers.
        Install the given hook as a backfill hook. This hook will
        be called whenever a part of the layer is supposed to be
        filled with a certain pattern. The backfill hook has to
        do that.
        If a super bitmap layer is wanted the flags LAYERSUPER and
        the flag LAYERSMART have to be set and a pointer to a 
        bitmap must also be passed to this function. 

    INPUTS
        li    - pointer to LayerInfo structure
        bm    - pointer to common bitmap
        x0, y0- upper left corner of the layer
        x1, y1- lower right corner of the layer
        flags - choose the type of layer by setting some flags
        hook  - pointer to the backfill hook of this layer
                The backfill hook will be called with
                     object = (struct RastPort *) result->RastPort
                and message = [ (struct Layer *) layer,
                                (struct Rectangle) bounds,
                                (WORD) offsetx,
                                (WORD) offsety ]
        bm2   - pointer to optional super bitmap. 
        
    RESULT
        Pointer to the newly created layer. NULL if layer could not be 
        created (Probably out of memory).
        
    NOTES
        Does not allow to create layers that are partially outside
        the given bitmap (, yet).

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    layers_lib.fd and clib/layers_protos.h

*****************************************************************************/
{
  AROS_LIBFUNC_INIT
  AROS_LIBBASE_EXT_DECL(struct LayersBase *,LayersBase)

  struct Region * lr = NewRegion();
  struct Rectangle r;
  struct TagItem tagList[5] = {{LA_Priority    , 0},
                               {LA_Hook        , NULL},
                               {LA_SuperBitMap , NULL},
                               {LA_Shape       , (ULONG)lr},
                               {TAG_DONE       , 0UL}};

  tagList[0].ti_Data = (LAYERBACKDROP == (flags & LAYERBACKDROP)) ?
                       BACKDROPPRIORITY:
                       UPFRONTPRIORITY;
  tagList[1].ti_Data = (ULONG)hook;
  tagList[2].ti_Data = (ULONG)bm2;

  r.MinX = x0;
  r.MaxX = x1;
  r.MinY = y0;
  r.MaxY = y1;

  if (FALSE == OrRectRegion(lr, &r))
    return FALSE;

  return CreateLayerTagList(li,
                            bm,
                            flags,
                            &tagList[0]);
  
  AROS_LIBFUNC_EXIT
} /* CreateBehindHookLayer */
