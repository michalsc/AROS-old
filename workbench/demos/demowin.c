/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $VER: demowin.c 1.16 (2.10.96)

    Desc: A short demo for the features of Intuition and Graphics
    Lang: english
*/
#define ENABLE_RT	1
#define ENABLE_PURIFY	1

#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/exall.h>
#include <dos/datetime.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/aros_protos.h>
#include <clib/alib_protos.h>
#include <clib/utility_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/console_protos.h>
#include <intuition/intuitionbase.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <ctype.h>
#include <aros/rt.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>

#if 1
#   define D(x)    x
#else
#   define D(x)     /* eps */
#endif
#define bug	kprintf

struct Library *ConsoleDevice;
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
Object * image;

void Refresh (struct RastPort * rp)
{
    int len;
    int t, tend, pen;

    SetAPen (rp, 1);
    SetDrMd (rp, JAM2);

    Move (rp, 0, 0);
    Draw (rp, 320, 150);

    Move (rp, 640, 0);
    Draw (rp, 0, 300);

    Move (rp, 300, 40);
    Text (rp, "Hello World.", 12);

    SetAPen (rp, 3);
    RectFill (rp, 90, 0, 120, 30);
    SetAPen (rp, 0);
    RectFill (rp, 100, 10, 110, 20);

    SetAPen (rp, 1);
    RectFill (rp, 150, 10, 160, 20);

    SetAPen (rp, 2);
    RectFill (rp, 200, 10, 210, 20);

    SetAPen (rp, 3);
    RectFill (rp, 250, 10, 260, 20);

    len = TextLength (rp, "Hello World.", 12);

    SetAPen (rp, 2);
    RectFill (rp, 299, 59, 300+len, 60+rp->Font->tf_YSize);

    SetAPen (rp, 1);
    Move (rp, 300, 60 + rp->Font->tf_Baseline);
    Text (rp, "Hello World.", 12);

    SetDrMd (rp, JAM1);
    SetAPen (rp, 1);
    Move (rp, 301, 101);
    Text (rp, "Hello World.", 12);
    SetAPen (rp, 2);
    Move (rp, 300, 100);
    Text (rp, "Hello World.", 12);

    Move (rp, 20, 350);
    Text (rp, "Press \"Complement\" to flip PropGadgets", 39);

    if (image)
	DrawImageState (rp, (struct Image *)image, 10, 10, IDS_NORMAL, NULL);

    tend = 10;
    t = 0;

    for (pen=1; pen<16; pen++)
    {
	SetAPen (rp, pen);

	for ( ; t<tend; t++)
	{
	    DrawEllipse (rp, 160, 150, t, t>>1);
	    DrawEllipse (rp, 160, 151, t, t>>1);
	}

	tend += 10;
    }
}

#define GAD_WID     100
#define GAD_HEI     30
#define BORDER	    20

WORD BorderData[6*2*2] =
{
    0, GAD_HEI-1, /* Top (lighter) edge */
    1, GAD_HEI-2,
    1, 1,
    GAD_WID-2, 1,
    GAD_WID-1, 0,
    0, 0,

    0, -(GAD_HEI-2), /* Bottom (darker) edge */
    -1, -(GAD_HEI-3),
    -1, -1,
    -(GAD_WID-3), -1,
    -(GAD_WID-2), 0,
    -1, 0,
};
struct Border
DemoBottomBorder =
{
    GAD_WID-1, GAD_HEI-1,
    1, 2,
    JAM1,
    6,
    &BorderData[6*2],
    NULL
},
DemoTopBorder =
{
    0, 0,
    2, 1,
    JAM1,
    6,
    &BorderData[0],
    &DemoBottomBorder
};
struct Border DemoIBottomBorder =
{
    GAD_WID-1, GAD_HEI-1,
    2, 1,
    JAM1,
    6,
    &BorderData[6*2],
    NULL
},
DemoITopBorder =
{
    0, 0,
    1, 2,
    JAM1,
    6,
    &BorderData[0],
    &DemoIBottomBorder
};

