/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc: ANSI C function sprintf()
    Lang: english
*/

#define _LIBC_KERNEL_
#define sprintf sprintf

/*****************************************************************************

    NAME */
#include <stdio.h>

	int sprintf (

/*  SYNOPSIS */
	char	   * str,
	const char * format,
	...)

/*  FUNCTION
	Formats a list of arguments and writes them into the string str.

    INPUTS
	str - The formatted string is written into this variable. You
		must make sure that it is large enough to contain the
		result.
	format - Format string as described above
	... - Arguments for the format string

    RESULT
	The number of characters written into the string.

    NOTES
	No checks are made that str is large enough for the result.

    EXAMPLE

    BUGS

    SEE ALSO
	fprintf(), vprintf(), vfprintf(), snprintf(), vsprintf(),
	vnsprintf()

    INTERNALS

    HISTORY
	11.12.1996 digulla created

******************************************************************************/
{
    int     retval;
    va_list args;

    va_start (args, format);

    retval = vsprintf (str, format, args);

    va_end (args);

    return retval;
} /* sprintf */

