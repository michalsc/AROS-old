/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Returns a pointer to the first char of the filename in the given
	  file part.
    Lang: english
*/
#ifndef TEST
#    include "dos_intern.h"
#else
#    define AROS_LH1(t,fn,a1,bt,bn,o,lib)     t fn (a1)
#    define AROS_LHA(t,n,r)                   t n
#    define AROS_LIBFUNC_INIT
#    define AROS_LIBBASE_EXT_DECL(bt,bn)
#    define AROS_LIBFUNC_EXIT
#    include <exec/types.h>
#    define CLIB_DOS_PROTOS_H
#endif


/*****************************************************************************

    NAME */
#include <proto/dos.h>

	AROS_LH1(STRPTR, FilePart,

/*  SYNOPSIS */
	AROS_LHA(STRPTR, path, D1),

/*  LOCATION */
	struct DosLibrary *, DOSBase, 145, Dos)

/*  FUNCTION
	Get a pointer to the last component of a path, which is normally the
	filename.

    INPUTS
	path - pointer AmigaDOS path string
	    May be relative to the current directory or the current disk.

    RESULT
	A pointer to the first char of the filename!

    NOTES

    EXAMPLE
	FilePart("xxx:yyy/zzz/qqq") returns a pointer to the first 'q'.
	FilePart("xxx:yyy")         returns a pointer to the first 'y'.
	FilePart("yyy")             returns a pointer to the first 'y'.

    BUGS
	None known.

    SEE ALSO
	PathPart(), AddPart()

    INTERNALS
	Goes from the last char of the pathname back until it finds a ':',
	a '/' or until the first char reached.

    HISTORY
	29-10-95    digulla automatically created from
			    dos_lib.fd and clib/dos_protos.h

	04-08-96    steigerwald hopefully filled up with something useful
				;-), however untested!

	07-08-96    steigerwald reworked code, implented digulla's
				suggestions, thanks Aaron ;-)

				added some documentation ;-)

				converted all comments in function to
				c++ style to avoid nested comments

				again untested, cause too much AROS stuff
				that is not easy to #ifdef out missing

	20-08-96    steigerwald finally added all those #ifndef NO_AROS
				to get this thing working stand-alone
				test routine added
				some bugs fixed

				problem: see while and ifs below ;-(((

				routine seems to work so far, but doesnt
				check for path consistency so
				FilePart("dh0:test/exec:now") will give a
				pointer to "now" ;-)

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct DosLibrary *,DOSBase)

    if(path)
    {
	STRPTR i;

	/* set i to last char of path */

	if (!*path) /* path == "" ? */
	  return path;

	i = path + strlen (path) -1;   /* set i to the \0-byte */

	/* decrease pointer as long as there is no ':', no '/' or till
	  the first char anyway. hope this works in all situations */
	while ((*i != ':') && (*i != '/') && (i != path))
	    i--;

	if ((*i == ':')) i++;
	if ((*i == '/')) i++;

	return(i);
    } /* path */

    return (0L);  /* if no path is given return NIL pointer */

    AROS_LIBFUNC_EXIT
} /* FilePart */

#ifdef TEST

#    include <stdio.h>

int main (int argc, char ** argv)
{
    UWORD i;
    STRPTR s,fileptr;

    while (--argc)
    {
	s = *++argv;
	fileptr = FilePart(s);

	printf("Pfad:  %s\nDatei: %s\n", s, fileptr);
    }
}

#endif /* TEST */
