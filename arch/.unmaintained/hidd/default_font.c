/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <graphics/text.h>

/* In ISO 8859-15 Euro symbol is ASCII #164 (0xA4) */

#define _EURO 1

#ifdef _EURO
    #define A4_0 0x1C /* ...***.. */
    #define A4_1 0x22 /* ..*...*. */
    #define A4_2 0x78 /* .****... */
    #define A4_3 0x20 /* ..*..... */
    #define A4_4 0x70 /* .***.... */
    #define A4_5 0x22 /* ..*...*. */
    #define A4_6 0x1C /* ...***.. */
    #define A4_7 0x00 /* ........ */
#else
    #define A4_0 0x42 /* .*....*. */
    #define A4_1 0x3C /* ..****.. */
    #define A4_2 0x66 /* .**..**. */
    #define A4_3 0x3C /* ..****.. */
    #define A4_4 0x42 /* .*....*. */
    #define A4_5 0x00 /* ........ */
    #define A4_6 0x00 /* ........ */
    #define A4_7 0x00 /* ........ */
#endif

const UBYTE topaz8_chardata[] =
{
    0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x1c, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x78, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x18, 0x70, 0x00, 0x0f, 
    0x00, 0x18, 0x00, 0x1c, A4_0, 0xc3, 0x18, 0x3c, 0x66, 0x7e, 0x1c, 0x00, 0x3e, 0x7e, 0x7e, 0x3c, 
    0x18, 0x78, 0x78, 0x18, 0x00, 0x3e, 0x00, 0x00, 0x30, 0x38, 0x00, 0x40, 0x40, 0xc0, 0x18, 0x30, 
    0x0c, 0x18, 0x71, 0x66, 0x18, 0x1f, 0x1e, 0x30, 0x0c, 0x18, 0x66, 0x30, 0x0c, 0x18, 0x66, 0x78, 
    0x71, 0x30, 0x0c, 0x18, 0x71, 0x00, 0x00, 0x3f, 0x30, 0x0c, 0x18, 0x66, 0x06, 0xc0, 0x00, 0x30, 
    0x0c, 0x18, 0x71, 0x66, 0x18, 0x00, 0x00, 0x30, 0x0c, 0x18, 0x66, 0x30, 0x0c, 0x18, 0x00, 0x60, 
    0x71, 0x30, 0x0c, 0x18, 0x71, 0x00, 0x00, 0x00, 0x30, 0x0c, 0x18, 0x00, 0x0c, 0x60, 0x00, 0x00, 

    0x00, 0x18, 0x66, 0x66, 0x3e, 0x66, 0x36, 0x18, 0x18, 0x18, 0x66, 0x18, 0x00, 0x00, 0x00, 0x06, 
    0x3c, 0x18, 0x3c, 0x7e, 0x0c, 0x7e, 0x3c, 0x7e, 0x3c, 0x3c, 0x18, 0x18, 0x06, 0x00, 0x60, 0x3c, 
    0x3c, 0x18, 0x7c, 0x3c, 0x7c, 0x7e, 0x7e, 0x3c, 0x66, 0x7e, 0x06, 0x66, 0x60, 0x63, 0x46, 0x3c, 
    0x7c, 0x3c, 0x7c, 0x3c, 0x7e, 0x66, 0x66, 0x63, 0x66, 0x66, 0x7e, 0x18, 0x40, 0x18, 0x18, 0x00, 
    0x18, 0x00, 0x60, 0x00, 0x06, 0x00, 0x0e, 0x00, 0x60, 0x18, 0x06, 0x60, 0x38, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x00, 0x3c, 
    0x7e, 0x00, 0x0c, 0x36, A4_1, 0x66, 0x18, 0x60, 0x66, 0x81, 0x24, 0x33, 0x06, 0x81, 0x00, 0x66, 
    0x18, 0x0c, 0x0c, 0x30, 0x00, 0x7a, 0x00, 0x00, 0x70, 0x44, 0xcc, 0xc6, 0xc6, 0x23, 0x00, 0x18, 
    0x18, 0x66, 0x8e, 0x00, 0x24, 0x3c, 0x30, 0x18, 0x18, 0x66, 0x00, 0x18, 0x18, 0x66, 0x00, 0x6c, 
    0xce, 0x18, 0x18, 0x66, 0x8e, 0x66, 0xc6, 0x66, 0x18, 0x18, 0x24, 0x00, 0x08, 0xc0, 0x3c, 0x18, 
    0x18, 0x66, 0x8e, 0x00, 0x24, 0x00, 0x00, 0x18, 0x18, 0x66, 0x00, 0x18, 0x18, 0x66, 0x66, 0xfc, 
    0x8e, 0x18, 0x18, 0x66, 0x8e, 0x66, 0x18, 0x02, 0x18, 0x18, 0x66, 0x66, 0x18, 0x60, 0x66, 0x00, 

    0x00, 0x18, 0x66, 0xff, 0x60, 0x6c, 0x1c, 0x18, 0x30, 0x0c, 0x3c, 0x18, 0x00, 0x00, 0x00, 0x0c, 
    0x66, 0x38, 0x66, 0x0c, 0x1c, 0x60, 0x60, 0x06, 0x66, 0x66, 0x18, 0x18, 0x18, 0x7e, 0x18, 0x66, 
    0x66, 0x3c, 0x66, 0x66, 0x66, 0x60, 0x60, 0x66, 0x66, 0x18, 0x06, 0x6c, 0x60, 0x77, 0x66, 0x66, 
    0x66, 0x66, 0x66, 0x60, 0x18, 0x66, 0x66, 0x63, 0x66, 0x66, 0x0c, 0x18, 0x60, 0x18, 0x3c, 0x00, 
    0x18, 0x3c, 0x60, 0x3c, 0x06, 0x3c, 0x18, 0x3e, 0x60, 0x00, 0x00, 0x60, 0x18, 0x7c, 0x7c, 0x3c, 
    0x7c, 0x3e, 0x7c, 0x3e, 0x7e, 0x66, 0x66, 0x42, 0x66, 0x66, 0x7e, 0x18, 0x18, 0x18, 0x32, 0xf0, 
    0x66, 0x18, 0x3e, 0x30, A4_2,  0x3c, 0x18, 0x3c, 0x00, 0x9d, 0x44, 0x66, 0x00, 0xb9, 0x00, 0x3c, 
    0x7e, 0x18, 0x18, 0x60, 0x66, 0x7a, 0x18, 0x00, 0x30, 0x44, 0x66, 0x4c, 0x4c, 0x66, 0x18, 0x3c, 
    0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x60, 0x7e, 0x7e, 0x7e, 0x7e, 0x3c, 0x3c, 0x3c, 0x3c, 0x66, 
    0xe6, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x6c, 0x6e, 0x66, 0x66, 0x66, 0x66, 0xc3, 0xfc, 0x66, 0x3c, 
    0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x7e, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x18, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 

    0x00, 0x18, 0x66, 0x66, 0x3c, 0x18, 0x38, 0x10, 0x30, 0x0c, 0xff, 0x7e, 0x00, 0x7e, 0x00, 0x18, 
    0x6e, 0x18, 0x0c, 0x18, 0x3c, 0x7c, 0x7c, 0x0c, 0x3c, 0x3e, 0x00, 0x00, 0x60, 0x00, 0x06, 0x0c, 
    0x6e, 0x66, 0x7c, 0x60, 0x66, 0x7c, 0x7c, 0x60, 0x7e, 0x18, 0x06, 0x78, 0x60, 0x7f, 0x76, 0x66, 
    0x66, 0x66, 0x66, 0x3c, 0x18, 0x66, 0x66, 0x6b, 0x3c, 0x3c, 0x18, 0x18, 0x30, 0x18, 0x7e, 0x00, 
    0x08, 0x06, 0x7c, 0x60, 0x3e, 0x66, 0x3e, 0x66, 0x7c, 0x38, 0x06, 0x6c, 0x18, 0x6a, 0x66, 0x66, 
    0x66, 0x66, 0x66, 0x60, 0x18, 0x66, 0x66, 0x42, 0x3c, 0x66, 0x0c, 0x70, 0x18, 0x0e, 0x4c, 0xc3, 
    0x66, 0x18, 0x6c, 0x78, A4_3, 0x18, 0x00, 0x66, 0x00, 0xb1, 0x3c, 0xcc, 0x00, 0xa5, 0x00, 0x00, 
    0x18, 0x30, 0x0c, 0x00, 0x66, 0x3a, 0x18, 0x00, 0x30, 0x38, 0x33, 0x58, 0x58, 0x2c, 0x30, 0x66, 
    0x66, 0x66, 0x66, 0x66, 0x66, 0x6f, 0x60, 0x60, 0x60, 0x60, 0x60, 0x18, 0x18, 0x18, 0x18, 0xf6, 
    0xf6, 0x66, 0x66, 0x66, 0x66, 0x66, 0x38, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x66, 0xc6, 0x7c, 0x06, 
    0x06, 0x06, 0x06, 0x06, 0x06, 0x1b, 0x60, 0x66, 0x66, 0x66, 0x66, 0x18, 0x18, 0x18, 0x18, 0x3c, 
    0x7c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x7e, 0xce, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 

    0x00, 0x18, 0x00, 0x66, 0x06, 0x30, 0x6f, 0x00, 0x30, 0x0c, 0x3c, 0x18, 0x00, 0x00, 0x00, 0x30, 
    0x76, 0x18, 0x18, 0x0c, 0x6c, 0x06, 0x66, 0x18, 0x66, 0x06, 0x00, 0x00, 0x18, 0x7e, 0x18, 0x18, 
    0x6e, 0x66, 0x66, 0x60, 0x66, 0x60, 0x60, 0x6e, 0x66, 0x18, 0x06, 0x78, 0x60, 0x6b, 0x6e, 0x66, 
    0x7c, 0x66, 0x7c, 0x06, 0x18, 0x66, 0x66, 0x7f, 0x3c, 0x18, 0x30, 0x18, 0x18, 0x18, 0x00, 0x00, 
    0x00, 0x3e, 0x66, 0x60, 0x66, 0x7e, 0x18, 0x66, 0x66, 0x18, 0x06, 0x78, 0x18, 0x6a, 0x66, 0x66, 
    0x66, 0x66, 0x60, 0x3c, 0x18, 0x66, 0x66, 0x5a, 0x18, 0x66, 0x18, 0x18, 0x18, 0x18, 0x00, 0x0f, 
    0x66, 0x18, 0x3e, 0x30, A4_4, 0x3c, 0x18, 0x3c, 0x00, 0x9d, 0x00, 0x66, 0x00, 0xb9, 0x00, 0x00, 
    0x18, 0x7c, 0x78, 0x00, 0x66, 0x0a, 0x00, 0x00, 0x30, 0x00, 0x66, 0x32, 0x3e, 0xd9, 0x60, 0x7e, 
    0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7c, 0x30, 0x78, 0x78, 0x78, 0x78, 0x18, 0x18, 0x18, 0x18, 0x66, 
    0xde, 0x66, 0x66, 0x66, 0x66, 0x66, 0x6c, 0x76, 0x66, 0x66, 0x66, 0x66, 0x3c, 0xfc, 0x66, 0x3e, 
    0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x7f, 0x60, 0x7e, 0x7e, 0x7e, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x66, 
    0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0xd6, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 

    0x00, 0x00, 0x00, 0xff, 0x7c, 0x66, 0x66, 0x00, 0x18, 0x18, 0x66, 0x18, 0x18, 0x00, 0x18, 0x60, 
    0x66, 0x18, 0x30, 0x66, 0x7e, 0x66, 0x66, 0x30, 0x66, 0x0c, 0x18, 0x18, 0x06, 0x00, 0x60, 0x00, 
    0x60, 0x7e, 0x66, 0x66, 0x66, 0x60, 0x60, 0x66, 0x66, 0x18, 0x66, 0x6c, 0x60, 0x63, 0x66, 0x66, 
    0x60, 0x6c, 0x6c, 0x06, 0x18, 0x66, 0x3c, 0x77, 0x66, 0x18, 0x60, 0x18, 0x0c, 0x18, 0x00, 0x00, 
    0x00, 0x66, 0x66, 0x60, 0x66, 0x60, 0x18, 0x3e, 0x66, 0x18, 0x06, 0x6c, 0x18, 0x6a, 0x66, 0x66, 
    0x7c, 0x3e, 0x60, 0x06, 0x18, 0x66, 0x3c, 0x7e, 0x3c, 0x3e, 0x30, 0x18, 0x18, 0x18, 0x00, 0x3c, 
    0x66, 0x18, 0x0c, 0x30, A4_5, 0x18, 0x18, 0x06, 0x00, 0x81, 0x7e, 0x33, 0x00, 0xa5, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x66, 0x0a, 0x00, 0x00, 0x00, 0x7c, 0xcc, 0x66, 0x62, 0x33, 0x66, 0x66, 
    0x66, 0x66, 0x66, 0x66, 0x66, 0xcc, 0x1e, 0x60, 0x60, 0x60, 0x60, 0x18, 0x18, 0x18, 0x18, 0x6c, 
    0xce, 0x66, 0x66, 0x66, 0x66, 0x66, 0xc6, 0x66, 0x66, 0x66, 0x66, 0x66, 0x18, 0xc0, 0x66, 0x66, 
    0x66, 0x66, 0x66, 0x66, 0x66, 0xd8, 0x60, 0x60, 0x60, 0x60, 0x60, 0x18, 0x18, 0x18, 0x18, 0x66, 
    0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x18, 0xe6, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x7c, 0x3c, 0x00, 

    0x00, 0x18, 0x00, 0x66, 0x18, 0x46, 0x3b, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x18, 0x00, 0x18, 0x40, 
    0x3c, 0x7e, 0x7e, 0x3c, 0x0c, 0x3c, 0x3c, 0x30, 0x3c, 0x38, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 
    0x3e, 0x66, 0x7c, 0x3c, 0x7c, 0x7e, 0x60, 0x3c, 0x66, 0x7e, 0x3c, 0x66, 0x7e, 0x63, 0x66, 0x3c, 
    0x60, 0x36, 0x66, 0x3c, 0x18, 0x3c, 0x18, 0x63, 0x66, 0x18, 0x7e, 0x1e, 0x06, 0x78, 0x00, 0x7e, 
    0x00, 0x3e, 0x7c, 0x3c, 0x3e, 0x3c, 0x18, 0x06, 0x66, 0x3c, 0x06, 0x66, 0x3c, 0x6a, 0x66, 0x3c, 
    0x60, 0x06, 0x60, 0x7c, 0x0e, 0x3e, 0x18, 0x66, 0x66, 0x0c, 0x7e, 0x0e, 0x18, 0x70, 0x00, 0xf0, 
    0x7e, 0x18, 0x00, 0x7e, A4_6, 0x18, 0x18, 0x3c, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 
    0x7e, 0x00, 0x00, 0x00, 0x7f, 0x0a, 0x00, 0x18, 0x00, 0x00, 0x00, 0xcf, 0xc4, 0x67, 0x3c, 0x66, 
    0x66, 0x66, 0x66, 0x66, 0x66, 0xcf, 0x0c, 0x7e, 0x7e, 0x7e, 0x7e, 0x3c, 0x3c, 0x3c, 0x3c, 0x78, 
    0xc6, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0xfc, 0x3c, 0x3c, 0x3c, 0x3c, 0x18, 0xc0, 0x7c, 0x3e, 
    0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x77, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c, 
    0x66, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x7c, 0x3e, 0x3e, 0x3e, 0x3e, 0x18, 0x60, 0x18, 0x00, 

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x60, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, A4_7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x02, 0x0e, 0x01, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x30, 0x60, 0x30, 0x00
};

