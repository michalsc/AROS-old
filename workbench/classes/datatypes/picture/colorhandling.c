/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <graphics/rpattr.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <intuition/gadgetclass.h>
#include <intuition/cghooks.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>
#include <cybergraphx/cybergraphics.h>

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>

#include "debug.h"
#include "pictureclass.h"
#include "colorhandling.h"

static UBYTE * AllocLineBuffer( long width, long height, int pixelbytes );
static void FreeSourceBM( struct Picture_Data *pd );
static void CopyColTable( struct Picture_Data *pd );
static BOOL RemapCM2CM( struct Picture_Data *pd );
static BOOL RemapTC2CM( struct Picture_Data *pd );
static int HistSort( const void *HistEntry1, const void *HistEntry2 );
static void RemapPens( struct Picture_Data *pd, int NumColors, int DestNumColors );

/**************************************************************************************************/
/*
static const UBYTE defcolmap[] =
{
    0,118,14,117,116,15,115,4,233,232,234,230,229,228,251,162,
    30,70,29,127,114,28,79,19,149,227,246,226,225,224,223,245,
    244,222,221,220,219,161,218,217,27,113,22,108,111,23,110,31,
    216,215,214,213,212,134,132,143,5,109,13,106,107,17,120,7,
    50,105,37,104,103,63,68,61,211,210,243,145,138,148,254,208,
    58,67,2,80,81,44,82,45,242,152,207,206,205,204,235,153,
    202,201,200,199,198,197,196,195,57,88,62,84,85,56,77,55,
    167,255,168,169,130,248,170,165,54,66,53,86,87,52,69,51,
    60,78,49,89,65,46,123,34,141,172,173,159,136,174,175,176,
    41,90,47,122,124,35,121,43,177,135,163,236,178,142,179,249,
    180,181,146,241,237,182,157,183,48,95,42,93,71,3,72,38,
    184,158,185,186,187,155,189,190,36,94,59,119,96,40,73,39,
    6,97,10,75,99,12,100,8,137,191,253,192,166,252,193,140,
    18,101,26,102,98,32,126,25,194,164,156,188,239,147,250,131,
    139,171,238,128,247,150,203,151,24,64,20,125,76,21,92,33,
    209,129,240,144,160,133,154,231,9,91,16,83,112,11,74,1
};
*/
static const UBYTE defcolmap[] =
{
    0,66,8,68,64,10,65,2,136,130,150,142,153,148,149,151,
    16,72,17,76,70,18,79,20,141,131,144,162,160,168,169,164,
    177,147,174,170,143,176,180,178,21,77,22,83,80,23,82,24,
    140,173,181,175,182,184,172,179,3,84,9,85,86,13,88,5,
    36,89,38,91,92,35,90,37,137,157,156,190,165,185,191,188,
    39,93,40,75,81,33,96,43,135,128,146,171,194,196,195,193,
    163,167,129,186,192,197,187,189,44,87,32,94,95,46,97,47,
    183,199,198,138,201,200,203,202,45,98,49,99,100,48,102,50,
    34,103,54,71,67,41,74,51,158,208,210,212,211,213,209,205,
    42,107,55,108,110,52,109,53,152,155,159,207,216,218,219,221,
    139,161,154,166,206,204,133,215,56,69,58,114,73,59,78,57,
    145,217,214,220,222,223,224,225,60,105,61,113,101,63,106,62,
    4,115,12,112,104,14,117,6,228,132,227,229,230,231,234,236,
    25,119,26,121,118,27,120,28,235,237,239,238,240,241,242,243,
    232,244,247,226,249,250,253,252,19,124,29,125,116,30,111,31,
    134,233,246,245,255,254,248,251,7,122,11,123,126,15,127,1
};

/**************************************************************************************************/

BOOL ConvertTC2TC( struct Picture_Data *pd )
{
    struct RastPort DestRP;
    long success; //BOOL success;

    D(bug("picture.datatype/ConvertTC2TC: TrueColor source/dest, PixelFormat %ld\n", pd->SrcPixelFormat));
    InitRastPort( &DestRP );
    DestRP.BitMap = pd->DestBM;
    success = WritePixelArray( pd->SrcBuffer,
				0,
				0,
				pd->SrcWidthBytes,
				&DestRP,
				0,
				0,
				pd->SrcWidth,
				pd->SrcHeight,
				pd->SrcPixelFormat);
    D(bug("picture.datatype/ConvertTC2TC: success %ld\n", success));
#ifdef __AROS__
    DeinitRastPort( &DestRP );
#endif
    return success;
}

