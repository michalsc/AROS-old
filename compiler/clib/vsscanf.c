/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$

    Desc: C function vsscanf()
    Lang: english
*/
/* Original source from libnix */
#define AROS_ALMOST_COMPATIBLE
#include <stdio.h>

static int _vsscanf_get (char ** str)
{
    if (!**str)
	return EOF;

    return *(*str)++;
}

static int _vsscanf_unget (int c, char ** str)
{
    (*str)--;

    return c;
}

/*****************************************************************************

    NAME */
	#include <stdio.h>
#include <stdarg.h>

	int vsscanf (

/*  SYNOPSIS */
	char	   * str,
	const char * format,
	va_list      args)

/*  FUNCTION
	Scan a string and convert it into the arguments as specified
	by format.

    INPUTS
	str - Scan this string
	format - A scanf() format string.
	args - A list of arguments for the results

    RESULT
	The number of arguments converted.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
	scanf(), sscanf(), fscanf(), vscanf(), vfscanf(), snscanf(),
	vsnscanf()

    INTERNALS

    HISTORY
	28.01.1997 digulla created

******************************************************************************/
{
    int rc;

    rc = __vcscan (&str,
	    (void *)_vsscanf_get,
	    (void *)_vsscanf_unget,
	    format,
	    args
    );

    *str = 0;

    return rc;
} /* vsscanf */