const ULONG topaz8_charloc[] =
{
	0x00000008, 0x00080008, 0x00100008, 0x00180008, 
	0x00200008, 0x00280008, 0x00300008, 0x00380008, 
	0x00400008, 0x00480008, 0x00500008, 0x00580008, 
	0x00600008, 0x00680008, 0x00700008, 0x00780008, 
	0x00800008, 0x00880008, 0x00900008, 0x00980008, 
	0x00a00008, 0x00a80008, 0x00b00008, 0x00b80008, 
	0x00c00008, 0x00c80008, 0x00d00008, 0x00d80008, 
	0x00e00008, 0x00e80008, 0x00f00008, 0x00f80008, 
	0x01000008, 0x01080008, 0x01100008, 0x01180008, 
	0x01200008, 0x01280008, 0x01300008, 0x01380008, 
	0x01400008, 0x01480008, 0x01500008, 0x01580008, 
	0x01600008, 0x01680008, 0x01700008, 0x01780008, 
	0x01800008, 0x01880008, 0x01900008, 0x01980008, 
	0x01a00008, 0x01a80008, 0x01b00008, 0x01b80008, 
	0x01c00008, 0x01c80008, 0x01d00008, 0x01d80008, 
	0x01e00008, 0x01e80008, 0x01f00008, 0x01f80008, 
	0x02000008, 0x02080008, 0x02100008, 0x02180008, 
	0x02200008, 0x02280008, 0x02300008, 0x02380008, 
	0x02400008, 0x02480008, 0x02500008, 0x02580008, 
	0x02600008, 0x02680008, 0x02700008, 0x02780008, 
	0x02800008, 0x02880008, 0x02900008, 0x02980008, 
	0x02a00008, 0x02a80008, 0x02b00008, 0x02b80008, 
	0x02c00008, 0x02c80008, 0x02d00008, 0x02d80008, 
	0x02e00008, 0x02e80008, 0x02f00008, 0x02f80008, 
	0x03000008, 0x03000008, 0x03000008, 0x03000008, 
	0x03000008, 0x03000008, 0x03000008, 0x03000008, 
	0x03000008, 0x03000008, 0x03000008, 0x03000008, 
	0x03000008, 0x03000008, 0x03000008, 0x03000008, 
	0x03000008, 0x03000008, 0x03000008, 0x03000008, 
	0x03000008, 0x03000008, 0x03000008, 0x03000008, 
	0x03000008, 0x03000008, 0x03000008, 0x03000008, 
	0x03000008, 0x03000008, 0x03000008, 0x03000008, 
	0x00000008, 0x03080008, 0x03100008, 0x03180008, 
	0x03200008, 0x03280008, 0x03300008, 0x03380008, 
	0x03400008, 0x03480008, 0x03500008, 0x03580008, 
	0x03600008, 0x00680008, 0x03680008, 0x03700008, 
	0x03780008, 0x03800008, 0x03880008, 0x03900008, 
	0x03980008, 0x03a00008, 0x03a80008, 0x03b00008, 
	0x03b80008, 0x03c00008, 0x03c80008, 0x03d00008, 
	0x03d80008, 0x03e00008, 0x03e80008, 0x03f00008, 
	0x03f80008, 0x04000008, 0x04080008, 0x04100008, 
	0x04180008, 0x04200008, 0x04280008, 0x04300008, 
	0x04380008, 0x04400008, 0x04480008, 0x04500008, 
	0x04580008, 0x04600008, 0x04680008, 0x04700008, 
	0x04780008, 0x04800008, 0x04880008, 0x04900008, 
	0x04980008, 0x04a00008, 0x04a80008, 0x04b00008, 
	0x04b80008, 0x04c00008, 0x04c80008, 0x04d00008, 
	0x04d80008, 0x04e00008, 0x04e80008, 0x04f00008, 
	0x04f80008, 0x05000008, 0x05080008, 0x05100008, 
	0x05180008, 0x05200008, 0x05280008, 0x05300008, 
	0x05380008, 0x05400008, 0x05480008, 0x05500008, 
	0x05580008, 0x05600008, 0x05680008, 0x05700008, 
	0x05780008, 0x05800008, 0x05880008, 0x05900008, 
	0x05980008, 0x05a00008, 0x05a80008, 0x05b00008, 
	0x05b80008, 0x05c00008, 0x05c80008, 0x05d00008, 
	0x05d80008, 0x05e00008, 0x05e80008, 0x05f00008, 
	0x03000008
};


const struct TextFont topaz8_tf =
{
	{	/* tf_Message */
		{	/* mn_Node */
			NULL,
			NULL,
#if (AROS_FLAVOUR & AROS_FLAVOUR_BINCOMPAT)
			NT_FONT,
			0,
			"topaz.font"
#else
			"topaz.font",
			NT_FONT,
			0
#endif /* AROS_FLAVOUR */
		},
		NULL,
		0
	},
	8,	/* YSize */
	FSF_TAGGED,	/* Style */
	FPF_DESIGNED|FPF_ROMFONT,	/* Flags */
	8,	/* XSize */
	6,	/* Baseline */
	1,	/* Boldsmear */
	0,	/* Accessors */
	32,	/* LoChar */
	255,	/* HiChar */
	(APTR)topaz8_chardata,	/* CharData */
	192,	/* Modulo */
	(APTR)topaz8_charloc,	/* CharLoc   */
	NULL,	/* CharSpace */
	NULL,	/* CharKern  */
};