BOOL ConvertCM2TC( struct Picture_Data *pd )
{
    struct RastPort DestRP;
    BOOL success;

    CopyColTable( pd );
    D(bug("picture.datatype/ConvertCM2TC: Colormapped source, TrueColor dest\n"));
    InitRastPort( &DestRP );
    DestRP.BitMap = pd->DestBM;
    success = WriteLUTPixelArray( pd->SrcBuffer,
				    0,
				    0,
				    pd->SrcWidthBytes,
				    &DestRP,
				    pd->ColTableXRGB,
				    0,
				    0,
				    pd->SrcWidth,
				    pd->SrcHeight,
				    CTABFMT_XRGB8 );
#ifdef __AROS__
    DeinitRastPort( &DestRP );
#endif
    return success;
}

BOOL ConvertCM2CM( struct Picture_Data *pd )
{
    struct RastPort DestRP;
    BOOL success;

    if( pd->Remap )
    {
	success = RemapCM2CM( pd );
    }
    else
    {
        D(bug("picture.datatype/ConvertCM2CM: Remapping disabled\n"));
	CopyColTable( pd ); // ?
	D(bug("picture.datatype/ConvertCM2CM: Colormapped source, Colormapped dest\n"));
	InitRastPort( &DestRP );
	DestRP.BitMap = pd->DestBM;
	WriteChunkyPixels( &DestRP,
			0,
			0,
			pd->SrcWidth-1,
			pd->SrcHeight-1,
			pd->SrcBuffer,
			pd->SrcWidthBytes );
#ifdef __AROS__
	DeinitRastPort( &DestRP );
#endif
	success = TRUE;
    }
    return success;
}

BOOL ConvertTC2CM( struct Picture_Data *pd )
{
    BOOL success;

    success = RemapTC2CM( pd );
    return success;
}

/**************************************************************************************************/

BOOL AllocSrcBuffer( struct Picture_Data *pd, long width, long height, ULONG pixelformat, int pixelbytes )
{
    pd->SrcWidthBytes = MOD16( width * pixelbytes);
    pd->SrcBuffer = AllocVec( pd->SrcWidthBytes * height, MEMF_ANY );
    if( !pd->SrcBuffer )
    {
	D(bug("picture.datatype/AllocSrcBuffer: Chunky source buffer allocation failed !\n"));
	return FALSE;
    }
    pd->SrcWidth = width;
    pd->SrcHeight = height;
    pd->SrcPixelFormat = pixelformat;
    pd->SrcPixelBytes = pixelbytes;
    D(bug("picture.datatype/AllocSrcBuffer: Chunky source buffer allocated\n"));
    return TRUE;
}

static UBYTE * AllocLineBuffer( long width, long height, int pixelbytes )
{
    long widthbytes;
    UBYTE *buffer;

    widthbytes = MOD16( width * pixelbytes);
    buffer = AllocVec( widthbytes * height, MEMF_ANY );
    if( !buffer )
    {
	D(bug("picture.datatype/AllocLineBuffer: Line buffer allocation failed !\n"));
	return FALSE;
    }
    D(bug("picture.datatype/AllocLineBuffer: Line buffer allocated\n"));
    return buffer;
}

