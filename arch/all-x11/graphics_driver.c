#include <X11/Xlib.h>
#include <X11/cursorfont.h>

#include <stdlib.h>
#include <stdio.h>
#include <exec/memory.h>
#include <clib/exec_protos.h>
#include <graphics/rastport.h>
#include <graphics/gfxbase.h>
#include <graphics/text.h>
#include <clib/graphics_protos.h>
#include "graphics_intern.h"

#define static	/* nothing */

static Display	     * sysDisplay;
static int	       sysScreen;
static Cursor	       sysCursor;
static struct TextAttr sysTA;

#ifndef SYSFONTNAME
#   define SYSFONTNAME	"8x13bold"
#endif

static const char * sysColName[] = {
    "grey70",
    "black",
    "white",
    "orange",
    NULL
};

static long sysCMap[] = { 0, 0, 0, 0, };
static unsigned long sysPlaneMask;

struct ETextFont
{
    struct TextFont etf_Font;
    XFontStruct     etf_XFS;
};


int driver_init (struct GfxBase * GfxBase)
{
    char * displayName;
    Colormap cm;
    XColor xc;
    XColor fg, bg;
    short t;
    short depth;

    if (!(displayName = getenv ("DISPLAY")) )
	displayName = ":0.0";

    if (!(sysDisplay = XOpenDisplay (displayName)) )
    {
	fprintf (stderr, "Cannot open display %s\n", displayName);
	return False;
    }

    sysScreen = DefaultScreen (sysDisplay);

    depth = DisplayPlanes (sysDisplay, sysScreen);
    cm = DefaultColormap (sysDisplay, sysScreen);

    sysPlaneMask = 0;

    for (t=0; sysColName[t]; t++)
    {
	if (depth == 1)
	{
	    sysCMap[t] = !(t & 1) ?
		    WhitePixel(sysDisplay, sysScreen) :
		    BlackPixel(sysDisplay, sysScreen);
	}
	else
	{
	    if (XParseColor (sysDisplay, cm, sysColName[t], &xc))
	    {
		if (!XAllocColor (sysDisplay, cm, &xc))
		{
		    fprintf (stderr, "Couldn't allocate color %s\n",
			    sysColName[t]);
		    sysCMap[t] = !(t & 1) ?
			    WhitePixel(sysDisplay, sysScreen) :
			    BlackPixel(sysDisplay, sysScreen);
		}

		sysCMap[t] = xc.pixel;

		if (t == 0)
		    bg = xc;
		else if (t == 1)
		    fg = xc;
	    }
	    else
	    {
		fprintf (stderr, "Couldn't get color %s\n", sysColName[t]);
	    }
	}

	sysPlaneMask |= sysCMap[t];
    }

    sysCursor = XCreateFontCursor (sysDisplay, XC_xterm);
    XRecolorCursor (sysDisplay, sysCursor, &fg, &bg);

    return True;
}

int driver_open (struct GfxBase * GfxBase)
{
    struct TextFont * def;

    if (!GfxBase->DefaultFont)
    {
	sysTA.ta_Name = (STRPTR)SYSFONTNAME;

	def = OpenFont (&sysTA);

	if (!def)
	{
	    fprintf (stderr, "Cannot open font %s; trying fixed\n",
		    SYSFONTNAME);

	    sysTA.ta_Name = (STRPTR)"fixed";
	    def = OpenFont (&sysTA);

	    if (!def)
	    {
		fprintf (stderr, "Cannot open font\n");
		return False;
	    }
	}

	GfxBase->DefaultFont = def;
	sysTA.ta_YSize = def->tf_YSize;
    }

    GfxBase->DefaultFont->tf_Accessors ++;

    return True;
}

void driver_close (struct GfxBase * GfxBase)
{
    GfxBase->DefaultFont->tf_Accessors --;

    return;
}

void driver_expunge (struct GfxBase * GfxBase)
{
    CloseFont (GfxBase->DefaultFont);

    return;
}

