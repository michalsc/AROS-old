/*
    (C) 1997-98 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: English
*/

/*****************************************************************************

    NAME */

#include "cxintern.h"
#include <exec/lists.h>
#include <proto/exec.h>
#include <proto/commodities.h>

VOID RecRem(CxObj *, struct Library *CxBase);

    AROS_LH1(VOID, DeleteCxObjAll,

/*  SYNOPSIS */

	AROS_LHA(CxObj *, co, A0),

/*  LOCATION */

	struct Library *, CxBase, 9, Commodities)

/*  FUNCTION

    Delete object and and all objects connected to commodity object 'co'.
    Handy for instances like when you are shutting down your commodity.
    To remove your commodity tree, just DeleteCxObjAll(YourBroker).

    INPUTS

    co  --  the object in question (may be NULL)

    RESULT

    NOTES

    The handle 'co' is invalid after the operation.

    EXAMPLE

    BUGS

    SEE ALSO

    DeleteCxObj()

    INTERNALS

    HISTORY

******************************************************************************/

{
    AROS_LIBFUNC_INIT

    if(co == NULL)
	return;

    RemoveCxObj(co);
    RecRem((CxObj *)co->co_ObjList.mlh_Tail, CxBase);
    FreeCxStructure(co, CX_OBJECT, CxBase);

    AROS_LIBFUNC_EXIT
} /* DeleteCxObjAll */


VOID RecRem(CxObj *obj, struct Library *CxBase)
{
    while(obj != NULL)
    {
	FreeCxStructure(obj->co_ObjList.mlh_Tail, CX_OBJECT, CxBase);
	obj = (CxObj *)RemHead((struct List *)&obj->co_ObjList);
    }
}