struct PropInfo
DemoPropInfo1 =
{
    AUTOKNOB | FREEHORIZ | PROPNEWLOOK,
    0, 0,
    MAXBODY, MAXBODY,
    0,0,0,0,0,0
},
DemoPropInfo2 =
{
    AUTOKNOB | FREEVERT | PROPNEWLOOK,
    0, 0,
    MAXBODY, MAXBODY,
    0,0,0,0,0,0
},
DemoPropInfo3 =
{
    AUTOKNOB | FREEHORIZ | FREEVERT | PROPNEWLOOK,
    0, 0,
    MAXBODY, MAXBODY,
    0,0,0,0,0,0
};

struct IntuiText
DemoIText =
{
    1, 2, /* Pens */
    JAM1, /* Drawmode */
    10, 10, /* Left, Top */
    NULL, /* TextAttr */
    "None", /* Text */
    NULL /* Next */
};

#include "images/ArrowUp0.h"
#include "images/ArrowUp1.h"
#include "images/ArrowDown0.h"
#include "images/ArrowDown1.h"
#include "images/ArrowLeft0.h"
#include "images/ArrowLeft1.h"
#include "images/ArrowRight0.h"
#include "images/ArrowRight1.h"
#include "images/ImageButton0.h"
#include "images/ImageButton1.h"

