/*
    Copyright � 2003, The AROS Development Team. All rights reserved.
    $Id$
    
    Builtin icon images to fall back upon if on-disk icons cannot be opened.
    Icons contributed by Martin "Mason" Merz.
*/

#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <proto/icon.h>

#include "icon_intern.h"

#define ICON_WIDTH  (32)
#define ICON_HEIGHT (32)
#define ICON_DEPTH  (32)

static const UBYTE disk_data_1[] =
{
    0x00,0x00,0x00,0x00,0x77,0x00,0x00,0xbc,0x77,0x00,0x3c,0xbe,0x77,0x00,0x3c,0xbf,
    0x77,0x00,0x3c,0xbf,0x77,0x00,0x3c,0xbf,0x77,0x00,0x3c,0xbf,0x77,0x00,0x3c,0xbf,
    0x77,0x00,0x3c,0xbf,0x77,0x00,0x3c,0xbf,0x77,0x00,0x00,0xbf,0x77,0x00,0x00,0xbf,
    0x77,0x7f,0xff,0xbf,0x70,0x00,0x00,0x7f,0x7f,0xff,0xff,0xff,0x7f,0xff,0xff,0xff,
    0x7f,0xff,0xff,0xff,0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,
    0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,
    0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,
    0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,0x7f,0xff,0xff,0xfe,
    0xff,0xff,0xff,0xf0,0xe7,0x80,0x00,0x38,0xe7,0x80,0x00,0x3c,0xe7,0x80,0x1e,0x3d,
    0xe7,0x80,0x1e,0x3d,0xe7,0x80,0x1e,0x3d,0xe7,0x80,0x1e,0x3d,0xe7,0x80,0x1e,0x3d,
    0xe7,0x80,0x1e,0x3d,0xe7,0x80,0x1e,0x3d,0xe7,0x80,0x1e,0x3d,0xe7,0x80,0x00,0x3d,
    0xe7,0x00,0x00,0x3d,0xff,0x80,0x00,0x7d,0xff,0xff,0xff,0xfd,0xff,0xff,0xff,0xfd,
    0xf0,0x00,0x00,0x1d,0xe0,0x00,0x00,0x0d,0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,
    0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,
    0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,
    0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,0x80,0x00,0x00,0x00
};

static const UBYTE disk_data_2[] =
{
    0x00,0x00,0x00,0x00,0x70,0x00,0x07,0xbc,0x70,0x01,0xe7,0xbe,0x70,0x01,0xe7,0xbf,
    0x70,0x01,0xe7,0xbf,0x70,0x01,0xe7,0xbf,0x70,0x01,0xe7,0xbf,0x70,0x01,0xe7,0xbf,
    0x70,0x01,0xe7,0xbf,0x70,0x01,0xe7,0xbf,0x70,0x00,0x07,0xbf,0x70,0x00,0x07,0xbf,
    0x73,0xff,0xff,0xbf,0x70,0x00,0x00,0x7f,0x7f,0xff,0xff,0xff,0x7f,0xff,0xff,0xff,
    0x7f,0xff,0xff,0xff,0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,
    0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,
    0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,
    0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,0x70,0x00,0x00,0x0f,0x7f,0xff,0xff,0xfe,
    0xff,0xff,0xff,0xf0,0xe4,0x00,0x03,0xf8,0xe4,0x00,0x03,0xfc,0xe4,0x00,0x13,0xfd,
    0xe4,0x00,0x13,0xfd,0xe4,0x00,0x13,0xfd,0xe4,0x00,0x13,0xfd,0xe4,0x00,0x13,0xfd,
    0xe4,0x00,0x13,0xfd,0xe4,0x00,0x13,0xfd,0xe4,0x00,0xf3,0xfd,0xe4,0x00,0x03,0xfd,
    0xe0,0x00,0x03,0xfd,0xfe,0x00,0x03,0xfd,0xff,0xff,0xff,0xfd,0xff,0xff,0xff,0xfd,
    0xf0,0x00,0x00,0x1d,0xe0,0x00,0x00,0x0d,0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,
    0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,
    0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,
    0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,0xe7,0xff,0xff,0xed,0x80,0x00,0x00,0x00
};