BOOL AllocDestBM( struct Picture_Data *pd, long width, long height, int depth )
{
    pd->DestBM = AllocBitMap( width,
			      height,
			      depth,
//			      (BMF_INTERLEAVED | BMF_MINPLANES),
			      BMF_MINPLANES | BMF_CLEAR,
			      pd->UseFriendBM ? pd->DestScreen->RastPort.BitMap : NULL );
    if( !pd->DestBM )
    {
	D(bug("picture.datatype/AllocDestBM: DestBitmap allocation failed !\n"));
	return FALSE;;
    }
    D(bug("picture.datatype/AllocDestBM: Flags %ld Width %ld Height %ld Depth %ld\n", (long)GetBitMapAttr(pd->DestBM, BMA_FLAGS),
	(long)GetBitMapAttr(pd->DestBM, BMA_WIDTH), (long)GetBitMapAttr(pd->DestBM, BMA_HEIGHT), (long)GetBitMapAttr(pd->DestBM, BMA_DEPTH)));
    pd->DestWidth = width;
    pd->DestHeight = height;
    pd->DestDepth = depth;
    D(bug("picture.datatype/AllocDestBM: DestBitmap allocated\n"));
    return TRUE;
}

void FreeSource( struct Picture_Data *pd )
{
    if( pd->SrcBuffer )
    {
	D(bug("picture.datatype/FreeSource: Freeing SrcBuffer\n"));
	FreeVec( (void *) pd->SrcBuffer );
	pd->SrcBuffer = NULL;
    }
    FreeSourceBM( pd );
}

static void FreeSourceBM( struct Picture_Data *pd )
{
    if( pd->SrcBM && !pd->KeepSrcBM )
    {
	D(bug("picture.datatype/FreeSourceBM: Freeing SrcBitmap\n"));
	FreeBitMap( pd->SrcBM );
	pd->SrcBM = NULL;
    }
}

void FreeDest( struct Picture_Data *pd )
{
    int i;

    if( pd->NumAlloc )
    {
	D(bug("picture.datatype/FreeDest: Freeing %ld pens\n", (long)pd->NumAlloc));
	for(i=0; i<pd->NumAlloc; i++)
	{
	    ReleasePen( pd->DestScreen->ViewPort.ColorMap, pd->ColTable[i] );
	}
	pd->NumAlloc=0;
    }
    
    if( pd->DestBM )
    {
	D(bug("picture.datatype/FreeDest: Freeing DestBitmap\n"));
	FreeBitMap( pd->DestBM );
	pd->DestBM = NULL;
    }
}

/**************************************************************************************************/

static void CopyColTable( struct Picture_Data *pd )
{
    int i, j;
    ULONG colR, colG, colB;

    j = 0;
    for( i=0; i<256; i++ )
    {
	colR = pd->DestColRegs[j] = pd->SrcColRegs[j];
	j++;
	colG = pd->DestColRegs[j] = pd->SrcColRegs[j];
	j++;
	colB = pd->DestColRegs[j] = pd->SrcColRegs[j];
	j++;
	pd->ColTableXRGB[i] = ((colR>>8) & 0x00ff0000) | ((colG>>16) & 0x0000ff00) | ((colB>>24) & 0x000000ff);
    }
}

BOOL ConvertBitmap2Chunky( struct Picture_Data *pd )
{
    struct RastPort SrcRP;
    ULONG y, offset;
    ULONG width, height;
    UBYTE *buffer;

    if( !pd->SrcBM )
	return FALSE;
    D(bug("picture.datatype/Bitmap2Chunky: SrcBM; Flags %ld Width %ld Height %ld Depth %ld\n", (long)GetBitMapAttr(pd->SrcBM, BMA_FLAGS),
	(long)GetBitMapAttr(pd->SrcBM, BMA_WIDTH), (long)GetBitMapAttr(pd->SrcBM, BMA_HEIGHT), (long)GetBitMapAttr(pd->SrcBM, BMA_DEPTH)));
    /* Determine size and allocate Chunky source buffer */
    width = pd->bmhd.bmh_Width;
    height = pd->bmhd.bmh_Height;
    if( !AllocSrcBuffer( pd, width, height, PBPAFMT_LUT8, 1 ) )
	return FALSE;

    /* Copy the source Bitmap into the Chunky source buffer */
    InitRastPort( &SrcRP );
    SrcRP.BitMap = pd->SrcBM;
    offset = 0;
    buffer = pd->SrcBuffer;

#ifdef __AROS__
    for(y=0; y<height; y++)
    {
	/* AROS ReadPixelLine/Array8 does not need a temprp */
	ReadPixelLine8( &SrcRP, 0, y, width, &buffer[offset], NULL );
	offset += pd->SrcWidthBytes;
    }
    DeinitRastPort(&SrcRP);
#else
    D(bug("picture.datatype/Bitmap2Chunky: Slow ReadPixel() conversion\n"));
    {
	ULONG x;
	for(y=0; y<height; y++)
	{
	    for(x=0; x<width; x++)
	    {
		buffer[x + offset] = ReadPixel(&SrcRP, x, y);
	    }
	    offset += pd->SrcWidthBytes;
	}
    }
#endif

    D(bug("picture.datatype/Bitmap2Chunky: Conversion done\n"));
    FreeSourceBM( pd );
    return TRUE;
}

