/*
    Copyright  2011, The AROS Development Team.
    $Id$
*/

#include <datatypes/pictureclass.h>
#include <proto/datatypes.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <string.h>

#include <libraries/mui.h> /* TODO: REMOVE needed for get() */

#include <aros/debug.h>

#include "newimage.h"

void DisposeImageContainer(struct NewImage *ni)
{
    if (ni)
    {
        if (ni->data)
        {
            FreeVec(ni->data);
        }
        if (ni->o) DisposeDTObject(ni->o);
        if (ni->filename) FreeVec(ni->filename);
        FreeVec(ni);
    }
}

void DisposeLUT8ImageContainer(struct NewLUT8Image *ni)
{
    if (ni)
    {
        if (ni->data)
        {
            FreeVec(ni->data);
        }
        FreeVec(ni);
    }
}

struct NewLUT8Image *NewLUT8ImageContainer(UWORD w, UWORD h)
{
    struct	NewLUT8Image *ni;

    ni = AllocVec(sizeof(struct NewLUT8Image), MEMF_ANY | MEMF_CLEAR);
    if (ni)
    {
        ni->w = w;
        ni->h = h;
        ni->data = AllocVec(w * h, MEMF_ANY | MEMF_CLEAR);
        if (ni->data == NULL)
        {
            FreeVec(ni);
            ni = NULL;
        }
    }
    return ni;
}

struct NewImage *NewImageContainer(UWORD w, UWORD h)
{
    struct	NewImage *ni;

    ni = AllocVec(sizeof(struct NewImage), MEMF_ANY | MEMF_CLEAR);
    if (ni)
    {
        ni->w = w;
        ni->h = h;
        ni->subimagescols = 1;
        ni->subimagesrows = 1;
        ni->data = AllocVec(w * h * sizeof (ULONG), MEMF_ANY | MEMF_CLEAR);
        if (ni->data == NULL)
        {
            FreeVec(ni);
            ni = NULL;
        }
    }
    return ni;
}

struct NewImage *GetImageFromFile(STRPTR path, STRPTR name,
    ULONG expectedsubimagescols, ULONG expectedsubimagesrows)
{
    struct	BitMapHeader       *bmhd = NULL;
    struct	NewImage           *ni = NULL;
    struct  BitMap             *map = NULL;
    struct  RastPort           *rp = NULL;
    Object                     *pic;
    struct	pdtBlitPixelArray   pa;
    char                        Buffer[256];
    UWORD                       w, h, tc, x, y;
    UBYTE                       mask;
    ULONG                       a;
    ULONG   *dst;

    strcpy(Buffer, name);

    pic = NewDTObject(Buffer,  DTA_SourceType,  DTST_FILE,
                               DTA_GroupID,     GID_PICTURE,
                               PDTA_Remap,      FALSE,
                               PDTA_DestMode,   PMODE_V43,
                               TAG_DONE);
    if (pic)
    {

        get(pic, PDTA_BitMapHeader, &bmhd);
        if(bmhd)
        {

            w = bmhd->bmh_Width;
            h = bmhd->bmh_Height;
            mask = bmhd->bmh_Masking;
            ni = NewImageContainer(w, h);
            ni->subimagescols = expectedsubimagescols;
            ni->subimagesrows = expectedsubimagesrows;
            if (ni)
            {
                int len = strlen(path) + strlen(Buffer) +2;
                ni->filename = AllocVec(len, MEMF_CLEAR | MEMF_ANY);
                if (ni->filename != NULL)
                {
                    strncpy(ni->filename, path, len);
                    AddPart(ni->filename, Buffer, len);

                    pa.MethodID = PDTM_READPIXELARRAY;
                    pa.pbpa_PixelData = (APTR) ni->data;
                    pa.pbpa_PixelFormat = PBPAFMT_ARGB;
                    pa.pbpa_PixelArrayMod = w*4;
                    pa.pbpa_Left = 0;
                    pa.pbpa_Top = 0;
                    pa.pbpa_Width = w;
                    pa.pbpa_Height = h;
                    DoMethodA(pic, (Msg) &pa);
                    ni->ok = TRUE;
                    if (bmhd->bmh_Depth <= 8)
                    {
                        get(pic, PDTA_BitMap, &map);
                        if (map && (mask == mskHasTransparentColor))
                        {
                            rp = CreateRastPort();
                            if (rp) rp->BitMap = map;
                            tc = bmhd->bmh_Transparent;
                        }
                    }

                    if (rp)
                    {
                        dst = ni->data;
                        for (y = 0; y < h; y++)
                        {
                            for (x = 0; x < w; x++)
                            {
    #if !AROS_BIG_ENDIAN
                            if (ReadPixel(rp, x, y) == 0) dst[x+y*w] &= 0xffffff00; else dst[x+y*w] |= 0x000000ff;
    #else
                            if (ReadPixel(rp, x, y) == 0) dst[x+y*w] &= 0x00ffffff; else dst[x+y*w] |= 0xff000000;
    #endif
                            }
                        }
                        FreeRastPort(rp);
                    }
                    else
                    {
    
                        if (mask != mskHasAlpha)
                        {
    #if !AROS_BIG_ENDIAN
                            for (a= 0; a < (w*h); a++) ni->data[a] |= 0x000000ff;
    #else
                            for (a= 0; a < (w*h); a++) ni->data[a] |= 0xff000000;
    #endif
                        }
                    }
                }
                else 
                {
                    DisposeImageContainer(ni);
                    ni = NULL;
                }
            }
        }

        DisposeDTObject(pic);
    }

    return ni;
}

