/*
    (C) 1997 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: english
*/
#include <aros/libcall.h>
#include "layers_intern.h"
#include "basicfuncs.h"
#include <exec/types.h>

/*****************************************************************************

    NAME */
#include <proto/layers.h>

	AROS_LH3(void, DoHookClipRects,

/*  SYNOPSIS */
	AROS_LHA(struct Hook      *, hook ,  A0),
	AROS_LHA(struct RastPort  *, rport,  A1),
	AROS_LHA(struct Rectangle *, rect ,  A2),

/*  LOCATION */
	struct LayersBase *, LayersBase, 36, Layers)

/*  FUNCTION

    INPUTS
        hook  - pointer to the hook to be called for the cliprects of
                the given layer
               
        rport - pointer to the rastport where the layers upon which the
                hook is to be called
        
        rect  - no operation is allowed outside this rectangle. If a layer
                is bigger than this rectangle only operations in the
                common area are allowed.

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    layers_lib.fd and clib/layers_protos.h

*****************************************************************************/
{ /* sv */
  AROS_LIBFUNC_INIT
  AROS_LIBBASE_EXT_DECL(struct LayersBase *,LayersBase)

  struct Layer     *L;

  D(bug("DoHookClipRects(hook @ $%lx, rport @ $%lx, rect @ $%lx)\n", hook, rport, rect));

  /* if the hook is LAYERS_NOBACKFILL then I am not doing anything here. */
  if(hook == (struct Hook *)LAYERS_NOBACKFILL)
  {
    return;
  }

  L = rport -> Layer;
  /* does this rastport have a layer?? */
  if( NULL == L )
  {
    /* non-layered rastport */

    /* You MUST supply a rect to clip the hook's actions! */
    _CallLayerHook(hook, rport, NULL, rect, rect->MinX, rect->MinY);
  }
  else
  {
    struct Rectangle boundrect;

    /* layered rastport */

    /* I assume that the given bounds rectangle is relative to the upper left corner of the
       layer's rastport. This makes more sense than if it was relative to the rastport of
       the screen where this layer is to be found in
    */
    boundrect.MinX = rect->MinX + L->bounds.MinX;
    boundrect.MinY = rect->MinY + L->bounds.MinY;
    boundrect.MaxX = rect->MaxX + L->bounds.MinX;
    boundrect.MaxY = rect->MaxY + L->bounds.MinY;

    /* first check whether this layer is to be considered at all */
    if (!(boundrect.MinX > L->bounds.MaxX ||
          boundrect.MinY > L->bounds.MaxY ||
          boundrect.MaxX < L->bounds.MinX ||
          boundrect.MaxY < L->bounds.MinY))
    {
      /* yes, that's a layer to be considered */
      /* I want nobody else to interrupt me while I call the hook for this layer */
      struct ClipRect * CR = L->ClipRect;
      LockLayer(0, L);

      /* process all ClipRects of this layer */
      while (NULL != CR)
      {
        /* I am going to call the hook for all visible cliprects and
           for invisible cliprects belonging to smart or superbitmap
           layers =>
           So I am not calling it for invisble cliprects belonging to
           a simple layer.
        */

        if (!(NULL != CR->lobs && 
              0    != (L->Flags & LAYERSIMPLE)) )
        {
          struct Rectangle bounds;
          /* That's a ClipRect to visit, if it's inside the given rectangle */
          /* Generate the bounds rectangle. This rectangle shows the part
             of the clipRect that is supposed to be changed. So it might get
             the coordinates of the ClipRect, but it can also be smaller. */
            
          bounds.MinX = (boundrect.MinX > CR->bounds.MinX) ? boundrect.MinX 
                                                           : CR->bounds.MinX;
          bounds.MinY = (boundrect.MinY > CR->bounds.MinY) ? boundrect.MinY 
                                                           : CR->bounds.MinY;
          bounds.MaxX = (boundrect.MaxX < CR->bounds.MaxX) ? boundrect.MaxX 
                                                           : CR->bounds.MaxX;
          bounds.MaxY = (boundrect.MaxY < CR->bounds.MaxY) ? boundrect.MaxY 
                                                           : CR->bounds.MaxY;

          /* Is the cliprect inside the bounds... */
          if (bounds.MinX <= bounds.MaxX && bounds.MinY <= bounds.MaxY)
          {
            struct BitMap * bm;
            WORD offsetX;
            WORD offsetY;
            /* Call the hook for the rectangle given by bounds. */
            
            /* If the ClipRect is hidden, then this might get special..., but
               only for non-simple layers */
            if (NULL != CR->lobs && 0 == (L->Flags & LAYERSIMPLE))
	    {
              /* calculate the offsets before making changes to bounds!! */
              offsetX = bounds.MinX - L->bounds.MinX - L->Scroll_X; 
              offsetY = bounds.MinY - L->bounds.MinY - L->Scroll_Y;
              bm = rport->BitMap;

              if (0 != (L->Flags & LAYERSUPER))
	      {
                /* it's a superbitmap layer */
                bounds.MinX -= ( L->bounds.MinX + L->Scroll_X );
                bounds.MinY -= ( L->bounds.MinY + L->Scroll_Y );
                bounds.MaxX -= ( L->bounds.MinX + L->Scroll_X );
                bounds.MaxY -= ( L->bounds.MinY + L->Scroll_Y );
                rport->BitMap = L->SuperBitMap;
	      }
              else
	      {
                /* it's a smart layer but not superbitmap */
                /* it's hidden, the hook has to blit into the hidden cliprect's bitmap now */
                /* adjust the bounds */
                bounds.MinX = bounds.MinX - CR->bounds.MinX + ALIGN_OFFSET(CR->bounds.MinX);
                bounds.MinY = bounds.MinY - CR->bounds.MinY;
                bounds.MaxX = bounds.MaxX - CR->bounds.MinX + ALIGN_OFFSET(CR->bounds.MinX);
                bounds.MaxY = bounds.MaxY - CR->bounds.MinY;
                rport->BitMap = CR->BitMap;
	      }

              _CallLayerHook(hook, rport, L, &bounds, offsetX, offsetY);
              rport->BitMap = bm;
	    }
            else
	    {
              /* make sure it's not hidden as the case "simple layer and hidden" also gets here */
              if (NULL == CR->lobs)
                _CallLayerHook(hook, rport, L, &bounds, bounds.MinX, bounds.MinY);
	    }
          }
        }

        CR = CR->Next;
      }

      UnlockLayer(L);
    }
  }
    AROS_LIBFUNC_EXIT
} /* DoHookClipRects */