struct Gadget
DemoGadget12 =
{
    NULL, /* &DemoGadget, / * NextGadget */
    -(BORDER+2*ARROWLEFT1_WIDTH), -((GAD_HEI+BORDER)*3),
    ARROWLEFT1_WIDTH, ARROWLEFT1_HEIGHT, /* hit box */
    GFLG_GADGHIMAGE
	| GFLG_RELRIGHT
	| GFLG_RELBOTTOM
	| GFLG_GADGIMAGE
	, /* Flags */
    GACT_IMMEDIATE | GACT_RELVERIFY, /* Activation */
    GTYP_BOOLGADGET, /* Type */
    &ArrowLeft0Image, &ArrowLeft1Image, /* Render */
    NULL, /* Text */
    0L, NULL, /* MutualExcl, SpecialInfo */
    13, /* GadgetID */
    NULL /* UserData */
},
DemoGadget11 =
{
    &DemoGadget12, /* NextGadget */
    -(BORDER+1*ARROWLEFT1_WIDTH), -((GAD_HEI+BORDER)*3),
    ARROWLEFT1_WIDTH, ARROWLEFT1_HEIGHT, /* hit box */
    GFLG_GADGHIMAGE
	| GFLG_RELRIGHT
	| GFLG_RELBOTTOM
	| GFLG_GADGIMAGE
	, /* Flags */
    GACT_IMMEDIATE | GACT_RELVERIFY, /* Activation */
    GTYP_BOOLGADGET, /* Type */
    &ArrowRight0Image, &ArrowRight1Image, /* Render */
    NULL, /* Text */
    0L, NULL, /* MutualExcl, SpecialInfo */
    12, /* GadgetID */
    NULL /* UserData */
},
DemoGadget10 =
{
    &DemoGadget11, /* NextGadget */
    -(BORDER+ARROWDOWN1_WIDTH), -((GAD_HEI+BORDER)*3+ARROWLEFT1_HEIGHT+0*ARROWDOWN1_HEIGHT-5),
    ARROWDOWN1_WIDTH, ARROWDOWN1_HEIGHT, /* hit box */
    GFLG_GADGHIMAGE
	| GFLG_RELRIGHT
	| GFLG_RELBOTTOM
	| GFLG_GADGIMAGE
	, /* Flags */
    GACT_IMMEDIATE | GACT_RELVERIFY, /* Activation */
    GTYP_BOOLGADGET, /* Type */
    &ArrowDown0Image, &ArrowDown1Image, /* Render */
    NULL, /* Text */
    0L, NULL, /* MutualExcl, SpecialInfo */
    11, /* GadgetID */
    NULL /* UserData */
},
DemoGadget9 =
{
    &DemoGadget10, /* NextGadget */
    -(BORDER+ARROWDOWN1_WIDTH), -((GAD_HEI+BORDER)*3+ARROWLEFT1_HEIGHT+1*ARROWDOWN1_HEIGHT-5),
    ARROWDOWN1_WIDTH, ARROWDOWN1_HEIGHT, /* hit box */
    GFLG_GADGHIMAGE
	| GFLG_RELRIGHT
	| GFLG_RELBOTTOM
	| GFLG_GADGIMAGE
	, /* Flags */
    GACT_IMMEDIATE | GACT_RELVERIFY, /* Activation */
    GTYP_BOOLGADGET, /* Type */
    &ArrowUp0Image, &ArrowUp1Image, /* Render */
    NULL, /* Text */
    0L, NULL, /* MutualExcl, SpecialInfo */
    10, /* GadgetID */
    NULL /* UserData */
},
DemoGadget8 =
{
    &DemoGadget9, /* NextGadget */
    -(BORDER+ARROWDOWN1_WIDTH+GAD_WID), -((GAD_HEI+BORDER)*3+GAD_WID),
    GAD_WID, GAD_WID, /* hit box */
    GFLG_GADGHIMAGE
	| GFLG_RELRIGHT
	| GFLG_RELBOTTOM
	, /* Flags */
    GACT_IMMEDIATE
	| GACT_RELVERIFY
	| GACT_TOGGLESELECT
	, /* Activation */
    GTYP_PROPGADGET, /* Type */
    NULL, NULL, /* Render */
    NULL, /* Text */
    0L, (APTR)&DemoPropInfo3, /* MutualExcl, SpecialInfo */
    9, /* GadgetID */
    NULL /* UserData */
},
DemoGadget7 =
{
    &DemoGadget8, /* NextGadget */
    -(BORDER+ARROWDOWN1_WIDTH), BORDER,
    ARROWDOWN1_WIDTH, -(GAD_HEI*3+BORDER*4+2*ARROWDOWN1_HEIGHT), /* hit box */
    GFLG_GADGHIMAGE
	| GFLG_RELRIGHT
	| GFLG_RELHEIGHT
	, /* Flags */
    GACT_IMMEDIATE
	| GACT_RELVERIFY
	| GACT_TOGGLESELECT
	, /* Activation */
    GTYP_PROPGADGET, /* Type */
    NULL, NULL, /* Render */
    NULL, /* Text */
    0L, (APTR)&DemoPropInfo2, /* MutualExcl, SpecialInfo */
    8, /* GadgetID */
    NULL /* UserData */
},
DemoGadget6 =
{
    &DemoGadget7, /* NextGadget */
    BORDER, -((GAD_HEI+BORDER)*3),
    -(2*BORDER+2*ARROWLEFT0_WIDTH), ARROWLEFT0_HEIGHT, /* hit box */
    GFLG_GADGHIMAGE
	| GFLG_RELBOTTOM
	| GFLG_RELWIDTH
	, /* Flags */
    GACT_IMMEDIATE
	| GACT_RELVERIFY
	| GACT_TOGGLESELECT
	, /* Activation */
    GTYP_PROPGADGET, /* Type */
    NULL, NULL, /* Render */
    NULL, /* Text */
    0L, (APTR)&DemoPropInfo1, /* MutualExcl, SpecialInfo */
    7, /* GadgetID */
    NULL /* UserData */
},
DemoGadget5 =
{
    &DemoGadget6, /* NextGadget */
    BORDER+(GAD_WID+BORDER)*4, -((GAD_HEI+BORDER)*2), GAD_WID, GAD_HEI, /* hit box */
    GFLG_GADGHIMAGE
	| GFLG_LABELSTRING
	| GFLG_RELBOTTOM
	, /* Flags */
    GACT_IMMEDIATE
	| GACT_RELVERIFY
	| GACT_TOGGLESELECT
	, /* Activation */
    GTYP_BOOLGADGET, /* Type */
    &DemoTopBorder, &DemoITopBorder, /* Render */
    (struct IntuiText *)"Toggle", /* Text */
    0L, NULL, /* MutualExcl, SpecialInfo */
    6, /* GadgetID */
    NULL /* UserData */
},
DemoGadget4 =
{
    &DemoGadget5, /* NextGadget */
    BORDER+(GAD_WID+BORDER)*3, -((GAD_HEI+BORDER)*2), GAD_WID, GAD_HEI, /* hit box */
    GFLG_GADGHNONE
	| GFLG_LABELITEXT
	| GFLG_RELBOTTOM
	, /* Flags */
    GACT_IMMEDIATE | GACT_RELVERIFY, /* Activation */
    GTYP_BOOLGADGET, /* Type */
    &DemoTopBorder, NULL, /* Render */
    &DemoIText, /* Text */
    0L, NULL, /* MutualExcl, SpecialInfo */
    5, /* GadgetID */
    NULL /* UserData */
},
DemoGadget3 =
{
    &DemoGadget4, /* NextGadget */
    BORDER+(GAD_WID+BORDER)*2, -((GAD_HEI+BORDER)*2),
    IMAGEBUTTON0_WIDTH, IMAGEBUTTON0_HEIGHT, /* hit box */
    GFLG_GADGHIMAGE
	| GFLG_LABELSTRING
	| GFLG_RELBOTTOM
	| GFLG_GADGIMAGE
	, /* Flags */
    GACT_IMMEDIATE | GACT_RELVERIFY, /* Activation */
    GTYP_BOOLGADGET, /* Type */
    &ImageButton0Image, &ImageButton1Image, /* Render */
    (struct IntuiText *)"Image", /* Text */
    0L, NULL, /* MutualExcl, SpecialInfo */
    4, /* GadgetID */
    NULL /* UserData */
},
DemoGadget2 =
{
    &DemoGadget3, /* NextGadget */
    BORDER+(GAD_WID+BORDER)*1, -((GAD_HEI+BORDER)*2), GAD_WID, GAD_HEI, /* hit box */
    GFLG_GADGHBOX
	| GFLG_LABELSTRING
	| GFLG_RELBOTTOM
	, /* Flags */
    GACT_IMMEDIATE | GACT_RELVERIFY, /* Activation */
    GTYP_BOOLGADGET, /* Type */
    &DemoTopBorder, NULL, /* Render */
    (struct IntuiText *)"Box", /* Text */
    0L, NULL, /* MutualExcl, SpecialInfo */
    3, /* GadgetID */
    NULL /* UserData */
},
DemoGadget1 =
{
    &DemoGadget2, /* NextGadget */
    BORDER+(GAD_WID+BORDER)*0, -((GAD_HEI+BORDER)*2), GAD_WID, GAD_HEI, /* hit box */
    GFLG_GADGHCOMP
	| GFLG_LABELSTRING
	| GFLG_RELBOTTOM
	, /* Flags */
    GACT_IMMEDIATE | GACT_RELVERIFY, /* Activation */
    GTYP_BOOLGADGET, /* Type */
    &DemoTopBorder, NULL, /* Render */
    (struct IntuiText *)"Complement", /* Text */
    0L, NULL, /* MutualExcl, SpecialInfo */
    2, /* GadgetID */
    NULL /* UserData */
},
ExitGadget =
{
    &DemoGadget1, /* NextGadget */
    BORDER, -(GAD_HEI+BORDER), GAD_WID, GAD_HEI, /* hit box */
    GFLG_GADGHIMAGE
	| GFLG_LABELSTRING
	| GFLG_RELBOTTOM
	, /* Flags */
    GACT_RELVERIFY, /* Activation */
    GTYP_BOOLGADGET, /* Type */
    &DemoTopBorder, &DemoITopBorder, /* Render */
    (struct IntuiText *)"Exit", /* Text */
    0L, NULL, /* MutualExcl, SpecialInfo */
    1, /* GadgetID */
    NULL /* UserData */
};