GC GetGC (struct RastPort * rp)
{
    return (GC) rp->longreserved[0];
}

int GetXWindow (struct RastPort * rp)
{
    return (int) rp->longreserved[1];
}

void SetGC (struct RastPort * rp, GC gc)
{
    rp->longreserved[0] = (ULONG)gc;
}

void SetXWindow (struct RastPort * rp, int win)
{
    rp->longreserved[1] = (ULONG)win;
}

Display * GetSysDisplay (void)
{
    return sysDisplay;
}

int GetSysScreen (void)
{
    return sysScreen;
}

void driver_SetAPen (struct RastPort * rp, unsigned long pen)
{
    if (pen == (unsigned long)-1L)
	pen = 3;

    pen &= 3;

    XSetForeground (sysDisplay, GetGC (rp), sysCMap[pen]);
}

void driver_SetBPen (struct RastPort * rp, unsigned long pen)
{
    if (pen == (unsigned long)-1L)
	pen = 3;

    pen &= 3;

    XSetBackground (sysDisplay, GetGC (rp), sysCMap[pen]);
}

void driver_SetDrMd (struct RastPort * rp, unsigned long mode)
{
    if (mode & COMPLEMENT)
	XSetFunction (sysDisplay, GetGC(rp), GXxor);
    else
	XSetFunction (sysDisplay, GetGC(rp), GXcopy);
}

void driver_RectFill (struct RastPort * rp, long x1, long y1, long x2, long y2)
{
    XFillRectangle (sysDisplay, GetXWindow (rp), GetGC (rp),
	    x1, y1, x2-x1+1, y2-y1+1);
}

#define SWAP(a,b)       { a ^= b; b ^= a; a ^= b; }
#define ABS(x)          ((x) < 0 ? -(x) : (x))

void driver_ScrollRaster (struct RastPort * rp, long dx, long dy,
	long x1, long y1, long x2, long y2, struct GfxBase * GfxBase)
{
    long w, h, x3, y3, x4, y4, _dx_, _dy_;
    long apen = rp->FgPen;

    if (!dx && !dy) return;

    if (x2 < x1) SWAP (x1,x2)
    if (y2 < y1) SWAP (y1,y2)

    _dx_ = ABS(dx);
    _dy_ = ABS(dy);

    x3 = x1 + _dx_;
    y3 = y1 + _dy_;

    x4 = x2 - _dx_ +1;
    y4 = y2 - _dy_ +1;

    w = x2 - x3 +1;
    h = y2 - y3 +1;

    SetAPen (rp, rp->BgPen);

    if (dx <= 0) {
	if (dy <= 0) {
	    XCopyArea (sysDisplay,GetXWindow(rp),GetXWindow(rp),GetGC(rp),
		    x1, y1, w, h, x3, y3);

	    if (_dy_) XClearArea (sysDisplay,GetXWindow(rp),
		    x1, y1, w+_dx_, _dy_, FALSE);

	    if (_dx_) XClearArea (sysDisplay,GetXWindow(rp),
		    x1, y1, _dx_, h, FALSE);

	} else { /* dy > 0 */
	    XCopyArea (sysDisplay,GetXWindow(rp),GetXWindow(rp),GetGC(rp),
		    x1, y3, w, h, x3, y1);

	    XClearArea (sysDisplay,GetXWindow(rp),
		    x1, y4, w+_dx_, _dy_, FALSE);

	    if (_dx_) XClearArea (sysDisplay,GetXWindow(rp),
		    x1, y1, _dx_, h, FALSE);
	}
    } else { /* dx > 0 */
	if (dy <= 0) {
	    XCopyArea (sysDisplay,GetXWindow(rp),GetXWindow(rp),GetGC(rp),
		    x3, y1, w, h, x1, y3);

	    if (_dy_) XClearArea (sysDisplay,GetXWindow(rp),
		    x1, y1, w+_dx_, _dy_, FALSE);

	    XClearArea (sysDisplay,GetXWindow(rp),
		    x4, y3, _dx_, h, FALSE);
	} else { /* dy > 0 */
	    XCopyArea (sysDisplay,GetXWindow(rp),GetXWindow(rp),GetGC(rp),
		    x3, y3, w, h, x1, y1);

	    XClearArea (sysDisplay,GetXWindow(rp),
		    x1, y4, w+_dx_, _dy_, FALSE);

	    XClearArea (sysDisplay,GetXWindow(rp),
		    x4, y1, _dx_, h, FALSE);
	}
    }

    SetAPen (rp, apen);
}

