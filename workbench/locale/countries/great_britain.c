/*
    Copyright (C) 1995-2001 AROS - The Amiga Research OS
    $Id$

    Desc: Country data for great britain.
    Lang:
*/

#include <exec/types.h>
#include <libraries/locale.h>
#include <libraries/iffparse.h>
#include <prefs/locale.h>

/* great_britain.country: Iain Templeton, iain@ugh.net.au */
/* Is probably wrong in a few places */
struct CountryPrefs greatBritainPrefs =
{
    /* Reserved */
    { 0, 0, 0, 0 },

    /* Country code, telephone code, measuring system */
    MAKE_ID('G','B',0,0), 44, MS_BRITISH,

    /* Date time format, date format, time format */
    "%A %e %B %Y  %H:%M",
    "%A %e %B %Y",
    "%H:%M:%S",

    /* Short datetime, short date, short time formats */
    "%d/%m/%Y %H:%M",
    "%d/%m/%Y",
    "%H:%M",

    /* Decimal point, group separator, frac group separator */
    ".", ",", "",

    /* For grouping rules, see <libraries/locale.h> */

    /* Grouping, Frac Grouping */
    { 3 }, { 0 },

    /* Mon dec pt, mon group sep, mon frac group sep */
    ".", ",", "",

    /* Mon Grouping, Mon frac grouping */
    { 3 }, { 0 },

    /* Mon Frac digits, Mon IntFrac digits, then number of digits in
       the fractional part of the money value. Most countries that
       use dollars and cents, would have 2 for this value

       (As would many of those you don't).
    */
    2, 2,

    /* Currency symbol, Small currency symbol */
    "�", "p",

    /* Int CS, this is the ISO 4217 symbol, followed by the character to
       separate that symbol from the rest of the money. (\x00 for none).
    */
    "GPB",

    /* Mon +ve sign, +ve space sep, +ve sign pos, +ve cs pos */
    "", SS_NOSPACE, SP_PREC_ALL, CSP_PRECEDES,

    /* Mon -ve sign, -ve space sep, -ve sign pos, -ve cs pos */
    "", SS_NOSPACE, SP_PARENS, CSP_PRECEDES,

    /* Calendar type */
    CT_7MON
};
