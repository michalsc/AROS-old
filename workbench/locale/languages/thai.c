/*
    Copyright � 1995-2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc: thai.language description file.
    Char: ISO 8859-11
*/

#define LANGSTR     "thai"              /* String version of above      */
#define NLANGSTR    "thai"              /* Native version of LANGSTR    */
#define LANGVER     41                  /* Version number of language   */
#define LANGREV     2                   /* Revision number of language  */
#define LANGTAG     "\0$VER: " LANGSTR ".language 41.2 (30.09.2014)"
#define NLANGTAG    "$NLANG:" NLANGSTR

#include "lang_openclose.inc"
#include "lang_getlangstring.inc"

/* -----------------------------------------------------------------------
   Library function table - you will need to alter this
   I have this right here at the end of the library so that I do not
   have to have prototypes for the functions. Although you could do that.
   ----------------------------------------------------------------------- */

void *const functable[] =
{
    &AROS_SLIB_ENTRY(open,language,1),
    &AROS_SLIB_ENTRY(close,language,2),
    &AROS_SLIB_ENTRY(expunge,language,3),
    &AROS_SLIB_ENTRY(null,language,0),
    &AROS_SLIB_ENTRY(mask,language,5),

    /* Note, shorter function table, as only getlangstring is used */

    /* 0 - 3 */
    &AROS_SLIB_ENTRY(null, language, 0),
    &AROS_SLIB_ENTRY(null, language, 0),
    &AROS_SLIB_ENTRY(null, language, 0),    
    &AROS_SLIB_ENTRY(getlangstring, language, 9),
    (void *)-1
};

/*
    Note how only the required data structures are kept...

    This is the list of strings. It is an array of pointers to strings,
    although how it is laid out is implementation dependant.
*/
const STRPTR ___strings[] =
{
    /* A blank string */
    "",

    /*  The days of the week. Starts with the first day of the week.
	In English this would be Sunday, this depends upon the settings
	of Locale->CalendarType.
    */

    // NOTICE: stegerg: I think this must always start with Sunday and not what comment above says

    "�ѹ�ҷԵ��", "�ѹ�ѹ���", "�ѹ�ѧ���", "�ѹ�ظ", "�ѹ����ʺ��",
    "�ѹ�ء��", "�ѹ�����",

    /* Abbreviated days of the week */
    "��.", "�.", "�.", "�.", "��.", "�.", "�.",

    /* Months of the year */
    "���Ҥ�", "����Ҿѹ��", "�չҤ�",
    "����¹", "����Ҥ�", "�Զع�¹",
    "�á�Ҥ�", "�ԧ�Ҥ�", "�ѹ��¹",
    "���Ҥ�", "��Ȩԡ�¹", "�ѹ�Ҥ�",

    /* Abbreviated months of the year */
    "�.�.", "�.�.", "��.�.", "��.�.", "�.�.", "��.�.",
    "�.�.", "�.�.", "�.�.", "�.�.", "�.�.", "�.�.",

    "��", /* Yes, affirmative response */
    "���", /* No/negative response */

    /* AM/PM strings AM 0000 -> 1159, PM 1200 -> 2359 */
    "AM", "PM",

    /* Soft and hard hyphens */
    "-", "-",

    /* Open and close quotes */
    "\"", "\"",

    /* Days: But not actual day names
       Yesterday - the day before the current
       Today - the current day
       Tomorrow - the next day
       Future.
    */
    "������ҹ���", "�ѹ���", "�ѹ���觹��", "͹Ҥ�",

    /* Native language name */
    NLANGSTR
};

/* This is the end of ROMtag marker. */
const char end=0;
