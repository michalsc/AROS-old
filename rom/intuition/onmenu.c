/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Intuition function OnMenu()
    Lang: english
*/
#include "intuition_intern.h"

/*****************************************************************************

    NAME */
#include <intuition/intuition.h>

	AROS_LH2(void, OnMenu,

/*  SYNOPSIS */
	AROS_LHA(struct Window    *, window, A0),
	AROS_LHA(UWORD             , menunumber, D0),

/*  LOCATION */
	struct IntuitionBase *, IntuitionBase, 32, Intuition)

/*  FUNCTION
	Enable a whole menu, an item or a sub-item depending on
	the menunumber.

    INPUTS
	window - The window, the menu belongs to
	menunumber - The packed information on what piece of menu to enable

    RESULT
	None.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
	OffMenu(), ResetMenuStrip()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct IntuitionBase *,IntuitionBase)
int i;
struct Menu * thismenu;
struct MenuItem * thisitem;

    thismenu = window->MenuStrip;
    for ( i = 0 ; i < MENUNUM ( menunumber ) ; i++ )
    {
	thismenu = thismenu->NextMenu;
    }
    if ( ITEMNUM ( menunumber ) == NOITEM )
    {
	thismenu->Flags |= MENUENABLED;
    }
    else
    {
	thisitem = thismenu->FirstItem;
	for ( i = 0 ; i < ITEMNUM ( menunumber ) ; i++ )
	{
	    thisitem = thisitem->NextItem;
	}
	if ( SUBNUM ( menunumber ) != NOSUB )
	{
	    thisitem = thisitem->SubItem;
	    for ( i = 0 ; i < SUBNUM ( menunumber ) ; i++ )
	    {
		thisitem = thisitem->NextItem;
	    }
	}
	thisitem->Flags |= ITEMENABLED;
    }

    AROS_LIBFUNC_EXIT
} /* OnMenu */
