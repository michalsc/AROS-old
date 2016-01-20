
/*
**
** $Id:$
**  anim.datatype 41.8
**
*/

#ifndef DEBUG
#   define DEBUG 0
#endif
#include <aros/debug.h>

struct ClassBase;
struct AnimInstData;
struct FrameNode;

/* main includes */
#include "classbase.h"
#include "classdata.h"

// ANIM-4
LONG generic_unpackanim4longdelta(struct AnimHeader *anhd, struct BitMap *bm, UBYTE *dlta, ULONG dltasize, ULONG flags )
{
    const ULONG *lists = (const ULONG *)dlta, *data;
    ULONG *pixels;
    UWORD *planeptr;
    UWORD x, count;
    UBYTE p;

    DFORMATS("[anim.datatype] %s()\n", __func__)

    for (p = 0; p < bm->Depth; p++)
    {
        data = (const ULONG *)((IPTR)dlta + AROS_BE2LONG(lists[p]));
        planeptr = (UWORD *)((IPTR)dlta + AROS_BE2LONG(lists[p + 8]));

        while (*planeptr != 0xFFFF)
        {
            pixels = (ULONG *)((IPTR)bm->Planes[p] + *planeptr++);
            count = *planeptr++;
            if (count < 0)
            {
                for (x = count; x < 0; x++)
                {
                    *pixels = *data++;
                    pixels = (ULONG *)((IPTR)pixels + (bm->BytesPerRow >> 2));
                }
                data++;
            }
            else
            {
                for (x = 0; x < count; x++)
                {
                    *pixels = *data++;
                    pixels = (ULONG *)((IPTR)pixels + (bm->BytesPerRow >> 2));
                }
            }
        }
    }

    return 0;
}

LONG generic_unpackanim4worddelta(struct AnimHeader *anhd, struct BitMap *bm, UBYTE *dlta, ULONG dltasize, ULONG flags )
{
    const ULONG *lists = (const ULONG *)dlta;
    const UWORD *data;
    UWORD *planeptr, *pixels;
    UWORD x, count;
    UBYTE p;

    DFORMATS("[anim.datatype] %s()\n", __func__)

    for (p = 0; p < bm->Depth; p++)
    {
        data = (const UWORD *)((IPTR)dlta + AROS_BE2LONG(lists[p]));
        planeptr = (UWORD *)((IPTR)dlta + AROS_BE2LONG(lists[p + 8]));

        while (*planeptr != 0xFFFF)
        {
            pixels = (UWORD *)((IPTR)bm->Planes[p] + *planeptr++);
            count = *planeptr++;
            if (count < 0)
            {
                for (x = count; x < 0; x++)
                {
                    *pixels = *data++;
                    pixels = (UWORD *)((IPTR)pixels + (bm->BytesPerRow >> 1));
                }
                data++;
            }
            else
            {
                for (x = 0; x < count; x++)
                {
                    *pixels = *data++;
                    pixels = (UWORD *)((IPTR)pixels + (bm->BytesPerRow >> 1));
                }
            }
        }
    }

    return 0;
}