BOOL ConvertChunky2Bitmap( struct Picture_Data *pd )
{
    struct RastPort SrcRP;

    /* Allocate source Bitmap */
    pd->SrcBM = AllocBitMap( pd->SrcWidth,
			     pd->SrcHeight,
			     pd->DestDepth,
			     (BMF_INTERLEAVED | BMF_MINPLANES),
			     pd->DestScreen->RastPort.BitMap );
    if( !pd->SrcBM )
    {
	D(bug("picture.datatype/Chunky2Bitmap: Bitmap allocation failed !\n"));
	return FALSE;;
    }

    /* Copy the Chunky source buffer to the source Bitmap */
    InitRastPort( &SrcRP );
    SrcRP.BitMap = pd->SrcBM;
    WriteChunkyPixels( &SrcRP, 0, 0, pd->SrcWidth-1, pd->SrcHeight-1, pd->SrcBuffer, pd->SrcWidthBytes );
#ifdef __AROS__
    DeinitRastPort( &SrcRP );
#endif

    D(bug("picture.datatype/Chunky2Bitmap: Conversion done\n"));
    return TRUE;
}

/**************************************************************************************************/

static BOOL RemapTC2CM( struct Picture_Data *pd )
{
    ULONG width, height;
    unsigned int DestNumColors;
    int i, j, k;
    int srccnt, destcnt, index;
    ULONG *srccolregs, *destcolregs;
    ULONG Col7, Col3;

    D(bug("picture.datatype/RemapTC2CM: alloc dest and init pens\n"));
    width = pd->SrcWidth;
    height = pd->SrcHeight;

    pd->NumSparse = pd->NumColors = 256;
    DestNumColors = 1<<pd->DestDepth;
    if( pd->MaxDitherPens )
	DestNumColors = pd->MaxDitherPens;

    /*
     *  Create color tables, src is in "natural" order, dest is sorted by priority using a precalculated table;
     *  "natural" is bits: bbrr.rggg
     */
    Col7 = 0xFFFFFFFF/7;
    Col3 = 0xFFFFFFFF/3;
    srccolregs = pd->SrcColRegs;
    srccnt = 0;
    destcolregs = pd->DestColRegs;
    destcnt = 0;
    for( i=0; i<4; i++ ) /* blue */
    {
	for( j=0; j<8; j++ ) /* red */
	{
	    for( k=0; k<8; k++ ) /* green */
	    {
		index = 3 * defcolmap[destcnt++];
		destcolregs[index++] = srccolregs[srccnt++] = j*Col7;
		destcolregs[index++] = srccolregs[srccnt++] = k*Col7;
		destcolregs[index]   = srccolregs[srccnt++] = i*Col3;
	    }
	}
    }

    /*
     *  Allocate Pens and create sparse table for remapping
     */
    RemapPens( pd, 256, DestNumColors );

    /*
     *  Remap line-by-line truecolor source buffer to destination using sparse table
     */
    {
	struct RastPort DestRP;
	ULONG x, y;
	UBYTE *linebuf;
	UBYTE *thislinebuf;
	UBYTE *srcbuf = pd->SrcBuffer;
	ULONG srcwidthadd = pd->SrcWidthBytes - pd->SrcWidth * pd->SrcPixelBytes;
	UBYTE *sparsetable = pd->SparseTable;
	BOOL argb = pd->SrcPixelFormat==PBPAFMT_ARGB;

	linebuf = AllocLineBuffer( width, height, 1 );
	if( !linebuf )
	    return FALSE;
	InitRastPort( &DestRP );
	DestRP.BitMap = pd->DestBM;
	if( pd->DitherQuality )
	{
	    int rval, gval, bval;
	    long rerr, gerr, berr;
	    UBYTE destindex;
	    ULONG *colregs;
	    int feedback;
	    
	    D(bug("picture.datatype/RemapTC2CM: remapping buffer with dither of %d\n", (int)pd->DitherQuality));
	    feedback = 4 - pd->DitherQuality;
	    destcolregs = pd->DestColRegs;
	    for( y=0; y<height; y++ )
	    {
		thislinebuf = linebuf;
		rerr = gerr = berr = 0;
		for( x=0; x<width; x++ )
		{
		    if( argb )
			srcbuf++; // skip alpha
		    if( feedback )
		    {
			rerr >>= feedback;
			gerr >>= feedback;
			berr >>= feedback;
		    }
		    rerr += (*srcbuf++);
		    gerr += (*srcbuf++);
		    berr += (*srcbuf++);
		    rval = CLIP( rerr );
		    gval = CLIP( gerr );
		    bval = CLIP( berr );
		    index = (rval>>2 & 0x38) | (gval>>5 & 0x07) | (bval & 0xc0);
		    destindex = sparsetable[index];
		    *thislinebuf++ = destindex;
		    colregs = destcolregs + destindex*3;
		    rerr -= (*colregs++)>>24;
		    gerr -= (*colregs++)>>24;
		    berr -= (*colregs)>>24;
		}
		WriteChunkyPixels( &DestRP,
				    0,
				    y,
				    width-1,
				    y,
				    linebuf,
				    width );
		srcbuf += srcwidthadd;
	    }
	}
	else
	{
	    D(bug("picture.datatype/RemapTC2CM: remapping buffer without dithering\n"));
	    for( y=0; y<height; y++ )
	    {
		thislinebuf = linebuf;
		for( x=0; x<width; x++ )
		{
		    if( argb )
			srcbuf++; // skip alpha
		    index  = (*srcbuf++)>>2 & 0x38; // red
		    index |= (*srcbuf++)>>5 & 0x07; // green
		    index |= (*srcbuf++)    & 0xc0; // blue
		    
		    *thislinebuf++ = sparsetable[index];
		}
		WriteChunkyPixels( &DestRP,
				    0,
				    y,
				    width-1,
				    y,
				    linebuf,
				    width );
		srcbuf += srcwidthadd;
	    }
	}
    #ifdef __AROS__
	DeinitRastPort( &DestRP );
    #endif
	FreeVec( (void *) linebuf );
    }
    D(bug("picture.datatype/RemapTC2CM: done\n"));
    return TRUE;
}

