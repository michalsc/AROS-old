/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "global.h"

IPTR argArray[NUM_ARGS];
struct RDArgs *readArgs;

// Grab any shell arguments given by the user
struct RDArgs * getArguments(void)
{
    UBYTE a;
    struct RDArgs *tmpReadArgs;

    // Let's clear the array. Do we really need this or will ReadArgs() set
    // every non given argument pointer to NULL? Check it up!
    // yes, you need it :) [by falemagn]

    for(a = 0; a < NUM_ARGS; a++)
	argArray[a] = NULL;

    // We need an AROS "IPTR" class pointer here! An IPTR is guaranteed to be AmigaOS "ULONG" compatible?

    if((tmpReadArgs = ReadArgs("FROM,EDIT/S,USE/S,SAVE/S,PUBSCREEN/K", argArray, NULL)))
    {
	for(a = 0; a < NUM_ARGS; a++)
	{
	    if(argArray[a])
	    {
		kprintf("%d is set!\n", a);

		/* ARG_PUBSCREEN is set (currently) set to NULL by Fonts Preferences.
		   If not set, it'll default to Workbench (NULL). If we can't open
		   the specified screen, should we try the Workbench screen instead? */

		if(a == ARG_PUBSCREEN)
		    kprintf("pubscreen is %s!\n", (char *)argArray[ARG_PUBSCREEN]);
	    }
	}

	return(tmpReadArgs);
    }
    else
	return(NULL);
}

// Deal with shell arguments. If failure, should we quit or not? If started
// from shell, do we assume the user wants to run Font Preferences on a non
// (GUI) interactive basis? Request for comments! (petah)
UBYTE processArguments(void)
{
    extern struct FontPrefs *fontPrefs[3];

    if(!(readArgs = getArguments()))
	PrintFault(IoErr(), NULL);

 /* If FROM is set, then also check for the USE and SAVE keywords - but only then. There isn't
    any point in just replacing the same settings with the very same values? */

    if(argArray[ARG_FROM])
    {
	if(!(readIFF((UBYTE *)argArray[ARG_FROM], fontPrefs)))
	    return(APP_FAIL);

     /* If USE or SAVE is set, write the FROM file to ENV: and/or ENVARC: and then quit. Is this
	what the "Classic" Font Preferences does? Look it up! (As a side note, if FILE is not
	found, the old settings will be overwritten with default values. Should we avoid this and
	implement some error checking in writeIFF() ? What if FROM is not set? Should we still 
	react for USE and SAVE (which we currently don't)? Request for comments to author! */

	if(argArray[ARG_USE] || argArray[ARG_SAVE])
	{
	    if(!(writeIFF("ENV:sys/font.prefs", fontPrefs)))
		return(APP_FAIL);

	    if(argArray[ARG_SAVE])
		if(!(writeIFF("ENVARC:sys/font.prefs", fontPrefs)))
		    return(APP_FAIL);

	    // Don't launch the rest of the program, just exit
	    return(APP_STOP);
	}
    }
    else
	if(!(readIFF("ENV:sys/font.prefs", fontPrefs)))
	    return(APP_FAIL);

    // What is "EDIT" supposed to do? Look it up!
    if(argArray[ARG_EDIT])
	kprintf("EDIT keyword set!\n");

    return(APP_RUN);
}
