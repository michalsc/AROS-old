/*
    (C) 1997 AROS - The Amiga Research OS
    $Id$

    Desc: AROSListview initialization code.
    Lang: English.
*/
#define AROS_ALMOST_COMPATIBLE
#include <stddef.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <dos/dos.h>
#include <utility/utility.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>

/*
    This one does magic things with some types and must come after all other
    includes
*/
#include "aroslistview_intern.h"

#undef SysBase

/* Customize libheader.c */
#define LC_SYSBASE_FIELD(lib)   (((LIBBASETYPEPTR       )(lib))->sysbase)
#define LC_SEGLIST_FIELD(lib)   (((LIBBASETYPEPTR       )(lib))->seglist)
#define LC_LIBBASESIZE          sizeof(LIBBASETYPE)
#define LC_LIBHEADERTYPEPTR     LIBBASETYPEPTR
#define LC_LIB_FIELD(lib)       (((LIBBASETYPEPTR)(lib))->library)

#define LC_NO_OPENLIB
#define LC_NO_CLOSELIB

#include <libcore/libheader.c>

#undef  SDEBUG
#undef  DEBUG
#define DEBUG 0
#include <aros/debug.h>

/* #define SysBase      (LC_SYSBASE_FIELD(lh)) */

/* Global IntuitionBase */
#ifdef GLOBAL_INTUIBASE
struct IntuitionBase *IntuitionBase;
#endif

/* Global SysBase */
struct ExecBase *SysBase;

#define AROSListviewBase lh


/* Predeclaration */
struct IClass *InitListviewClass(struct LVBase_intern *);

ULONG SAVEDS STDARGS LC_BUILDNAME(L_InitLib) (LC_LIBHEADERTYPEPTR lh)
{

    SysBase = LC_SYSBASE_FIELD(lh);
    
    if (!GfxBase)
	GfxBase = (GraphicsBase *)OpenLibrary(GRAPHICSNAME, 37);
    if (!GfxBase)
	return(NULL);

    if (!UtilityBase)
	UtilityBase = OpenLibrary(UTILITYNAME, 37);
    if (!UtilityBase)
	return(NULL);

    if (!IntuitionBase)
	IntuitionBase = (IntuiBase *)OpenLibrary(INTUITIONNAME, 37);
    if (!IntuitionBase)
	return (NULL);

    if (!DOSBase)
	DOSBase = OpenLibrary(DOSNAME, 37);
    if (!DOSBase)
	return (NULL);

    /* ------------------------- */
    /* Create the class itself */
    
    if (!lh->classptr)
    	lh->classptr = InitListviewClass(lh);
    if (!lh->classptr)
	return (NULL);


    /* ------------------------- */


    return TRUE;

}

VOID SAVEDS STDARGS LC_BUILDNAME(L_ExpungeLib) (LC_LIBHEADERTYPEPTR lh)
{
	
    if (lh->classptr)
    {
	RemoveClass(lh->classptr);
	FreeClass(lh->classptr);
	lh->classptr = NULL;
    }

    CloseLibrary(UtilityBase);
    UtilityBase = NULL;

    CloseLibrary((struct Library *)GfxBase);
    GfxBase = NULL;

    CloseLibrary((struct Library *)IntuitionBase);
    IntuitionBase = NULL;

    CloseLibrary(DOSBase);
    DOSBase = NULL;

    return;

}

