/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "coolimages.h"

#define USEIMAGE_WIDTH  13
#define USEIMAGE_HEIGHT 16
#define USEIMAGE_COLORS 3

static const UBYTE useimage_data[] =
{
	00,00,00,00,00,00,00,00,00,02,02,02,00,
	00,00,00,00,00,00,00,00,02,02,02,02,01,
	00,00,00,00,00,00,00,02,02,02,02,02,01,
	00,00,00,00,00,00,00,02,02,02,02,01,00,
	00,00,00,00,00,00,02,02,02,02,01,00,00,
	00,00,00,00,00,00,02,02,02,01,00,00,00,
	00,02,02,00,00,00,02,02,02,01,00,00,00,
	02,02,02,02,00,02,02,02,01,00,00,00,00,
	02,02,02,02,02,02,02,02,01,00,00,00,00,
	02,02,02,02,02,02,02,02,01,00,00,00,00,
	00,02,02,02,02,02,02,01,00,00,00,00,00,
	00,00,02,02,02,02,02,01,00,00,00,00,00,
	00,00,02,02,02,02,02,01,00,00,00,00,00,
	00,00,00,02,02,02,01,00,00,00,00,00,00,
	00,00,00,02,02,02,01,00,00,00,00,00,00,
	00,00,00,00,02,01,00,00,00,00,00,00,00
};

static const UBYTE useimage_pal[] =
{
	0xb3,0xb3,0xb3,0x00,0x00,0x00,
	0x00,0x76,0x00
};

const struct CoolImage cool_useimage =
{
	useimage_data,
	useimage_pal,
	USEIMAGE_WIDTH,
	USEIMAGE_HEIGHT,
	USEIMAGE_COLORS
};