static BOOL RemapCM2CM( struct Picture_Data *pd )
{
    struct HistEntry TheHist[256];
    ULONG width, height;
    int DestNumColors, NumColors;
    int i, j, index;

    D(bug("picture.datatype/RemapCM2CM: alloc dest and init pens\n"));
    width = pd->SrcWidth;
    height = pd->SrcHeight;

    NumColors = pd->NumColors;
    DestNumColors = 1<<pd->DestDepth;
    if( pd->MaxDitherPens )
	DestNumColors = pd->MaxDitherPens;
    if( NumColors < DestNumColors )
	DestNumColors = NumColors;

    memset( pd->DestColRegs, 0xFF, 768*sizeof(ULONG) );	/* initialize GRegs table */
    memset( pd->SparseTable, 0x0, 256 );		/* initialize Sparse table */
    pd->NumSparse = NumColors;

    D(bug("picture.datatype/RemapCM2CM: sorting pens\n"));
    /*
     *  Farben im Histogramm ausfuellen
     */
    index = 0;
    for(i=0; i<NumColors; i++)
    {
	TheHist[i].Count = 0;
	TheHist[i].Red   = pd->SrcColRegs[index++];
	TheHist[i].Green = pd->SrcColRegs[index++];
	TheHist[i].Blue  = pd->SrcColRegs[index++];
    }

    /*
     *  Farbanzahl im Histogramm ermitteln
     */
    { 
	UBYTE *sb = pd->SrcBuffer;
    
	for( i=0; i<height; i++ )
	{
	    for( j=0; j<width; j++ )
	    {
		TheHist[sb[j]].Count++;
	    }
	    sb += pd->SrcWidthBytes;
	}
    }
    
    /*
     *  Duplikate im Histogramm ausmerzen
     */
    for( i=0; i<NumColors-1; i++ )
    {
	for( j=i+1; j<NumColors; j++ )
	{
	    if( (TheHist[j].Red   == TheHist[i].Red  ) &&
		(TheHist[j].Green == TheHist[i].Green) &&
		(TheHist[j].Blue  == TheHist[i].Blue ) )
	    {
		TheHist[i].Count += TheHist[j].Count;
		TheHist[j].Count = 0;
	    }
	}
    }

    /*
     *  Histogramm nach Haeufigkeit sortieren
     */
    qsort( (void *) TheHist, NumColors, sizeof(struct HistEntry), HistSort );

    /*
     *  Es werden die DestNumColors meistvorhandenen Farben benutzt
     */
    index = 0;
    for( i=0; i<DestNumColors; i++ )
    {
	pd->DestColRegs[index++] = TheHist[i].Red;
	pd->DestColRegs[index++] = TheHist[i].Green;
	pd->DestColRegs[index++] = TheHist[i].Blue;
    }

    /*
     *  Allocate Pens and create sparse table for remapping
     */
    RemapPens( pd, NumColors, DestNumColors );
    
    /*
     *  ChunkyBuffer remappen
     */
    D(bug("picture.datatype/RemapCM2CM: remap chunky buffer\n"));
    { 
	struct RastPort DestRP;
	UBYTE *srcbuf = pd->SrcBuffer;
	ULONG srcwidthadd = pd->SrcWidthBytes - pd->SrcWidth * pd->SrcPixelBytes;
	UBYTE *sparsetable = pd->SparseTable;
	UBYTE *linebuf;
	UBYTE *thislinebuf;
	long x, y;
 
	linebuf = AllocLineBuffer( width, height, 1 );
	if( !linebuf )
	    return FALSE;
	InitRastPort( &DestRP );
	DestRP.BitMap = pd->DestBM;
	for( y=0; y<height; y++ )
	{
	    thislinebuf = linebuf;
	    for( x=0; x<width; x++ )
	    {
		*thislinebuf++ = sparsetable[*srcbuf++];
	    }
	    WriteChunkyPixels( &DestRP,
				0,
				y,
				width-1,
				y,
				linebuf,
				width );
	    srcbuf += srcwidthadd;
	}
#ifdef __AROS__
	DeinitRastPort( &DestRP );
#endif
	FreeVec( (void *) linebuf );
    }

    D(bug("picture.datatype/RemapCM2CM: done\n"));
    return TRUE;
}

