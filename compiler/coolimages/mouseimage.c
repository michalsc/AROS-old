/*
    Copyright (C) 2000-2001 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: english
*/

#include "coolimages.h"

#define MOUSEIMAGE_WIDTH     15
#define MOUSEIMAGE_HEIGHT    15
#define MOUSEIMAGE_COLORS    16

static const UBYTE mouseimage_data[] =
{
	0,0,0,0,0,0,12,13,10,13,5,9,2,6,0,
	0,0,0,0,9,10,4,12,5,4,13,2,2,0,0,
	0,0,0,9,13,12,12,12,12,7,2,1,7,9,0,
	0,0,9,13,12,12,6,6,10,7,4,1,4,2,9,
	0,0,1,11,3,14,6,11,6,12,4,4,15,13,8,
	0,11,4,3,14,14,14,3,8,12,5,5,10,13,9,
	0,10,6,14,14,14,6,8,11,5,5,4,10,13,8,
	9,4,3,14,14,3,3,12,4,5,1,10,13,11,0,
	5,12,6,3,3,6,11,12,5,5,10,10,10,0,0,
	1,12,11,6,11,11,12,15,5,5,1,7,9,0,0,
	1,15,12,12,12,12,15,15,15,5,7,12,0,0,0,
	5,4,5,15,15,15,12,12,5,7,12,0,0,0,0,
	9,7,4,5,15,12,12,4,7,12,0,0,0,0,0,
	0,4,10,5,15,5,10,13,9,0,0,0,0,0,0,
	0,0,12,1,1,1,0,0,0,0,0,0,0,0,0

};

static const UBYTE mouseimage_pal[] =
{
	255,255,  0,
	42, 50, 58,
	114,114,122,
	226,226,242,
	114,126,162,
	130,142,182,
	210,210,222,
	 62, 74, 90,
	170,166,170,
	198,198,210,
	 94,102,126,
	190,190,214,
	167,168,217,
	 82, 86,102,
	238,238,242,
	138,150,202
};

const struct CoolImage cool_mouseimage =
{
	mouseimage_data,
	mouseimage_pal,
	MOUSEIMAGE_WIDTH,
	MOUSEIMAGE_HEIGHT,
	MOUSEIMAGE_COLORS
};

