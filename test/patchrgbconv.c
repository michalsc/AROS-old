#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <dos/rdargs.h>
#include <hidd/graphics.h>
#include <aros/macros.h>
#include <aros/debug.h>

#include <stdio.h>
#include <stdlib.h>

static BOOL noisy;

#include "patchrgbconv_macros.h"
#include "patchrgbconv_rgbswap.h"
#include "patchrgbconv_argb32.h"
#include "patchrgbconv_bgra32.h"
#include "patchrgbconv_rgba32.h"
#include "patchrgbconv_abgr32.h"
#include "patchrgbconv_rgb24.h"
#include "patchrgbconv_bgr24.h"
#include "patchrgbconv_rgb15.h"
#include "patchrgbconv_bgr15.h"
#include "patchrgbconv_rgb16.h"
#include "patchrgbconv_bgr16.h"
#include "patchrgbconv_rgb16oe.h"
#include "patchrgbconv_bgr16oe.h"
#include "patchrgbconv_rgb15oe.h"
#include "patchrgbconv_bgr15oe.h"

#include "patchrgbconv_verify.h"

#define VERIFY_SIZE 100

#define ARG_TEMPLATE "VERIFY=V/S,BENCH=B/N/K,NOISY=N/S"
#define ARG_VERIFY 0
#define ARG_BENCH 1
#define ARG_NOISY 2
#define ARG_NUM 3

static IPTR args[ARG_NUM];

static ULONG dstbuf_orig[VERIFY_SIZE * 4 + 10];
static ULONG dstbuf_new[VERIFY_SIZE * 4 + 10];
static UBYTE *benchmem;

static void dumpmem(UBYTE *buf, ULONG num, ULONG size, STRPTR info)
{
    int i, b;
    
    bug("  %s", info);
    
    for(i = 0; i < num; i++)
    {
    	if ((i % (32 / size)) == 0)
	{
	    bug("\n   ");
	}
	
    	for(b = 0; b < size; b++)
	{
	#if AROS_BIG_ENDAIN
	    bug("%02x", buf[b]);
	#else
	    bug("%02x", buf[size - 1 -b]);
    	#endif
	}
	buf += size;
	bug(" ");
    }
    bug("\n\n");
}

static void installfunc(ULONG srcfmt, ULONG dstfmt, ULONG srcbits, ULONG dstbits,
    	    	    	HIDDT_RGBConversionFunction f, BOOL verify, ULONG bench,
			APTR testpixels, ULONG numtestpixels, char *srcfmt_string,
			char *dstfmt_string)
{
    ULONG srcbytes, dstbytes;
    ULONG sec1, sec2, micro1, micro2, time1 = 0, time2;
    
    srcbytes = (srcbits + 7) / 8;
    dstbytes = (dstbits + 7) / 8;
    
    if (verify)
    {
    	ConvertPixelsA(testpixels, 0, srcfmt,
	    	       dstbuf_orig, numtestpixels * dstbytes, dstfmt,
		       numtestpixels,
		       VERIFY_SIZE / numtestpixels,
		       0);
	 
    }
    
    if (bench && benchmem)
    {
    	CurrentTime(&sec1, &micro1);
	ConvertPixelsA(benchmem, 0, srcfmt,
	    	       benchmem, 0, dstfmt,
		       bench, 1,
		       0);
		       
    	CurrentTime(&sec2, &micro2);
	
	time1 = (sec2 - sec1) * 1000000 + (((LONG)micro2) - ((LONG)micro1));
    }
    
    SetRGBConversionFunctionA(srcfmt, dstfmt, f, 0);

    if (verify)
    {
    	ConvertPixelsA(testpixels, 0, srcfmt,
	    	       dstbuf_new, numtestpixels * dstbytes, dstfmt,
		       numtestpixels,
		       VERIFY_SIZE / numtestpixels,
		       0);
	
	if (memcmp(dstbuf_orig, dstbuf_new, (VERIFY_SIZE / numtestpixels) * (numtestpixels * dstbytes)) != 0)
	{
	    int numtest = (VERIFY_SIZE / numtestpixels) * numtestpixels;
	    
	    bug("  Verify failed!\n");
	    dumpmem((UBYTE *)testpixels, numtestpixels, srcbytes, (STRPTR)"SRC:");
	    dumpmem((UBYTE *)dstbuf_orig, numtest, dstbytes, (STRPTR)"OLD:");
	    dumpmem((UBYTE *)dstbuf_new, numtest, dstbytes, (STRPTR)"NEW:");
	    
	}
	
    }

    if (bench && benchmem)
    {
    	static char sbuf[256];
	
    	CurrentTime(&sec1, &micro1);
	ConvertPixelsA(benchmem, 0, srcfmt,
	    	       benchmem, 0, dstfmt,
		       bench, 1,
		       0);
		       
    	CurrentTime(&sec2, &micro2);
	
	time2 = (sec2 - sec1) * 1000000 + (((LONG)micro2) - ((LONG)micro1));
	
	sprintf(sbuf, " Benchmark %s to %s: before %ld (%f) after %ld (%f) (%ld %%)\n", 
	    	      srcfmt_string, dstfmt_string,
	    	      time1, time1 / 1000000.0, 
	    	      time2, time2 / 1000000.0,
		      (time2 ? time1 * 100 / time2 : 0));
	bug("%s", sbuf);
    }

}