void RemoveLUTImage(struct NewImage *ni)
{
    if (ni)
    {
        if (ni->ok)
        {
            if (ni->o) DisposeDTObject(ni->o);
            ni->o = NULL;
            ni->bitmap = NULL;
            ni->mask = NULL;
        }
    }
}

static Object *LoadPicture(CONST_STRPTR filename, struct Screen *scr)
{
    Object *o;


    o = NewDTObject((APTR)filename,
    DTA_GroupID          , GID_PICTURE,
    OBP_Precision        , PRECISION_EXACT,
    PDTA_Screen          , (IPTR)scr,
    PDTA_FreeSourceBitMap, TRUE,
    PDTA_DestMode        , PMODE_V43,
    PDTA_UseFriendBitMap , TRUE,
    TAG_DONE);
    

    if (o)
    {
    struct BitMapHeader *bmhd;
    
    GetDTAttrs(o,PDTA_BitMapHeader, (IPTR)&bmhd, TAG_DONE);
    
    struct FrameInfo fri = {0};
    
    D(bug("DTM_FRAMEBOX\n", o));
    DoMethod(o,DTM_FRAMEBOX,NULL,(IPTR)&fri,(IPTR)&fri,sizeof(struct FrameInfo),0);
    
    if (fri.fri_Dimensions.Depth>0)
    {
        D(bug("DTM_PROCLAYOUT\n", o));
        if (DoMethod(o,DTM_PROCLAYOUT,NULL,1))
        {
        return o;
        }
    }
    DisposeDTObject(o);
    }
    return NULL;
}


void SetImage(struct NewImage *in, struct NewImage *out, BOOL truecolor, struct Screen* scr)
{
    out->ok = FALSE;
    if (in != NULL)
    {
        out->w = in->w;
        out->h = in->h;
        out->subimagescols = in->subimagescols;
        out->subimagesrows = in->subimagesrows;
        out->istiled = in->istiled;
        out->data = in->data;
        out->tile_left = in->tile_left;
        out->tile_right = in->tile_right;
        out->tile_top = in->tile_top;
        out->tile_bottom = in->tile_bottom;
        out->inner_left = in->inner_left;
        out->inner_right = in->inner_right;
        out->inner_top = in->inner_top;
        out->inner_bottom = in->inner_bottom;
        out->bitmap = NULL;
        out->mask = NULL;
        out->o = NULL;
        out->ok = (in->data != NULL) ? TRUE : FALSE;
        if (!truecolor)
        {
            out->ok = FALSE;
            STRPTR file = AllocVec(strlen(in->filename) + 5, MEMF_ANY | MEMF_CLEAR);
            if (file != NULL)
            {
                strcpy(file, in->filename);
                strcat(file, "_LUT");
                out->o = LoadPicture(file, scr);
                FreeVec(file);
            }
            out->filename = in->filename; 
            if (out->o == NULL) out->o = LoadPicture(in->filename, scr);
            if (out->o)
            {
                    GetDTAttrs(out->o, PDTA_DestBitMap, (IPTR)&out->bitmap, TAG_DONE);
                    if (out->bitmap == NULL) GetDTAttrs(out->o, PDTA_BitMap, (IPTR)&out->bitmap, TAG_DONE);

                    if (out->bitmap) GetDTAttrs(out->o, PDTA_MaskPlane, (IPTR)&out->mask, TAG_DONE);
                    if (out->bitmap == NULL)
                    {
                        DisposeDTObject(out->o);
                        out->o = NULL;
                    } else out->ok = TRUE;
            }
        }
    }
}

struct Region *RegionFromLUT8Image(int w, int h, struct NewLUT8Image *s)
{
    struct Region  *shape;
    int             x, y;
    BOOL            transp, transpstate;
    BOOL            failed = FALSE;

    if (s == NULL) return NULL;

    UBYTE   *src = s->data;

    shape = NewRegion();

    if (shape) {
        struct Rectangle rect = {0, s->h, w-1, h-1};
        if (!OrRectRegion(shape, &rect)) failed = TRUE;
        for(y = 0; y < s->h; y++)
        {
            struct Rectangle rect = {0, y, 0, y};
            transpstate = TRUE;
            for(x = 0; x < s->w; x++)
            {
                transp = src[y * s->w + x] == 0;
                if (transpstate)
                {
                    if (!transp)
                    {
                        rect.MaxX = rect.MinX = x;
                        transpstate = FALSE;
                    }
                } else {
                    if (transp)
                    {
                        OrRectRegion(shape, &rect);
                        transpstate = TRUE;
                    }
                    else
                    {
                        rect.MaxX++;
                    }
                }
            }
            if (!transpstate) if (!OrRectRegion(shape, &rect)) failed = TRUE;
        }
        if (failed)
        {
            DisposeRegion(shape);
            shape = NULL;
        }
    }
    return shape;
}