static int HistSort( const void *HistEntry1, const void *HistEntry2 )
{
    struct HistEntry *HE1, *HE2;
    
    HE1 = (struct HistEntry *) HistEntry1;
    HE2 = (struct HistEntry *) HistEntry2;
    
    return ((int) (HE2->Count - HE1->Count));
}

static void RemapPens( struct Picture_Data *pd, int NumColors, int DestNumColors )
{
    int i, j, index;
    int pen;

    /*
     *  Pens fuer DestColRegs (GRegs) obtainen
     */
    index = 0;
    for( i=0; i<DestNumColors; i++ )
    {
	pd->ColTable[i] = ObtainBestPen( pd->DestScreen->ViewPort.ColorMap,
				         pd->DestColRegs[index+0],
					 pd->DestColRegs[index+1],
					 pd->DestColRegs[index+2],
				         OBP_Precision, pd->Precision,
				         OBP_FailIfBad, FALSE,
				         TAG_DONE);
	// D(bug("picture.datatype/RemapPens: %d Pen %d: R %d G %d B %d\n",
	//     (int)i, (int)pd->ColTable[i], (int)(pd->DestColRegs[index]>>24), (int)(pd->DestColRegs[index+1]>>24), (int)(pd->DestColRegs[index+2]>>24)));
	index += 3;
	pd->NumAlloc++;
    }
    D(bug("picture.datatype/RemapPens: NumColors: %ld DestNumColors: %ld NumAlloc: %ld Depth: %ld\n",
        (long)pd->NumColors, (long)DestNumColors, (long)pd->NumAlloc, (long)pd->DestDepth));
 
    /*
     *  Die wirklichen Farben der Pens holen
     */
    for( i=0; i<DestNumColors; i++ )
    {
	pen = pd->ColTable[i];
	GetRGB32( pd->DestScreen->ViewPort.ColorMap, pen, 1, pd->DestColRegs+pen*3 );
	// D(bug("picture.datatype/RemapPens: %d Pen %d: R %d G %d B %d\n",
	//     i, pen, (int)(pd->DestColRegs[pen*3]>>24), (int)(pd->DestColRegs[pen*3+1]>>24), (int)(pd->DestColRegs[pen*3+2]>>24)));
    }
 
    /*
     *  SparseTable nach der "Geringster Abstand" Methode bestimmen
     */
    index = 0;
    for( i=0; i<NumColors; i++ )
    {
	ULONG Diff, LastDiff;
	int CRed, GRed, CGreen, GGreen, CBlue, GBlue;
    
	LastDiff=0xFFFFFFFF;
    
	CRed   = pd->SrcColRegs[index++]>>17;
	CGreen = pd->SrcColRegs[index++]>>17;
	CBlue  = pd->SrcColRegs[index++]>>17;
    
	for( j=0; j<DestNumColors; j++ )
	{
	    pen = pd->ColTable[j] * 3;
	    GRed   = pd->DestColRegs[pen++]>>17;
	    GGreen = pd->DestColRegs[pen++]>>17;
	    GBlue  = pd->DestColRegs[pen]>>17;
       
	    Diff = abs(CRed   - GRed  ) +
		   abs(CGreen - GGreen) +
		   abs(CBlue  - GBlue );
       
	    if( Diff <= LastDiff )
	    {
		pd->SparseTable[i] = pd->ColTable[j];
		LastDiff = Diff;
	    }
       
	    if(LastDiff==0)
	    {
		break;
	    }
	}
    }
#if 1
    {
	int sp;
	D(bug("picture.datatype/RemapPens: sparse table: source col -> dest pen\n"));
	for( i=0; i<NumColors; i++ )
	{
	    sp = pd->SparseTable[i];
	    D(bug("picture.datatype/RemapPens: %d (R %d G %d B %d) -> %d (R %d G %d B %d)\n",
		i, pd->SrcColRegs[i*3]>>24, pd->SrcColRegs[i*3+1]>>24, pd->SrcColRegs[i*3+2]>>24,
		sp, pd->DestColRegs[sp*3]>>24, pd->DestColRegs[sp*3+1]>>24, pd->DestColRegs[sp*3+2]>>24));
	}
    }
#endif
}