void driver_Text (struct RastPort * rp, char * string, long len)
{
    struct ETextFont * etf;

    if (rp->DrawMode & JAM2)
	XDrawImageString (sysDisplay, GetXWindow(rp), GetGC(rp), rp->cp_x,
	    rp->cp_y, string, len);
    else
	XDrawString (sysDisplay, GetXWindow(rp), GetGC(rp), rp->cp_x,
	    rp->cp_y, string, len);

    etf = (struct ETextFont *)rp->Font;

    rp->cp_x += XTextWidth (&etf->etf_XFS, string, len);
}

void driver_Move (struct RastPort * rp, long x, long y)
{
    return;
}

void driver_Draw (struct RastPort * rp, long x, long y)
{
    XDrawLine (sysDisplay, GetXWindow(rp), GetGC(rp),
	    rp->cp_x, rp->cp_y,
	    x, y);
}

void driver_WritePixel (struct RastPort * rp, long x, long y)
{
    XDrawPoint (sysDisplay, GetXWindow(rp), GetGC(rp),
	    x, y);
}

void driver_SetRast (struct RastPort * rp, unsigned long color)
{
    XClearArea (sysDisplay, GetXWindow(rp),
	    0, 0,
	    1000, 1000,
	    FALSE);
}

void driver_SetFont (struct RastPort * rp, struct ETextFont * font)
{
    if (GetGC(rp))
	XSetFont (sysDisplay, GetGC(rp), font->etf_XFS.fid);

    rp->Font	   = (struct TextFont *)font;
    rp->TxWidth    = font->etf_Font.tf_XSize;
    rp->TxHeight   = font->etf_Font.tf_YSize;
    rp->TxBaseline = font->etf_Font.tf_Baseline;

    font->etf_Font.tf_Accessors ++;
}

struct TextFont * driver_OpenFont (struct TextAttr * ta,
	struct GfxBase * GfxBase)
{
    struct ETextFont * tf;
    XFontStruct      * xfs;

    if (!(tf = AllocMem (sizeof (struct ETextFont), MEMF_ANY)) )
	return (NULL);

    xfs = XLoadQueryFont (sysDisplay, ta->ta_Name);

    if (!xfs) {
	free (tf);
	return (NULL);
    }

    tf->etf_XFS = *xfs;

    tf->etf_Font.tf_YSize = tf->etf_XFS.max_bounds.ascent +
		    tf->etf_XFS.max_bounds.descent;
    tf->etf_Font.tf_XSize = tf->etf_XFS.max_bounds.rbearing -
		    tf->etf_XFS.min_bounds.lbearing;
    tf->etf_Font.tf_Baseline = tf->etf_XFS.ascent;
    tf->etf_Font.tf_LoChar = tf->etf_XFS.min_char_or_byte2;
    tf->etf_Font.tf_HiChar = tf->etf_XFS.max_char_or_byte2;
    tf->etf_Font.tf_Accessors ++;

    if (!tf->etf_Font.tf_XSize || !tf->etf_Font.tf_YSize)
    {
	fprintf (stderr, "Error: Fontsize");
	exit (1);
    }

    return (struct TextFont *)tf;
}

void driver_CloseFont (struct ETextFont * tf, struct GfxBase * GfxBase)
{
    tf->etf_Font.tf_Accessors --;

    if (!tf->etf_Font.tf_Accessors)
    {
	XUnloadFont (sysDisplay, tf->etf_XFS.fid);
	FreeMem (tf, sizeof (struct ETextFont));
    }
}