static const UBYTE drawer_data_1[] =
{
0x00,0x00,0x03,0x00,0x00,0x00,0x0f,0x80,0x00,0x00,0x3f,0x80,0x00,0x00,0xff,0xc0,
0x00,0x03,0xff,0xc0,0x00,0x0f,0xff,0xe0,0x00,0x3f,0xff,0xe0,0x00,0xff,0xff,0xf0,
0x03,0xff,0xff,0xf0,0x03,0xff,0xff,0xf8,0x01,0xff,0xff,0xf8,0x01,0xff,0xff,0xfc,
0x00,0xff,0xff,0xfc,0x00,0xff,0xff,0xfe,0x00,0x7f,0xff,0xfe,0x00,0x7f,0xff,0xff,
0x00,0x3f,0xff,0xff,0x30,0x3f,0xff,0xff,0x38,0x1f,0xff,0xff,0x1c,0x1f,0xff,0xfe,
0x0e,0x0f,0xff,0xfc,0x07,0x0f,0xff,0xf8,0x03,0x87,0xff,0xf0,0x01,0xc7,0xff,0xe0,
0x00,0xe3,0xff,0xc0,0x00,0x73,0xff,0x80,0x00,0x39,0xff,0x00,0x00,0x1d,0xfe,0x00,
0x00,0x0e,0xfc,0x00,0x00,0x07,0xf8,0x00,0x00,0x03,0xf0,0x00,0x00,0x01,0xe0,0x00,
0x00,0x00,0x03,0x00,0x00,0x00,0x0f,0x80,0x00,0x00,0x3f,0x80,0x00,0x00,0xff,0xc0,
0x00,0x03,0xff,0xc0,0x00,0x0f,0xff,0xe0,0x00,0x3f,0xff,0xe0,0x00,0xff,0xff,0xf0,
0x03,0xff,0xff,0xf0,0x03,0xff,0xff,0xf8,0x01,0xff,0xff,0xf8,0x01,0xff,0xff,0xfc,
0x00,0xff,0xff,0xfc,0x01,0xff,0xff,0xfe,0x07,0x7f,0xff,0xfe,0x1f,0xff,0xff,0xff,
0x1f,0xbf,0xff,0xff,0x3f,0xff,0xff,0xfe,0x2f,0xdf,0xff,0xfd,0x17,0xff,0xff,0xfa,
0x0b,0xef,0xff,0xf4,0x05,0xff,0xff,0xe8,0x02,0xf7,0xff,0xd0,0x01,0x7f,0xff,0xa0,
0x00,0xbb,0xff,0x40,0x00,0x5f,0xfe,0x80,0x00,0x2d,0xfd,0x00,0x00,0x17,0xfa,0x00,
0x00,0x0a,0xf4,0x00,0x00,0x05,0xe8,0x00,0x00,0x02,0xd0,0x00,0x00,0x01,0x20,0x00
};

