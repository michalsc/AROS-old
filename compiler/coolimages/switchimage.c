/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: english
*/

#include "coolimages.h"

#define SWITCHIMAGE_WIDTH   18
#define SWITCHIMAGE_HEIGHT  16
#define SWITCHIMAGE_COLORS  4

static const UBYTE switchimage_data[] =
{
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
	00,00,00,00,00,00,02,02,02,02,00,00,00,00,00,00,00,00,
	00,00,00,00,00,02,02,02,02,02,01,00,00,00,00,00,00,00,
	00,00,00,00,02,02,02,02,02,02,02,01,00,00,03,01,00,00,
	00,00,00,00,02,02,02,01,00,02,02,02,01,03,03,03,01,00,
	00,00,00,02,02,02,01,00,00,00,02,03,03,03,03,03,01,00,
	02,02,00,02,02,01,00,00,00,00,03,03,03,03,03,03,03,01,
	02,02,02,02,02,01,00,00,00,00,03,03,03,03,03,03,03,01,
	02,02,02,02,02,02,02,02,00,00,00,00,03,03,03,01,03,01,
	02,02,02,02,02,02,02,01,00,00,00,00,03,03,01,00,01,01,
	00,02,02,02,02,01,01,03,00,00,00,03,03,03,01,00,00,00,
	00,02,02,02,01,03,03,03,03,00,03,03,03,01,00,00,00,00,
	00,00,02,01,00,00,01,03,03,03,03,03,03,01,00,00,00,00,
	00,00,00,00,00,00,00,01,03,03,03,03,01,00,00,00,00,00,
	00,00,00,00,00,00,00,00,01,01,01,01,00,00,00,00,00,00,
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00
};

static const UBYTE switchimage_pal[] =
{
	0xb3,0xb3,0xb3,0x00,0x00,0x00,
	0xdb,0x20,0x26,0x00,0x76,0x00
};

const struct CoolImage cool_switchimage =
{
	switchimage_data,
	switchimage_pal,
	SWITCHIMAGE_WIDTH,
	SWITCHIMAGE_HEIGHT,
	SWITCHIMAGE_COLORS
};
