/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$

    Desc: ANSI C function strncat()
    Lang: english
*/

/*****************************************************************************

    NAME */
#include <string.h>

	char * strncat (

/*  SYNOPSIS */
	char	   * dest,
	const char * src,
	size_t	     n)

/*  FUNCTION
	Concatenates two strings. If src is longer than n characters, then
	only the first n characters are copied.

    INPUTS
	dest - src is appended to this string. Make sure that there
		is enough room for src.
	src - This string is appended to dest
	n - No more than this number of characters of src are copied.

    RESULT
	dest.

    NOTES
	The routine makes no checks if dest is large enough. The size of
	dest must be at least strlen(dest)+n+1.

    EXAMPLE
	char buffer[64];

	strcpy (buffer, "Hello ");
	strncat (buffer, "World.!!", 6);

	// Buffer now contains "Hello World."

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

******************************************************************************/
{
    char * d = dest;

    while (*dest)
	dest ++;

    while (n && (*dest ++ = *src ++))
	n --;

    return d;
} /* strncat */