static const UBYTE drawer_data_2[] =
{
    0x00,0x00,0x03,0x00,0x00,0x00,0x0f,0x80,0x00,0x00,0x3f,0x80,0x00,0x00,0xff,0xc0,
    0x00,0x03,0xff,0xc0,0x00,0x0f,0xff,0xe0,0x00,0x3f,0xff,0xe0,0x00,0xff,0xff,0xf0,
    0x03,0xff,0xff,0xf0,0x03,0xff,0xff,0xf8,0x01,0xff,0xff,0xf8,0x01,0xff,0xff,0xfc,
    0x00,0xff,0xff,0xfc,0x00,0xff,0xff,0xfe,0x00,0x7f,0xff,0xfe,0x00,0x7f,0xff,0xff,
    0x00,0x3f,0xff,0xff,0x30,0x3f,0xff,0xff,0x38,0x1f,0xff,0xff,0x1c,0x1f,0xff,0xfe,
    0x0e,0x0f,0xff,0xfc,0x07,0x0f,0xff,0xf8,0x03,0x87,0xff,0xf0,0x01,0xc7,0xff,0xe0,
    0x00,0xe3,0xff,0xc0,0x00,0x73,0xff,0x80,0x00,0x39,0xff,0x00,0x00,0x1d,0xfe,0x00,
    0x00,0x0e,0xfc,0x00,0x00,0x07,0xf8,0x00,0x00,0x03,0xf0,0x00,0x00,0x01,0xe0,0x00,
    0x00,0x00,0x03,0x00,0x00,0x00,0x1f,0x80,0x00,0x00,0x7f,0x80,0x00,0x01,0xff,0xc0,
    0x00,0x07,0xff,0xc0,0x00,0x1f,0xff,0xe0,0x00,0x7f,0xff,0xe0,0x01,0xff,0xff,0xf0,
    0x07,0xff,0xff,0xf0,0x1f,0xff,0xff,0xf8,0x7f,0xff,0xff,0xf8,0x7f,0xff,0xff,0xfc,
    0x3f,0xff,0xff,0xfc,0x1f,0xff,0xff,0xfe,0x0f,0x7f,0xff,0xfe,0x17,0xff,0xff,0xff,
    0x1b,0xbf,0xff,0xff,0x3d,0xff,0xff,0xfe,0x2e,0xdf,0xff,0xfd,0x17,0x7f,0xff,0xfa,
    0x0b,0xaf,0xff,0xf4,0x05,0xdf,0xff,0xe8,0x02,0xe7,0xff,0xd0,0x01,0x77,0xff,0xa0,
    0x00,0xbb,0xff,0x40,0x00,0x5f,0xfe,0x80,0x00,0x2d,0xfd,0x00,0x00,0x17,0xfa,0x00,
    0x00,0x0a,0xf4,0x00,0x00,0x05,0xe8,0x00,0x00,0x02,0xd0,0x00,0x00,0x01,0x20,0x00
};

static const UBYTE project_data_1[] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
    0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x18,
    0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x60,0x80,0x00,0x00,0xc0,0xe0,0x00,0x01,0x80,
    0x38,0x00,0x03,0x00,0x0e,0x00,0x06,0x00,0x03,0x80,0x0c,0x00,0x00,0xe0,0x18,0x00,
    0x00,0x38,0x30,0x00,0x00,0x0e,0x60,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x80,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x80,0x00,
    0x00,0x07,0xe0,0x00,0x00,0x0f,0xf8,0x00,0x00,0x1f,0xfe,0x00,0x00,0x3f,0xff,0x80,
    0x00,0x7f,0xff,0xe0,0x00,0xff,0xff,0xf8,0x01,0xff,0xff,0xfe,0x03,0xff,0xff,0xfd,
    0x07,0xff,0xff,0xf9,0x0f,0xff,0xff,0xf2,0x1f,0xff,0xff,0xe4,0x3f,0xff,0xff,0xc8,
    0x7f,0xff,0xff,0x90,0xff,0xff,0xff,0x20,0x3f,0xff,0xfe,0x40,0x8f,0xff,0xfc,0x80,
    0x23,0xff,0xf9,0x00,0x08,0xff,0xf2,0x00,0x02,0x3f,0xe4,0x00,0x00,0x8f,0xc8,0x00,
    0x00,0x23,0x90,0x00,0x00,0x08,0x20,0x00,0x00,0x02,0x40,0x00,0x00,0x00,0x80,0x00,
};

static const UBYTE project_data_2[] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
    0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x18,
    0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x60,0x80,0x00,0x00,0xc0,0xe0,0x00,0x01,0x80,
    0x38,0x00,0x03,0x00,0x0e,0x00,0x06,0x00,0x03,0x80,0x0c,0x00,0x00,0xe0,0x18,0x00,
    0x00,0x38,0x30,0x00,0x00,0x0e,0x60,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x80,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xfc,0x00,0x00,0x3b,0xe7,0x80,
    0x00,0x7d,0x9f,0xe0,0x00,0xfe,0x7f,0xf8,0x01,0xff,0xff,0xfe,0x03,0xff,0xff,0xfd,
    0x07,0xff,0xff,0xf9,0x0f,0xff,0xff,0xf2,0x1f,0xff,0xff,0xe4,0x3f,0xff,0xff,0xc8,
    0x7f,0xff,0xff,0x90,0xff,0xff,0xff,0x20,0x3f,0xff,0xfe,0x40,0x8f,0xff,0xfc,0x80,
    0x23,0xff,0xf9,0x00,0x08,0xff,0xf2,0x00,0x02,0x3f,0xe4,0x00,0x00,0x8f,0xc8,0x00,
    0x00,0x23,0x90,0x00,0x00,0x08,0x20,0x00,0x00,0x02,0x40,0x00,0x00,0x00,0x80,0x00,
};

