/*
    (C) 2000-2001 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: English
*/
#include "datatypes_intern.h"
#include <datatypes/datatypesclass.h>
#include <utility/tagitem.h>
#include <dos/dostags.h>
#include <proto/alib.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>

/* Putchar procedure needed by RawDoFmt() */

AROS_UFH2(void, dt_putchr,
	  AROS_UFHA(UBYTE,    chr, D0),
	  AROS_UFHA(STRPTR *, p,   A3))
{
    *(*p)++ = chr;
}

void dt__sprintf(struct Library *DataTypesBase, UBYTE *buffer,
		 UBYTE *format, ...)
{
    RawDoFmt(format, &format+1, (VOID_FUNC)dt_putchr, &buffer);
}


/*****************************************************************************

    NAME */
#include <proto/datatypes.h>

	AROS_LH3(ULONG, LaunchToolA,

/*  SYNOPSIS */
	AROS_LHA(struct Tool *   , tool,    A0),
        AROS_LHA(STRPTR          , project, A1),
	AROS_LHA(struct TagItem *, attrs,   A2),

/*  LOCATION */
	struct Library *, DataTypesBase, 42, DataTypes)

/*  FUNCTION

    Launch an application with a particular project.

    INPUTS

    tool     --  tool to use (may be NULL in which case this function
                 returns 0)
    project  --  name of the project to execute or NULL
    attrs    --  additional attributes

    TAGS

    NP_Priority (BYTE) -- priority of the launched tool (default is the
                          priority of the currect process except for
			  Workbench applications where the default priority
			  is 0 if not overridden by the TOOLPRI tooltype).
			  
    NP_Synchronous (BOOL) -- don't return until lauched application process
                             finishes (defaults to FALSE).

    RESULT

    Zero for failure, non-zero otherwise.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    NewDTObjectA()

    INTERNALS

    HISTORY

    SDuvan  17.12.2000  --  basic implementation

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    BOOL isSynchronous;
    BPTR output;

    if (tool == NULL)
    {
	SetIoErr(ERROR_REQUIRED_ARG_MISSING);
	
	return 0;
    }

    isSynchronous = (BOOL)GetTagData(NP_Synchronous, (IPTR)FALSE, attrs);

    switch (tool->tn_Flags & TF_LAUNCH_MASK)
    {
    case TF_SHELL:
	{
	    char tBuffer[512];
	    LONG ret;

	    dt__sprintf(DataTypesBase, tBuffer,
			"%s \"%s\"", tool->tn_Program, project);

	    output = Open("CON:////Output window/AUTO/WAIT/CLOSE/INACTIVE",
			  MODE_NEWFILE);

	    if (output != NULL)
	    {
		struct TagItem tags[] = { { SYS_Asynch, !isSynchronous },
					  { SYS_Input , NULL           },
					  { SYS_Output, (IPTR)output   },
					  { TAG_DONE, NULL             } };


		ret = SystemTagList(tBuffer, tags);

		/* Error? */
		if (ret == -1)
		{
		    return 0;
		}

		Close(output);
	    }
	    else
	    {
		return 0;
	    }
	}

	break;

    case TF_WORKBENCH:
#warning WBlib not finished
	/*
	if (!OpenWorkbenchObject(tool->tn_Program, WBOPENA_ArgName, project,
	                         TAG_DONE))
	*/
	{
	    return 0;
	}
	
	break;

    case TF_RX:
	/* Sorry, no Arexx in AROS yet. */
#warning No Arexx compatibility yet

	/* Do some "RX command" here */
	return 0;
	break;

    default:
	return 0;
	break;
    }

    return 1;
    
    AROS_LIBFUNC_EXIT
} /* LaunchToolA */

