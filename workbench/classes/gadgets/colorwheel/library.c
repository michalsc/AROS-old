
/*
**
**	library code colorwheel
**	AMIGA version
**
*/

#include <exec/resident.h>
#include <exec/initializers.h>
#include <exec/execbase.h>
#include <exec/libraries.h>
#include <graphics/gfxbase.h>

#ifdef __GNUC__
#include <inline/exec.h>
#include <inline/intuition.h>
#include <inline/dos.h>
#else
#include <pragma/exec_lib.h>
#include <pragma/intuition_lib.h>
#include <pragma/dos_lib.h>

#define REG( x, a )	register __## x a
#define __regargs
#endif

#ifdef __GNUC__
#include "BoopsiStubs.h"
#endif

#define ColorWheelBase	ColorWheelBase_intern

#include "colorwheel_intern.h"

/****************************************************************************/

#ifndef BPTR
#define BPTR		ULONG
#endif

/****************************************************************************/

STATIC struct Library *LibInit(REG(a0, ULONG seglist), REG(d0, struct ColorWheelBase_intern *ClassBase), REG(a6, struct Library *) );
STATIC struct Library *LibOpen( REG(a6, struct ColorWheelBase_intern *ClassBase ) );
STATIC ULONG LibExpunge( REG(a6, struct ColorWheelBase_intern *ClassBase ) );
STATIC ULONG LibClose( REG(a6, struct ColorWheelBase_intern *ClassBase ) );
STATIC LONG LibExtFunc(void);


BOOL __regargs L_OpenLibs(struct ColorWheelBase_intern *);
void __regargs L_CloseLibs(struct ColorWheelBase_intern *);
Class * __regargs initClass (struct ColorWheelBase_intern *);
Class  * ObtainClass(REG(a6, struct ColorWheelBase_intern *) );

extern ULONG   dispatch_colorwheelclass(REG(a0, Class *), REG(a2, Object *), REG(a1, Msg ) );

/****************************************************************************/

UBYTE	LibName[] = NAME_STRING,
		LibID[] = VERSION_STRING;

LONG	LibVersion = VERSION_NUMBER,
		LibRevision = REVISION_NUMBER;

extern void ConvertHSBToRGB( void ), ConvertRGBToHSB( void );

APTR LibVectors[] =
{
	(APTR) LibOpen,
	(APTR) LibClose,
	(APTR) LibExpunge,
	(APTR) LibExtFunc,
	(APTR) ConvertHSBToRGB,
	(APTR) ConvertRGBToHSB,
	(APTR) -1L
};

ULONG LibInitTab[] =
{
	(ULONG) sizeof(struct ColorWheelBase_intern),
	(ULONG) LibVectors,
	(ULONG) NULL,
	(ULONG) LibInit
};

struct Resident ROMTag =     /* do not change */
{
	RTC_MATCHWORD,
 	&ROMTag,
 	&ROMTag + sizeof(ROMTag),
	RTF_AUTOINIT,
 	VERSION_NUMBER,
	NT_LIBRARY,
 	0,
	LibName,
	LibID,
	(APTR)&LibInitTab
};

/****************************************************************************/

LONG LibExtFunc(void)
{
	return(-1L);
}

/****************************************************************************/

struct Library *
  __saveds LibInit( REG(a0, ULONG seglist), REG(d0, struct ColorWheelBase_intern *ColorWheelBase), REG(a6, struct Library *ExecBase ) )
{
	SysBase = (struct ExecBase *) ExecBase;
	ColorWheelBase->seglist = seglist;
	ColorWheelBase->library.lib_Revision = REVISION_NUMBER;
	
	if( ( ExecBase->lib_Version >= 39L ) && ( ((struct ExecBase *)SysBase)->AttnFlags & AFF_68020 ) )
	{
		if(L_OpenLibs(ColorWheelBase))
		{	
			if(ColorWheelBase->classptr = InitColorWheelClass(ColorWheelBase))
			{			
				return((struct Library *)ColorWheelBase);
			}
		}
		
		L_CloseLibs( ColorWheelBase );
	}

	FreeMem((BYTE *)ColorWheelBase-ColorWheelBase->library.lib_NegSize, 
		ColorWheelBase->library.lib_NegSize + ColorWheelBase->library.lib_PosSize);
	
	return(NULL);
}

/****************************************************************************/

struct Library * __saveds  LibOpen(REG(a6, struct ColorWheelBase_intern *ColorWheelBase ) )
{	
	ColorWheelBase->library.lib_Flags &= ~LIBF_DELEXP;
	ColorWheelBase->library.lib_OpenCnt++;

	return((struct Library *)ColorWheelBase);
}

/****************************************************************************/

ULONG __saveds LibExpunge(REG(a6, struct ColorWheelBase_intern *ColorWheelBase) )
{
 	if(!ColorWheelBase->library.lib_OpenCnt) 
 	{
		if(ColorWheelBase->classptr) 
		{
			if( ! FreeClass(ColorWheelBase->classptr) )
			{
				AddClass( ColorWheelBase->classptr );
				return NULL;
			}
		}
		
		L_CloseLibs(ColorWheelBase);
		
 		Remove((struct Node *)ColorWheelBase);

  		FreeMem((BYTE *)ColorWheelBase-ColorWheelBase->library.lib_NegSize,
	          ColorWheelBase->library.lib_NegSize + ColorWheelBase->library.lib_PosSize);

		return( ColorWheelBase->seglist );
 	} 
 	else 
	{
		ColorWheelBase->library.lib_Flags |= LIBF_DELEXP;
	}

 	return( NULL );
}

/****************************************************************************/

ULONG __saveds  LibClose(REG(a6, struct ColorWheelBase_intern *ColorWheelBase) )
{		
 	if(ColorWheelBase->library.lib_OpenCnt) 
	{
		ColorWheelBase->library.lib_OpenCnt--;
	}

 	return((!ColorWheelBase->library.lib_OpenCnt &&
        	 ColorWheelBase->library.lib_Flags & LIBF_DELEXP)?LibExpunge(ColorWheelBase):NULL);
}

/****************************************************************************/

BOOL __regargs L_OpenLibs( struct ColorWheelBase_intern *ColorWheelBase )
{
	if(	( GfxBase = OpenLibrary( "graphics.library", 39L ) ) &&
		( IntuitionBase = OpenLibrary( "intuition.library", 39L ) ) &&
		( LayersBase = OpenLibrary( "layers.library", 39L ) ) &&
		( UtilityBase = OpenLibrary( "utility.library", 39L ) )	)
	{	
		CyberGfxBase = OpenLibrary( "cybergraphics.library", 40L );
	
		return( TRUE );
	}
	
	return FALSE;
}

/****************************************************************************/

void __regargs L_CloseLibs( struct ColorWheelBase_intern *ColorWheelBase )
{
	CloseLibrary( (struct Library *)GfxBase );
	CloseLibrary( (struct Library *)IntuitionBase );
	CloseLibrary( UtilityBase );
	CloseLibrary( CyberGfxBase );
	CloseLibrary( LayersBase );
}

/****************************************************************************/