/**************************************************************************************************/

#if 0
unsigned int *MakeARGB(unsigned long *ColRegs, unsigned int Count)
{
 unsigned int *ARGB;
 register unsigned int i;

 ARGB=NULL;

 if(!(ColRegs && Count))
 {
  return(NULL);
 }

 ARGB=AllocVec(Count*sizeof(unsigned int), MEMF_ANY | MEMF_CLEAR);
 if(!ARGB)
 {
  return(NULL);
 }

 for(i=0; i<Count; i++)
 {
  ARGB[i]  = ((*(ColRegs++)) & 0xFF000000) >>  8;
  ARGB[i] |= ((*(ColRegs++)) & 0xFF000000) >> 16;
  ARGB[i] |= ((*(ColRegs++)) & 0xFF000000) >> 24;
 }

 return(ARGB);
}

unsigned int CountColors(unsigned int *ARGB, unsigned int Count)
{
 unsigned int NumColors;
 register unsigned int i, j;

 NumColors=0;

 if(!(ARGB && Count))
 {
  return(0);
 }

 for(i=0; i<Count; i++)
 {
  /*
   *  We assume that it is a new color.
   */

  NumColors++;

  for(j=0; j<i; j++)
  {
   if(ARGB[j]==ARGB[i])
   {
    /*
     *  Oops, it isn't a new color.
     */

    NumColors--;

    break;
   }
  }
 }

 return(NumColors);
}
#endif