static const UBYTE tool_data_1[] =
{
    0x00,0x00,0x00,0x00,0x07,0xf0,0x00,0x00,0x0c,0x10,0x00,0x00,0x10,0x30,0x00,0x00,
    0x20,0x60,0x00,0x00,0x40,0xc0,0x00,0x00,0x40,0xc1,0x00,0x00,0x40,0xc2,0x80,0x00,
    0x40,0x64,0xc0,0x00,0x41,0x18,0xc0,0x00,0x43,0x88,0xc0,0x00,0x62,0xc5,0x80,0x00,
    0x31,0x63,0x80,0x00,0x18,0xc3,0x00,0x00,0x0c,0x06,0x00,0x00,0x0c,0x0c,0x00,0x01,
    0x04,0xc8,0x00,0x03,0x04,0x80,0x00,0x06,0x04,0x80,0x00,0x0c,0x04,0x00,0x00,0x18,
    0x04,0x80,0x00,0x30,0x01,0x00,0x00,0x60,0x84,0x00,0x00,0xc0,0xe0,0x00,0x01,0x80,
    0x38,0x00,0x03,0x00,0x0e,0x00,0x06,0x00,0x03,0x80,0x0c,0x00,0x00,0xe0,0x18,0x00,
    0x00,0x38,0x30,0x00,0x00,0x0e,0x60,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x80,0x00,
    0x00,0x00,0x00,0x00,0x04,0x10,0x00,0x00,0x01,0xc0,0x00,0x00,0x07,0x90,0x00,0x00,
    0x0f,0x20,0x00,0x00,0x5e,0x40,0x00,0x00,0x3e,0x40,0x00,0x00,0x3e,0x41,0x00,0x00,
    0x3f,0x01,0x60,0x00,0x3c,0x43,0x58,0x00,0x5d,0x36,0x4e,0x00,0x4c,0x90,0x9f,0x80,
    0x24,0x58,0xbf,0xe0,0x12,0x19,0x7f,0xf8,0x0b,0xf2,0xff,0xfe,0x0b,0xe5,0xff,0xfd,
    0x03,0x63,0xff,0xf9,0x0b,0x27,0xff,0xf2,0x17,0x8f,0xff,0xe4,0x23,0x1f,0xff,0xc8,
    0x47,0xbf,0xff,0x90,0x83,0x7f,0xff,0x20,0x24,0xff,0xfe,0x40,0x89,0xff,0xfc,0x80,
    0x23,0xff,0xf9,0x00,0x08,0xff,0xf2,0x00,0x02,0x3f,0xe4,0x00,0x00,0x8f,0xc8,0x00,
    0x00,0x23,0x90,0x00,0x00,0x08,0x20,0x00,0x00,0x02,0x40,0x00,0x00,0x00,0x80,0x00
};

