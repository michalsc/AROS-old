#ifndef AROSMUTUALEXCLUDE_INTERN_H
#define AROSMUTUALEXCLUDE_INTERN_H

#undef  AROS_ALMOST_COMPATIBLE 
#define AROS_ALMOST_COMPATIBLE 


#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif
#ifndef EXEC_LIBRARIES_H
#   include <exec/libraries.h>
#endif
#ifndef UTILITY_HOOKS_H
#   include <utility/hooks.h>
#endif
#ifndef DOS_BPTR_H
#   include <dos/bptr.h>
#endif
#ifndef GRAPHICS_RASTPORT_H
#   include <graphics/rastport.h>
#endif
#ifndef INTUITION_CLASSES_H
#   include <intuition/classes.h>
#endif
#ifndef INTUITION_CGHOOKS_H
#   include <intuition/cghooks.h>
#endif
#ifndef INTUITION_GADGETCLASS_H
#   include <intuition/gadgetclass.h>
#endif

/* Predeclaration */
struct MXBase_intern;

#define GLOBAL_INTUIBASE

#define TURN_OFF_DEBUG


/* Support */
#define G(obj) ((struct Gadget *)(obj))


/* MutualExcludeClass definitions */
struct MXData {
    struct DrawInfo *dri;
    struct Image *mximage;
    ULONG active, newactive;
    STRPTR *labels;
    ULONG numlabels;
    UWORD spacing;
    LONG labelplace;
};


/* Prototypes */
void drawdisabledpattern(struct MXBase_intern *AROSMutualExcludeBase, struct RastPort *rport, UWORD pen, WORD left, WORD top, UWORD width, UWORD height);
BOOL renderlabel(struct MXBase_intern *AROSMutualExcludeBase,
		 struct Gadget *gad, struct RastPort *rport, LONG labelplace);



/* Library stuff */
struct MXBase_intern
{
    struct Library 	library;
    struct ExecBase	*sysbase;
    BPTR		seglist;
    struct Library	*dosbase;

    #ifndef GLOBAL_INTUIBASE
    struct IntuitionBase *intuitionbase;
    #endif
    struct GfxBase	*gfxbase;
    struct Library	*utilitybase;
    
    struct IClass	*classptr;
	
};

/* The following typedefs are necessary, because the names of the global
   variables storing the library base pointers	and the corresponding
   structs are equal.
   This is a hack, of course. */
typedef struct GfxBase GraphicsBase;
typedef struct IntuitionBase IntuiBase;

#undef MXB
#define MXB(b) ((struct MXBase_intern *)b)
#undef UtilityBase
#define UtilityBase 	MXB(AROSMutualExcludeBase)->utilitybase


#ifndef GLOBAL_INTUIBASE
#undef IntuitionBase
#define IntuitionBase	MXB(AROSMutualExcludeBase)->intuitionbase
#endif

#undef GfxBase
#define GfxBase		MXB(AROSMutualExcludeBase)->gfxbase
#undef SysBase
#define SysBase		MXB(AROSMutualExcludeBase)->sysbase
#undef DOSBase
#define DOSBase		MXB(AROSMutualExcludeBase)->dosbase


#define expunge() \
AROS_LC0(BPTR, expunge, struct MXBase_intern *, AROSMutualExcludeBase, 3, AROSMutualExclude)

#endif /* AROSMUTUALEXCLUDE_INTERN_H */