#define PATCHFUNC(a,b) \
    installfunc(FMT_ ## a, FMT_ ## b, a ## _ ## BITS, b ## _ ## BITS, convert_ ## a ## _ ## b, \
    	    	verify, bench, testpixels_ ## a, NUMTESTPIXELS_ ## a, # a, # b);
        
#define UNPATCHFUNC(a,b) SetRGBConversionFunctionA(FMT_ ## a, FMT_ ## b, 0, 0);

int main(int argc, char **argv)
{
    struct RDArgs *myargs;
    BOOL    	   verify = FALSE;
    ULONG   	   bench = 0;
    
    if ((myargs = ReadArgs((STRPTR)ARG_TEMPLATE, args, NULL)))
    {
    	if (args[ARG_VERIFY]) verify = TRUE;
	if (args[ARG_BENCH]) bench = *(IPTR *)args[ARG_BENCH];
	if (args[ARG_NOISY]) noisy = TRUE;
	
    	FreeArgs(myargs);
    }
    else
    {
    	PrintFault(IoErr(), (STRPTR)argv[0]);
	
	return RETURN_FAIL;
    }
        
    if (bench)
    {
    	benchmem = malloc(bench * 4);
	if (!benchmem)
	{
	    PrintFault(ERROR_NO_FREE_STORE, (STRPTR)argv[0]);	
    	
	    return RETURN_FAIL;
	}
    }
    
    /* BGRA32 to #? */
    PATCHFUNC(BGRA32,RGB16)
    PATCHFUNC(BGRA32,BGR16)
    PATCHFUNC(BGRA32,RGB15)
    PATCHFUNC(BGRA32,BGR15)
    PATCHFUNC(BGRA32,ARGB32)
    PATCHFUNC(BGRA32,RGBA32)
    PATCHFUNC(BGRA32,ABGR32)
    PATCHFUNC(BGRA32,RGB24)
    PATCHFUNC(BGRA32,BGR24)
    PATCHFUNC(BGRA32,RGB16OE)
    PATCHFUNC(BGRA32,BGR16OE)
    PATCHFUNC(BGRA32,RGB15OE)
    PATCHFUNC(BGRA32,BGR15OE)
    
    /* ARGB32 to #? */
    PATCHFUNC(ARGB32,RGB16)
    PATCHFUNC(ARGB32,BGR16)
    PATCHFUNC(ARGB32,RGB15)
    PATCHFUNC(ARGB32,BGR15)
    PATCHFUNC(ARGB32,BGRA32)
    PATCHFUNC(ARGB32,RGBA32)
    PATCHFUNC(ARGB32,ABGR32)
    PATCHFUNC(ARGB32,RGB24)
    PATCHFUNC(ARGB32,BGR24)
    PATCHFUNC(ARGB32,RGB16OE)
    PATCHFUNC(ARGB32,BGR16OE)
    PATCHFUNC(ARGB32,RGB15OE)
    PATCHFUNC(ARGB32,BGR15OE)
    
    /* RGBA32 to #? */
    PATCHFUNC(RGBA32,RGB16)
    PATCHFUNC(RGBA32,BGR16)
    PATCHFUNC(RGBA32,RGB15)
    PATCHFUNC(RGBA32,BGR15)
    PATCHFUNC(RGBA32,BGRA32)
    PATCHFUNC(RGBA32,ARGB32)
    PATCHFUNC(RGBA32,ABGR32)
    PATCHFUNC(RGBA32,RGB24)
    PATCHFUNC(RGBA32,BGR24)
    PATCHFUNC(RGBA32,RGB16OE)
    PATCHFUNC(RGBA32,BGR16OE)
    PATCHFUNC(RGBA32,RGB15OE)
    PATCHFUNC(RGBA32,BGR15OE)
    
    /* ABGR32 to #? */
    PATCHFUNC(ABGR32,RGB16)
    PATCHFUNC(ABGR32,BGR16)
    PATCHFUNC(ABGR32,RGB15)
    PATCHFUNC(ABGR32,BGR15)
    PATCHFUNC(ABGR32,BGRA32)
    PATCHFUNC(ABGR32,ARGB32)
    PATCHFUNC(ABGR32,RGBA32)
    PATCHFUNC(ABGR32,RGB24)
    PATCHFUNC(ABGR32,BGR24)
    PATCHFUNC(ABGR32,RGB16OE)
    PATCHFUNC(ABGR32,BGR16OE)
    PATCHFUNC(ABGR32,RGB15OE)
    PATCHFUNC(ABGR32,BGR15OE)

    /* RGB24 to #? */
    PATCHFUNC(RGB24,RGB16)
    PATCHFUNC(RGB24,BGR16)
    PATCHFUNC(RGB24,RGB15)
    PATCHFUNC(RGB24,BGR15)
    PATCHFUNC(RGB24,ARGB32)
    PATCHFUNC(RGB24,BGRA32)
    PATCHFUNC(RGB24,RGBA32)
    PATCHFUNC(RGB24,ABGR32)
    PATCHFUNC(RGB24,BGR24)
    PATCHFUNC(RGB24,RGB16OE)
    PATCHFUNC(RGB24,BGR16OE)
    PATCHFUNC(RGB24,RGB15OE)
    PATCHFUNC(RGB24,BGR15OE)
    
    /* BGR24 to #? */
    PATCHFUNC(BGR24,RGB16)
    PATCHFUNC(BGR24,BGR16)
    PATCHFUNC(BGR24,RGB15)
    PATCHFUNC(BGR24,BGR15)
    PATCHFUNC(BGR24,ARGB32)
    PATCHFUNC(BGR24,BGRA32)
    PATCHFUNC(BGR24,RGBA32)
    PATCHFUNC(BGR24,ABGR32)
    PATCHFUNC(BGR24,RGB24)
    PATCHFUNC(BGR24,RGB16OE)
    PATCHFUNC(BGR24,BGR16OE)
    PATCHFUNC(BGR24,RGB15OE)
    PATCHFUNC(BGR24,BGR15OE)

    /* RGB15 to #? */
    PATCHFUNC(RGB15,RGB16)
    PATCHFUNC(RGB15,BGR16)
    PATCHFUNC(RGB15,BGR15)
    PATCHFUNC(RGB15,ARGB32)
    PATCHFUNC(RGB15,BGRA32)
    PATCHFUNC(RGB15,RGBA32)
    PATCHFUNC(RGB15,ABGR32)
    PATCHFUNC(RGB15,RGB24)
    PATCHFUNC(RGB15,BGR24)
    PATCHFUNC(RGB15,RGB16OE)
    PATCHFUNC(RGB15,RGB15OE)
    PATCHFUNC(RGB15,BGR16OE)
    PATCHFUNC(RGB15,BGR15OE)
    
    /* BGR15 to #? */
    PATCHFUNC(BGR15,RGB16)
    PATCHFUNC(BGR15,BGR16)
    PATCHFUNC(BGR15,RGB15)
    PATCHFUNC(BGR15,ARGB32)
    PATCHFUNC(BGR15,BGRA32)
    PATCHFUNC(BGR15,RGBA32)
    PATCHFUNC(BGR15,ABGR32)
    PATCHFUNC(BGR15,RGB24)
    PATCHFUNC(BGR15,BGR24)
    PATCHFUNC(BGR15,RGB16OE)
    PATCHFUNC(BGR15,RGB15OE)
    PATCHFUNC(BGR15,BGR16OE)
    PATCHFUNC(BGR15,BGR15OE)

    /* RGB16 to #? */
    PATCHFUNC(RGB16,RGB15)
    PATCHFUNC(RGB16,BGR16)
    PATCHFUNC(RGB16,BGR15)
    PATCHFUNC(RGB16,ARGB32)
    PATCHFUNC(RGB16,BGRA32)
    PATCHFUNC(RGB16,RGBA32)
    PATCHFUNC(RGB16,ABGR32)
    PATCHFUNC(RGB16,RGB24)
    PATCHFUNC(RGB16,BGR24)
    PATCHFUNC(RGB16,RGB16OE)
    PATCHFUNC(RGB16,RGB15OE)
    PATCHFUNC(RGB16,BGR16OE)
    PATCHFUNC(RGB16,BGR15OE)

    /* BGR16 to #? */
    PATCHFUNC(BGR16,RGB15)
    PATCHFUNC(BGR16,RGB16)
    PATCHFUNC(BGR16,BGR15)
    PATCHFUNC(BGR16,ARGB32)
    PATCHFUNC(BGR16,BGRA32)
    PATCHFUNC(BGR16,RGBA32)
    PATCHFUNC(BGR16,ABGR32)
    PATCHFUNC(BGR16,RGB24)
    PATCHFUNC(BGR16,BGR24)
    PATCHFUNC(BGR16,RGB16OE)
    PATCHFUNC(BGR16,RGB15OE)
    PATCHFUNC(BGR16,BGR16OE)
    PATCHFUNC(BGR16,BGR15OE)

    /* RGB16OE to #? */
    PATCHFUNC(RGB16OE,RGB16)
    PATCHFUNC(RGB16OE,RGB15)
    PATCHFUNC(RGB16OE,BGR16)
    PATCHFUNC(RGB16OE,BGR15)
    PATCHFUNC(RGB16OE,ARGB32)
    PATCHFUNC(RGB16OE,BGRA32)
    PATCHFUNC(RGB16OE,RGBA32)
    PATCHFUNC(RGB16OE,ABGR32)
    PATCHFUNC(RGB16OE,RGB24)
    PATCHFUNC(RGB16OE,BGR24)
    PATCHFUNC(RGB16OE,RGB15OE)
    PATCHFUNC(RGB16OE,BGR16OE)
    PATCHFUNC(RGB16OE,BGR15OE)

    /* BGR16OE to #? */
    PATCHFUNC(BGR16OE,RGB16)
    PATCHFUNC(BGR16OE,RGB15)
    PATCHFUNC(BGR16OE,BGR16)
    PATCHFUNC(BGR16OE,BGR15)
    PATCHFUNC(BGR16OE,ARGB32)
    PATCHFUNC(BGR16OE,BGRA32)
    PATCHFUNC(BGR16OE,RGBA32)
    PATCHFUNC(BGR16OE,ABGR32)
    PATCHFUNC(BGR16OE,RGB24)
    PATCHFUNC(BGR16OE,BGR24)
    PATCHFUNC(BGR16OE,RGB15OE)
    PATCHFUNC(BGR16OE,RGB16OE)
    PATCHFUNC(BGR16OE,BGR15OE)

    /* RGB15OE to #? */
    PATCHFUNC(RGB15OE,RGB16)
    PATCHFUNC(RGB15OE,RGB15)
    PATCHFUNC(RGB15OE,BGR16)
    PATCHFUNC(RGB15OE,BGR15)
    PATCHFUNC(RGB15OE,ARGB32)
    PATCHFUNC(RGB15OE,BGRA32)
    PATCHFUNC(RGB15OE,RGBA32)
    PATCHFUNC(RGB15OE,ABGR32)
    PATCHFUNC(RGB15OE,RGB24)
    PATCHFUNC(RGB15OE,BGR24)
    PATCHFUNC(RGB15OE,RGB16OE)
    PATCHFUNC(RGB15OE,BGR16OE)
    PATCHFUNC(RGB15OE,BGR15OE)
    
    /* BGR15OE to #? */
    PATCHFUNC(BGR15OE,RGB16)
    PATCHFUNC(BGR15OE,RGB15)
    PATCHFUNC(BGR15OE,BGR16)
    PATCHFUNC(BGR15OE,BGR15)
    PATCHFUNC(BGR15OE,ARGB32)
    PATCHFUNC(BGR15OE,BGRA32)
    PATCHFUNC(BGR15OE,RGBA32)
    PATCHFUNC(BGR15OE,ABGR32)
    PATCHFUNC(BGR15OE,RGB24)
    PATCHFUNC(BGR15OE,BGR24)
    PATCHFUNC(BGR15OE,RGB16OE)
    PATCHFUNC(BGR15OE,BGR16OE)
    PATCHFUNC(BGR15OE,RGB15OE)
    
    Wait(SIGBREAKF_CTRL_C);
    
    UNPATCHFUNC(BGRA32,RGB16)
    UNPATCHFUNC(BGRA32,BGR16)
    UNPATCHFUNC(BGRA32,RGB15)
    UNPATCHFUNC(BGRA32,BGR15)
    UNPATCHFUNC(BGRA32,ARGB32)
    UNPATCHFUNC(BGRA32,RGBA32)
    UNPATCHFUNC(BGRA32,ABGR32)
    UNPATCHFUNC(BGRA32,RGB24)
    UNPATCHFUNC(BGRA32,BGR24)
    UNPATCHFUNC(BGRA32,RGB16OE)
    UNPATCHFUNC(BGRA32,BGR16OE)
    UNPATCHFUNC(BGRA32,RGB15OE)
    UNPATCHFUNC(BGRA32,BGR15OE)

    UNPATCHFUNC(ARGB32,RGB16)
    UNPATCHFUNC(ARGB32,BGR16)
    UNPATCHFUNC(ARGB32,RGB15)
    UNPATCHFUNC(ARGB32,BGR15)
    UNPATCHFUNC(ARGB32,BGRA32)
    UNPATCHFUNC(ARGB32,RGBA32)
    UNPATCHFUNC(ARGB32,ABGR32)
    UNPATCHFUNC(ARGB32,RGB24)
    UNPATCHFUNC(ARGB32,BGR24)
    UNPATCHFUNC(ARGB32,RGB16OE)
    UNPATCHFUNC(ARGB32,BGR16OE)
    UNPATCHFUNC(ARGB32,RGB15OE)
    UNPATCHFUNC(ARGB32,BGR15OE)

    UNPATCHFUNC(RGBA32,RGB16)
    UNPATCHFUNC(RGBA32,BGR16)
    UNPATCHFUNC(RGBA32,RGB15)
    UNPATCHFUNC(RGBA32,BGR15)
    UNPATCHFUNC(RGBA32,BGRA32)
    UNPATCHFUNC(RGBA32,ARGB32)
    UNPATCHFUNC(RGBA32,ABGR32)
    UNPATCHFUNC(RGBA32,RGB24)
    UNPATCHFUNC(RGBA32,BGR24)
    UNPATCHFUNC(RGBA32,RGB16OE)
    UNPATCHFUNC(RGBA32,BGR16OE)
    UNPATCHFUNC(RGBA32,RGB15OE)
    UNPATCHFUNC(RGBA32,BGR15OE)

    UNPATCHFUNC(ABGR32,RGB16)
    UNPATCHFUNC(ABGR32,BGR16)
    UNPATCHFUNC(ABGR32,RGB15)
    UNPATCHFUNC(ABGR32,BGR15)
    UNPATCHFUNC(ABGR32,BGRA32)
    UNPATCHFUNC(ABGR32,ARGB32)
    UNPATCHFUNC(ABGR32,RGBA32)
    UNPATCHFUNC(ABGR32,RGB24)
    UNPATCHFUNC(ABGR32,BGR24)
    UNPATCHFUNC(ABGR32,RGB16OE)
    UNPATCHFUNC(ABGR32,BGR16OE)
    UNPATCHFUNC(ABGR32,RGB15OE)
    UNPATCHFUNC(ABGR32,BGR15OE)

    UNPATCHFUNC(RGB24,RGB16)
    UNPATCHFUNC(RGB24,BGR16)
    UNPATCHFUNC(RGB24,RGB15)
    UNPATCHFUNC(RGB24,BGR15)
    UNPATCHFUNC(RGB24,ARGB32)
    UNPATCHFUNC(RGB24,BGRA32)
    UNPATCHFUNC(RGB24,RGBA32)
    UNPATCHFUNC(RGB24,ABGR32)
    UNPATCHFUNC(RGB24,BGR24)
    UNPATCHFUNC(RGB24,RGB16OE)
    UNPATCHFUNC(RGB24,BGR16OE)
    UNPATCHFUNC(RGB24,RGB15OE)
    UNPATCHFUNC(RGB24,BGR15OE)

    UNPATCHFUNC(BGR24,RGB16)
    UNPATCHFUNC(BGR24,BGR16)
    UNPATCHFUNC(BGR24,RGB15)
    UNPATCHFUNC(BGR24,BGR15)
    UNPATCHFUNC(BGR24,ARGB32)
    UNPATCHFUNC(BGR24,BGRA32)
    UNPATCHFUNC(BGR24,RGBA32)
    UNPATCHFUNC(BGR24,ABGR32)
    UNPATCHFUNC(BGR24,RGB24)
    UNPATCHFUNC(BGR24,RGB16OE)
    UNPATCHFUNC(BGR24,BGR16OE)
    UNPATCHFUNC(BGR24,RGB15OE)
    UNPATCHFUNC(BGR24,BGR15OE)

    UNPATCHFUNC(RGB15,RGB16)
    UNPATCHFUNC(RGB15,BGR16)
    UNPATCHFUNC(RGB15,BGR15)
    UNPATCHFUNC(RGB15,ARGB32)
    UNPATCHFUNC(RGB15,BGRA32)
    UNPATCHFUNC(RGB15,RGBA32)
    UNPATCHFUNC(RGB15,ABGR32)
    UNPATCHFUNC(RGB15,RGB24)
    UNPATCHFUNC(RGB15,BGR24)
    UNPATCHFUNC(RGB15,RGB16OE)
    UNPATCHFUNC(RGB15,RGB15OE)
    UNPATCHFUNC(RGB15,BGR16OE)
    UNPATCHFUNC(RGB15,BGR15OE)

    UNPATCHFUNC(BGR15,RGB16)
    UNPATCHFUNC(BGR15,BGR16)
    UNPATCHFUNC(BGR15,RGB15)
    UNPATCHFUNC(BGR15,ARGB32)
    UNPATCHFUNC(BGR15,BGRA32)
    UNPATCHFUNC(BGR15,RGBA32)
    UNPATCHFUNC(BGR15,ABGR32)
    UNPATCHFUNC(BGR15,RGB24)
    UNPATCHFUNC(BGR15,BGR24)
    UNPATCHFUNC(BGR15,RGB16OE)
    UNPATCHFUNC(BGR15,RGB15OE)
    UNPATCHFUNC(BGR15,BGR16OE)
    UNPATCHFUNC(BGR15,BGR15OE)

    UNPATCHFUNC(RGB16,RGB15)
    UNPATCHFUNC(RGB16,BGR16)
    UNPATCHFUNC(RGB16,BGR15)
    UNPATCHFUNC(RGB16,ARGB32)
    UNPATCHFUNC(RGB16,BGRA32)
    UNPATCHFUNC(RGB16,RGBA32)
    UNPATCHFUNC(RGB16,ABGR32)
    UNPATCHFUNC(RGB16,RGB24)
    UNPATCHFUNC(RGB16,BGR24)
    UNPATCHFUNC(RGB16,RGB16OE)
    UNPATCHFUNC(RGB16,RGB15OE)
    UNPATCHFUNC(RGB16,BGR16OE)
    UNPATCHFUNC(RGB16,BGR15OE)

    UNPATCHFUNC(BGR16,RGB15)
    UNPATCHFUNC(BGR16,RGB16)
    UNPATCHFUNC(BGR16,BGR15)
    UNPATCHFUNC(BGR16,ARGB32)
    UNPATCHFUNC(BGR16,BGRA32)
    UNPATCHFUNC(BGR16,RGBA32)
    UNPATCHFUNC(BGR16,ABGR32)
    UNPATCHFUNC(BGR16,RGB24)
    UNPATCHFUNC(BGR16,BGR24)
    UNPATCHFUNC(BGR16,RGB16OE)
    UNPATCHFUNC(BGR16,RGB15OE)
    UNPATCHFUNC(BGR16,BGR16OE)
    UNPATCHFUNC(BGR16,BGR15OE)

    UNPATCHFUNC(RGB16OE,RGB16)
    UNPATCHFUNC(RGB16OE,RGB15)
    UNPATCHFUNC(RGB16OE,BGR16)
    UNPATCHFUNC(RGB16OE,BGR15)
    UNPATCHFUNC(RGB16OE,ARGB32)
    UNPATCHFUNC(RGB16OE,BGRA32)
    UNPATCHFUNC(RGB16OE,RGBA32)
    UNPATCHFUNC(RGB16OE,ABGR32)
    UNPATCHFUNC(RGB16OE,RGB24)
    UNPATCHFUNC(RGB16OE,BGR24)
    UNPATCHFUNC(RGB16OE,RGB15OE)
    UNPATCHFUNC(RGB16OE,BGR16OE)
    UNPATCHFUNC(RGB16OE,BGR15OE)

    UNPATCHFUNC(BGR16OE,RGB16)
    UNPATCHFUNC(BGR16OE,RGB15)
    UNPATCHFUNC(BGR16OE,BGR16)
    UNPATCHFUNC(BGR16OE,BGR15)
    UNPATCHFUNC(BGR16OE,ARGB32)
    UNPATCHFUNC(BGR16OE,BGRA32)
    UNPATCHFUNC(BGR16OE,RGBA32)
    UNPATCHFUNC(BGR16OE,ABGR32)
    UNPATCHFUNC(BGR16OE,RGB24)
    UNPATCHFUNC(BGR16OE,BGR24)
    UNPATCHFUNC(BGR16OE,RGB15OE)
    UNPATCHFUNC(BGR16OE,RGB16OE)
    UNPATCHFUNC(BGR16OE,BGR15OE)

    UNPATCHFUNC(RGB15OE,RGB16)
    UNPATCHFUNC(RGB15OE,RGB15)
    UNPATCHFUNC(RGB15OE,BGR16)
    UNPATCHFUNC(RGB15OE,BGR15)
    UNPATCHFUNC(RGB15OE,ARGB32)
    UNPATCHFUNC(RGB15OE,BGRA32)
    UNPATCHFUNC(RGB15OE,RGBA32)
    UNPATCHFUNC(RGB15OE,ABGR32)
    UNPATCHFUNC(RGB15OE,RGB24)
    UNPATCHFUNC(RGB15OE,BGR24)
    UNPATCHFUNC(RGB15OE,RGB16OE)
    UNPATCHFUNC(RGB15OE,BGR16OE)
    UNPATCHFUNC(RGB15OE,BGR15OE)

    UNPATCHFUNC(BGR15OE,RGB16)
    UNPATCHFUNC(BGR15OE,RGB15)
    UNPATCHFUNC(BGR15OE,BGR16)
    UNPATCHFUNC(BGR15OE,BGR15)
    UNPATCHFUNC(BGR15OE,ARGB32)
    UNPATCHFUNC(BGR15OE,BGRA32)
    UNPATCHFUNC(BGR15OE,RGBA32)
    UNPATCHFUNC(BGR15OE,ABGR32)
    UNPATCHFUNC(BGR15OE,RGB24)
    UNPATCHFUNC(BGR15OE,BGR24)
    UNPATCHFUNC(BGR15OE,RGB16OE)
    UNPATCHFUNC(BGR15OE,BGR16OE)
    UNPATCHFUNC(BGR15OE,RGB15OE)
            
    return RETURN_OK;
}
