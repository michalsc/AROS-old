/*
    Copyright (C) 1995-2000 AROS - The Amiga Research OS
    $Id$

    Desc: BOOPSI functions DoMethodA() and DoMethod()
    Lang: english
*/
#include <intuition/classes.h>
#include <proto/alib.h>
#include "alib_intern.h"

/******************************************************************************

    NAME */
#include <intuition/classusr.h>
#include <proto/intuition.h>

	IPTR DoMethodA (

/*  SYNOPSIS */
	Object * obj,
	Msg	 message)

/*  FUNCTION
	Invokes a method on a BOOPSI object. The dispatcher of the class, the
	object is inherited from, is called. For more information about methods
	a class supports, see the class documentation.

    INPUTS
	obj - The object, on which the method is to be performed on.
	message - The message. The first field is the same for all methods and
		  specifies which method is to be invokes (see
		  <intuition/classusr.h>).

    RESULT
	Class and method depending. See the class documentation. A value of 0
	can mean a valid return code but can also mean that a method was not
	supported.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
	NewObjectA(), SetAttrsA(), GetAttr(), DisposeObject(), CoerceMethodA(),
        DoSuperMethodA(), <intuition/classusr.h>

    HISTORY
	14.09.93    ada created

******************************************************************************/
{
    if (!obj)
	return 0L;
    return (CallHookPkt ((struct Hook *)OCLASS(obj), obj, message));
} /* DoMethodA */

ULONG DoMethod (Object * obj, ULONG MethodID, ...)
{
    AROS_SLOWSTACKMETHODS_PRE(MethodID)
    if (!obj)
	retval = 0L;
    else
	retval = CallHookPkt ((struct Hook *)OCLASS(obj)
	    , obj
	    , AROS_SLOWSTACKMETHODS_ARG(MethodID)
        );
    AROS_SLOWSTACKMETHODS_POST
} /* DoMethod */