int main (int argc, char ** argv)
{
    struct Window * win;
    struct RastPort * rp;
    struct IntuiMessage * im;
    struct IOStdReq cioreq;
    struct InputEvent ievent =
    {
	NULL,
	IECLASS_RAWKEY,
	/* ... */
    };
    int cont, draw;
    int prop;

    bug("Welcome to the window demo of AROS\n");

    GfxBase=(struct GfxBase *)OpenLibrary(GRAPHICSNAME,39);
    IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",39);

    if (!GfxBase)
    {
	bug ("Couldn't open %s\n", GRAPHICSNAME);
	goto end;
    }

    if (!IntuitionBase)
    {
	bug ("Couldn't open intuition.library\n");
	goto end;
    }

    D(bug("main=%p Refresh=%p\n"
	, main
	, Refresh
    ));

    OpenDevice ("console.device", -1, (struct IORequest *)&cioreq, 0);
    ConsoleDevice = (struct Library *)cioreq.io_Device;
    bug ("Opening console.device=%p\n", ConsoleDevice);

    if (!ConsoleDevice)
    {
	D(bug("Couldn't open console\n"));
	return 10;
    }

    win = OpenWindowTags (NULL
	, WA_Title,	    "Open a window demo"
	, WA_Left,	    100
	, WA_Top,	    100
	, WA_Width,	    640
	, WA_Height,	    512
	, WA_IDCMP,	    IDCMP_RAWKEY
			    | IDCMP_REFRESHWINDOW
			    | IDCMP_MOUSEBUTTONS
			    | IDCMP_MOUSEMOVE
			    | IDCMP_GADGETDOWN
			    | IDCMP_GADGETUP
	, WA_SimpleRefresh, TRUE
	, WA_Gadgets,	    &ExitGadget
	, TAG_END
    );
    D(bug("OpenWindow win=%p\n", win));

    if (!win)
    {
	D(bug("Couldn't open window\n"));
	return 10;
    }

    rp = win->RPort;

    DemoIText.LeftEdge = GAD_WID/2 - rp->Font->tf_XSize*2;
    DemoIText.TopEdge = GAD_HEI/2 - rp->Font->tf_YSize/2 + rp->Font->tf_Baseline;

    image = NewObject (NULL, IMAGECLASS
	, SYSIA_Which, DOWNIMAGE
	, TAG_END
    );

    if (!image)
	bug("Warning: Couldn't create image\n");

    cont = 1;
    draw = 0;
    prop = 0;

    while (cont)
    {
	if ((im = (struct IntuiMessage *)GetMsg (win->UserPort)))
	{
	    /* D("Got msg\n"); */
	    switch (im->Class)
	    {
	    case IDCMP_RAWKEY: {
		UBYTE buf[10];
		int   len;
		int   t;

		ievent.ie_Code	    = im->Code;
		ievent.ie_Qualifier = im->Qualifier;

		len = RawKeyConvert (&ievent, buf, sizeof (buf), NULL);

		bug ("Key %s %3ld + Qual %08lx=\""
		    , (im->Code & 0x8000) ? "up  " : "down"
		    , (LONG)(im->Code & 0xFF)
		    , (LONG)im->Qualifier
		);

		if (len < 0)
		{
		    bug ("\" (buffer too short)");
		    break;
		}

		for (t=0; t<len; t++)
		{
		    if (buf[t] < 32 || (buf[t] >= 127 && buf[t] < 160))
		    {
			switch (buf[t])
			{
			case '\n':
			    bug ("\\n");
			    break;

			case '\t':
			    bug ("\\t");
			    break;

			case '\b':
			    bug ("\\b");
			    break;

			case '\r':
			    bug ("\\r");
			    break;

			case 0x1B:
			    bug ("^[");
			    break;

			default:
			    bug ("\\x%02x", buf[t]);
			    break;
			} /* switch */
		    }
		    else
			bug ("%c", buf[t]);
		}
		bug ("\"\n");

		if (*buf == '\x1b' && len == 1)
		{
		    if (len == 1)
			cont = 0;
		}

		break; }

	    case IDCMP_MOUSEBUTTONS:
		switch (im->Code)
		{
		case SELECTDOWN:
		    SetAPen (rp, 2);
		    Move (rp, im->MouseX, im->MouseY);
		    draw = 1;
		    break;

		case SELECTUP:
		    draw = 0;
		    break;

		case MIDDLEDOWN:
		    SetAPen (rp, 1);
		    Move (rp, im->MouseX, im->MouseY);
		    draw = 1;
		    break;

		case MIDDLEUP:
		    draw = 0;
		    break;

		case MENUDOWN:
		    SetAPen (rp, 3);
		    Move (rp, im->MouseX, im->MouseY);
		    draw = 1;
		    break;

		case MENUUP:
		    draw = 0;
		    break;

		}

		break;

	    case IDCMP_MOUSEMOVE:
		if (draw)
		    Draw (rp, im->MouseX, im->MouseY);

		break;

	    case IDCMP_REFRESHWINDOW:
		Refresh (rp);
		break;

	    case IDCMP_GADGETDOWN: {
		struct Gadget * gadget;
		LONG pot;

		gadget = (struct Gadget *)im->IAddress;

		bug ("User pressed gadget %ld\n", gadget->GadgetID);

		switch (gadget->GadgetID)
		{
		case 10: /* Up */
		    pot = DemoPropInfo3.VertPot - DemoPropInfo3.VertBody;

		    if (pot < 0)
			pot = 0;

		    NewModifyProp (&DemoGadget7
			, win
			, NULL
			, DemoPropInfo2.Flags
			, DemoPropInfo2.HorizPot
			, pot
			, DemoPropInfo2.HorizBody
			, DemoPropInfo2.VertBody
			, 1
		    );
		    NewModifyProp (&DemoGadget8
			, win
			, NULL
			, DemoPropInfo3.Flags
			, DemoPropInfo3.HorizPot
			, pot
			, DemoPropInfo3.HorizBody
			, DemoPropInfo3.VertBody
			, 1
		    );

		    break;

		case 11: /* Down */
		    pot = DemoPropInfo3.VertPot + DemoPropInfo3.VertBody;

		    if (pot > MAXPOT)
			pot = MAXPOT;

		    NewModifyProp (&DemoGadget7
			, win
			, NULL
			, DemoPropInfo2.Flags
			, DemoPropInfo2.HorizPot
			, pot
			, DemoPropInfo2.HorizBody
			, DemoPropInfo2.VertBody
			, 1
		    );
		    NewModifyProp (&DemoGadget8
			, win
			, NULL
			, DemoPropInfo3.Flags
			, DemoPropInfo3.HorizPot
			, pot
			, DemoPropInfo3.HorizBody
			, DemoPropInfo3.VertBody
			, 1
		    );

		    break;

		case 12: /* Right */
		    pot = DemoPropInfo1.HorizPot + DemoPropInfo1.HorizBody;

		    if (pot > MAXPOT)
			pot = MAXPOT;

		    NewModifyProp (&DemoGadget6
			, win
			, NULL
			, DemoPropInfo1.Flags
			, pot
			, DemoPropInfo1.VertPot
			, DemoPropInfo1.HorizBody
			, DemoPropInfo1.VertBody
			, 1
		    );
		    NewModifyProp (&DemoGadget8
			, win
			, NULL
			, DemoPropInfo3.Flags
			, pot
			, DemoPropInfo3.VertPot
			, DemoPropInfo3.HorizBody
			, DemoPropInfo3.VertBody
			, 1
		    );

		    break;

		case 13: /* Left */
		    pot = DemoPropInfo1.HorizPot - DemoPropInfo1.HorizBody;

		    if (pot < 0)
			pot = 0;

		    NewModifyProp (&DemoGadget6
			, win
			, NULL
			, DemoPropInfo1.Flags
			, pot
			, DemoPropInfo1.VertPot
			, DemoPropInfo1.HorizBody
			, DemoPropInfo1.VertBody
			, 1
		    );
		    NewModifyProp (&DemoGadget8
			, win
			, NULL
			, DemoPropInfo3.Flags
			, pot
			, DemoPropInfo3.VertPot
			, DemoPropInfo3.HorizBody
			, DemoPropInfo3.VertBody
			, 1
		    );

		    break;

		}

		break; }

	    case IDCMP_GADGETUP: {
		struct Gadget * gadget;

		gadget = (struct Gadget *)im->IAddress;

		bug ("User released gadget %ld\n", gadget->GadgetID);

		if (gadget->GadgetID == 1)
		    cont = 0;
		else if (gadget->GadgetID == 2)
		{
		    prop ++;

		    switch (prop)
		    {
		    case 0:
			NewModifyProp (&DemoGadget6, win, NULL,
			    DemoPropInfo1.Flags,
			    0, 0, MAXBODY, MAXBODY, 1);
			NewModifyProp (&DemoGadget7, win, NULL,
			    DemoPropInfo2.Flags,
			    0, 0, MAXBODY, MAXBODY, 1);
			NewModifyProp (&DemoGadget8, win, NULL,
			    DemoPropInfo3.Flags,
			    0, 0, MAXBODY, MAXBODY, 1);
			break;

		    case 1:
			NewModifyProp (&DemoGadget6, win, NULL,
			    DemoPropInfo1.Flags,
			    0, 0, MAXBODY/2, MAXBODY, 1);
			NewModifyProp (&DemoGadget7, win, NULL,
			    DemoPropInfo2.Flags,
			    0, 0, MAXBODY, MAXBODY/2, 1);
			NewModifyProp (&DemoGadget8, win, NULL,
			    DemoPropInfo3.Flags,
			    0, 0, MAXBODY/2, MAXBODY/2, 1);
			break;

		    case 2:
			NewModifyProp (&DemoGadget6, win, NULL,
			    DemoPropInfo1.Flags,
			    MAXPOT, 0, MAXBODY/2, MAXBODY, 1);
			NewModifyProp (&DemoGadget7, win, NULL,
			    DemoPropInfo2.Flags,
			    0, MAXPOT, MAXBODY, MAXBODY/2, 1);
			NewModifyProp (&DemoGadget8, win, NULL,
			    DemoPropInfo3.Flags,
			    MAXPOT, MAXPOT, MAXBODY/2, MAXBODY/2, 1);
			break;

		    default:
			NewModifyProp (&DemoGadget6, win, NULL,
			    DemoPropInfo1.Flags,
			    0, 0, MAXBODY/9, MAXBODY, 1);
			NewModifyProp (&DemoGadget7, win, NULL,
			    DemoPropInfo2.Flags,
			    0, 0, MAXBODY, MAXBODY/9, 1);
			NewModifyProp (&DemoGadget8, win, NULL,
			    DemoPropInfo3.Flags,
			    0, 0, MAXBODY/9, MAXBODY/9, 1);
			prop = -1;
			break;

		    }
		}

		break; }

	    } /* switch */

	    Flush (Output ());

	    ReplyMsg ((struct Message *)im);
	}
	else
	{
	    /* D("Waiting\n"); */
	    Wait (1L << win->UserPort->mp_SigBit);
	}
    }

    D(bug("CloseWindow (%p)\n", win));
    CloseWindow (win);

end:
    if (image)
	DisposeObject (image);

    if (GfxBase)
	CloseLibrary ((struct Library *)GfxBase);

    if (IntuitionBase)
	CloseLibrary ((struct Library *)IntuitionBase);

    return 0;
}
