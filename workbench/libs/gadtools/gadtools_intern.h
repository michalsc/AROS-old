#ifndef GADTOOLS_INTERN_H
#define GADTOOLS_INTERN_H

/* Include files */
#ifndef EXEC_LIBRARIES_H
#   include <exec/libraries.h>
#endif
#ifndef EXEC_SEMAPHORES_H
#   include <exec/semaphores.h>
#endif
#ifndef DOS_DOS_H
#   include <dos/dos.h>
#endif
#ifndef PROTO_GRAPHICS_H
#   include <proto/graphics.h>
#endif
#ifndef GRAPHICS_GFXBASE_H
#   include <graphics/gfxbase.h>
#endif
#ifndef PROTO_INTUITION_H
#   include <proto/intuition.h>
#endif
#ifndef INTUITION_CLASSES_H
#   include <intuition/classes.h>
#endif
#ifndef INTUITION_INTUITIONBASE_H
#   include <intuition/intuitionbase.h>
#endif
#ifndef INTUITION_SCREENS_H
#   include <intuition/screens.h>
#endif
#ifndef PROTO_UTILITY_H
#   include <proto/utility.h>
#endif
#ifndef LIBRARIES_GADTOOLS_H
#   include <libraries/gadtools.h>
#endif
#ifndef PROTO_BOOPSI_H
#   include <proto/boopsi.h>
#endif

struct VisualInfo;

/* Some external stuff (gadtools_init.c) */
struct GadToolsBase_intern; /* prerefrence */

/* Internal prototypes */
struct IntuiText *makeitext(struct GadToolsBase_intern *GadToolsBase,
			    struct NewGadget *ng);
void freeitext(struct GadToolsBase_intern *GadToolsBase,
	       struct IntuiText *itext);
	       
BOOL renderlabel(struct GadToolsBase_intern *GadToolsBase,
		 struct Gadget *gad, struct RastPort *rport, LONG labelplace);
	       
Class *makebuttonclass(struct GadToolsBase_intern *GadToolsBase);
Class *maketextclass(struct GadToolsBase_intern *GadToolsBase);
Class *makesliderclass(struct GadToolsBase_intern *GadToolsBase);
Class *makescrollerclass(struct GadToolsBase_intern *GadToolsBase);
Class *makearrowclass(struct GadToolsBase_intern *GadToolsBase);

struct Gadget *makebutton(struct GadToolsBase_intern *GadToolsBase,
			  struct TagItem stdgadtags[],
			  struct VisualInfo *vi,
			  struct TagItem *taglist);
struct Gadget *makecheckbox(struct GadToolsBase_intern *GadToolsBase,
			    struct TagItem stdgadtags[],
			    struct VisualInfo *vi,
			    struct TagItem *taglist);
struct Gadget *makecycle(struct GadToolsBase_intern *GadToolsBase,
                         struct TagItem stdgadtags[],
                         struct VisualInfo *vi,
                         struct TagItem *taglist);
struct Gadget *makemx(struct GadToolsBase_intern *GadToolsBase,
		      struct TagItem stdgadtags[],
		      struct VisualInfo *vi,
		      struct TagItem *taglist);

struct Gadget *makemx(struct GadToolsBase_intern *GadToolsBase,
		      struct TagItem stdgadtags[],
		      struct VisualInfo *vi,
		      struct TagItem *taglist);

struct Gadget *makepalette(struct GadToolsBase_intern *GadToolsBase,
		      struct TagItem stdgadtags[],
		      struct VisualInfo *vi,
		      struct TagItem *taglist);

struct Gadget *maketext(struct GadToolsBase_intern *GadToolsBase,
		      	struct TagItem stdgadtags[],
		      	struct VisualInfo *vi,
		      	struct TextAttr *tattr,
		      	struct TagItem *taglist);

struct Gadget *makenumber(struct GadToolsBase_intern *GadToolsBase,
		      	  struct TagItem stdgadtags[],
		      	  struct VisualInfo *vi,
		      	  struct TextAttr *tattr,
		      	  struct TagItem *taglist);


struct Gadget *makeslider(struct GadToolsBase_intern *GadToolsBase,
		      	  struct TagItem stdgadtags[],
		      	  struct VisualInfo *vi,
		      	  struct TextAttr *tattr,
		      	  struct TagItem *taglist);

struct Gadget *makescroller(struct GadToolsBase_intern *GadToolsBase,
		      	  struct TagItem stdgadtags[],
		      	  struct VisualInfo *vi,
		      	  struct TagItem *taglist);


/* Tags for the private gadtool classes */
#define GT_Dummy (TAG_USER)

#define GTA_Text_DispFunc	(GT_Dummy + 1)
#define GTA_Text_Format		(GT_Dummy + 2)
#define GTA_Arrow_Type		(GT_Dummy + 3)
#define GTA_Arrow_Pulse		(GT_Dummy + 4)
#define GTA_Arrow_Scroller	(GT_Dummy + 5)
#define GTA_Scroller_Dec	(GT_Dummy + 6)
#define GTA_Scroller_Inc	(GT_Dummy + 7)

struct GadToolsBase_intern
{
    struct Library    library;
    struct ExecBase * sysbase;
    BPTR	      seglist;

    struct IntuitionBase * intuibase;
    struct Library	 * dosbase;
    struct GfxBase	 * gfxbase;
    struct Library	 * utilitybase;
    struct Library	 * boopsibase;

    struct Library	 * aroscbbase;
    struct Library       * aroscybase;
    struct Library	 * arosmxbase;
    struct Library	 * arospabase;

    Class * buttonclass;
    Class * textclass;
    Class * sliderclass;
    Class * scrollerclass;
    Class * arrowclass;
    Class * stringclass;

    /* Semaphore to protect the bevel object. */
    struct SignalSemaphore   bevelsema;
    /* Actually an Object *. The image used for bevel boxes. */
    struct Image           * bevel;
};

/* The following typedefs are necessary, because the names of the global
   variables storing the library base pointers	and the corresponding
   structs are equal.
   This is a hack, of course. */
typedef struct IntuitionBase IntuiBase;
typedef struct GfxBase GraphicsBase;

#define GTB(gtb)        ((struct GadToolsBase_intern *)gtb)
/*
#undef SysBase
#define SysBase (GTB(GadToolsBase)->sysbase)
*/
extern struct ExecBase * SysBase;
#undef IntuitionBase
#define IntuitionBase (GTB(GadToolsBase)->intuibase)
#undef DOSBase
#define DOSBase (GTB(GadToolsBase)->dosbase)
#undef GfxBase
#define GfxBase (GTB(GadToolsBase)->gfxbase)
#undef UtilityBase
#define UtilityBase (GTB(GadToolsBase)->utilitybase)
#undef BOOPSIBase
#define BOOPSIBase (GTB(GadToolsBase)->boopsibase)

#define expunge() \
AROS_LC0(BPTR, expunge, struct GadToolsBase_intern *, GadToolsBase, 3, GadTools)

struct VisualInfo
{
    struct Screen   * vi_screen;
    struct DrawInfo * vi_dri;
};

#define TAG_Left	0
#define TAG_Top 	1
#define TAG_Width	2
#define TAG_Height	3
#define TAG_IText	4
#define TAG_LabelPlace	5
#define TAG_Previous	6
#define TAG_ID		7
#define TAG_DrawInfo	8
#define TAG_UserData	9
#define TAG_Num        10

#endif /* GADTOOLS_INTERN_H */