static const UBYTE tool_data_2[] =
{
    0x00,0x00,0x00,0x00,0x07,0xf0,0x00,0x00,0x0c,0x10,0x00,0x00,0x10,0x30,0x00,0x00,
    0x20,0x68,0x00,0x00,0x40,0xcc,0x00,0x00,0x40,0xcc,0x00,0x00,0x40,0x8c,0x00,0x00,
    0x40,0x78,0x00,0x00,0x41,0x10,0x00,0x00,0x43,0x88,0x00,0x00,0x62,0xc4,0x00,0x00,
    0x31,0x62,0x00,0x00,0x18,0xc2,0x00,0x00,0x0c,0x06,0x00,0x00,0x0c,0x0c,0x00,0x01,
    0x04,0xc8,0x00,0x03,0x04,0x80,0x00,0x06,0x04,0x80,0x00,0x0c,0x04,0x00,0x00,0x18,
    0x04,0x80,0x00,0x30,0x01,0x00,0x00,0x60,0x84,0x00,0x00,0xc0,0xe0,0x00,0x01,0x80,
    0x38,0x00,0x03,0x00,0x0e,0x00,0x06,0x00,0x03,0x80,0x0c,0x00,0x00,0xe0,0x18,0x00,
    0x00,0x38,0x30,0x00,0x00,0x0e,0x60,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x80,0x00,
    0x00,0x00,0x00,0x00,0x04,0x10,0x00,0x00,0x01,0xc0,0x00,0x00,0x07,0x80,0x00,0x00,
    0x0f,0x10,0x00,0x00,0x5e,0x14,0x00,0x00,0x3e,0x74,0x00,0x00,0x3e,0x64,0x00,0x00,
    0x3f,0x08,0x00,0x00,0x3c,0x40,0x00,0x00,0x5d,0x33,0xfc,0x00,0x4c,0x91,0xe7,0x80,
    0x24,0x58,0x9f,0xe0,0x12,0x18,0x7f,0xf8,0x0b,0xf2,0xff,0xfe,0x0b,0xe5,0xff,0xfd,
    0x03,0x63,0xff,0xf9,0x0b,0x27,0xff,0xf2,0x17,0x8f,0xff,0xe4,0x23,0x1f,0xff,0xc8,
    0x47,0xbf,0xff,0x90,0x83,0x7f,0xff,0x20,0x24,0xff,0xfe,0x40,0x89,0xff,0xfc,0x80,
    0x23,0xff,0xf9,0x00,0x08,0xff,0xf2,0x00,0x02,0x3f,0xe4,0x00,0x00,0x8f,0xc8,0x00,
    0x00,0x23,0x90,0x00,0x00,0x08,0x20,0x00,0x00,0x02,0x40,0x00,0x00,0x00,0x80,0x00
};

struct DiskObject *__GetBuiltinIcon_WB(LONG type, struct IconBase *IconBase)
{
    struct DiskObject  temp = { 0 };
    struct DrawerData  dd   = { 0 };
    struct Image       img1 = { 0 };
    struct Image       img2 = { 0 };
    
    /* Only WBDISK, WBDRAWER, WBTOOL and WBPROJECT are supported */
    if (type < WBDISK || type > WBPROJECT) return NULL;
    
    temp.do_Magic                = WB_DISKMAGIC;
    temp.do_Version              = (WB_DISKVERSION << 8) | WB_DISKREVISION;
    temp.do_Type                 = type;
    temp.do_CurrentX             = NO_ICON_POSITION;
    temp.do_CurrentY             = NO_ICON_POSITION;
    temp.do_Gadget.GadgetRender  = &img1;
    temp.do_Gadget.SelectRender  = &img2;
    temp.do_Gadget.Width         = ICON_HEIGHT;
    temp.do_Gadget.Height        = ICON_HEIGHT;
    temp.do_Gadget.Flags        |= GFLG_GADGIMAGE;
    // FIXME: probably need to setup some more fields 
    
    img1.Width     = ICON_WIDTH;
    img1.Height    = ICON_HEIGHT;
    img1.PlanePick = ICON_DEPTH;
    img2.Width     = ICON_WIDTH;
    img2.Height    = ICON_HEIGHT;
    img2.PlanePick = ICON_DEPTH;
            
    switch (type)
    {
        case WBDISK:
            temp.do_DrawerData =           &dd;
            img1.ImageData     = (UWORD *) disk_data_1;
            img2.ImageData     = (UWORD *) disk_data_2;
            break;
        
        case WBDRAWER:
            temp.do_DrawerData =           &dd;
            img1.ImageData     = (UWORD *) drawer_data_1;
            img2.ImageData     = (UWORD *) drawer_data_2;
            break;
        
        case WBPROJECT:
            img1.ImageData     = (UWORD *) project_data_1;
            img2.ImageData     = (UWORD *) project_data_2;
            break;
        
        case WBTOOL:
            img1.ImageData     = (UWORD *) tool_data_1;
            img2.ImageData     = (UWORD *) tool_data_2;
            break;
    }

    return DupDiskObjectA(&temp, NULL);
}