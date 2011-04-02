/*
    Copyright  2011, The AROS Development Team.
    $Id$
*/

#ifndef NEWIMAGE_H
#define NEWIMAGE_H

#include <exec/types.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>

struct NewImage
{
    ULONG  *data; /* Buffer of RAW ARGB data */
    UWORD   w;
    UWORD   h;
    BOOL    ok;

    ULONG   subimagescols;  /* Number of columns of subimages in image, ie. states of gadget*/
    ULONG   subimagesrows;  /* Number of rows of subimages in image, ie. states of gadget*/
    BOOL    *subimageinbm;  /* Array marking which subimage in present in *bitmap */


    BOOL    istiled;
    UWORD   tile_left, tile_top, tile_bottom, tile_right;

    Object  *o;                 /* Stored DT object */
    APTR    mask;               /* Mask pointer from DT object */
    struct  BitMap  *bitmap;    /* Bitmap pointer from DT object */
    STRPTR  filename;
    
    struct  BitMap  *bitmap2;   /* Allocated bitmap object */
};

struct  NewLUT8Image
{
    UWORD   w;
    UWORD   h;
    UBYTE  *data;
};

struct NewImage *NewImageContainer(UWORD w, UWORD h);
void DisposeImageContainer(struct NewImage *ni);

struct NewImage *GetImageFromFile(STRPTR path, STRPTR name,
    ULONG expectedsubimagescols, ULONG expectedsubimagesrows);
struct NewImage * CreateNewImageContainerMatchingScreen(struct NewImage *in, BOOL truecolor, struct Screen* scr);

struct NewLUT8Image *NewLUT8ImageContainer(UWORD w, UWORD h);
void DisposeLUT8ImageContainer(struct NewLUT8Image *ni);
struct Region *RegionFromLUT8Image(int w, int h, struct NewLUT8Image *s);

#endif
