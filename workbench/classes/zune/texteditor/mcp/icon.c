/***************************************************************************

 TextEditor.mcc - Textediting MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2013 by TextEditor.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TextEditor class Support Site:  http://www.sf.net/projects/texteditor-mcc

 $Id$

***************************************************************************/

#if !defined(__MORPHOS__)
// uncompressed ARGB data
const unsigned long icon32[] = {
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x49da7d7f, 0xebd87e80, 0xffd5856b, 0xc5c3725d, 0x37bb5e4f, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x31e68689, 0xd0df8484, 0xffd37979, 0xffbf6566, 0xffb55a5d, 0xffb96547, 0x38b05341, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x59e0b86a, 0xbcda7b81, 0xffe48b8a, 0xffd37979, 0xffc16767, 0xffae5454, 0xffa44a4d, 0xc5994339, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x6cecdb5c, 0xffe6ce73, 0xffca786b, 0xffd07477, 0xffd27877, 0xffc26868, 0xffb05656, 0xff9b4142, 0xff984336, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x6dffff00, 0xffede049, 0xffe9d876, 0xffd2a861, 0xffb66057, 0xffbf6366, 0xffba6060, 0xffa64c4c, 0xff9b4141, 0xec923839, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x66ffff00, 0xf9ffff00, 0xfff6e829, 0xffe0c657, 0xffdbbf5a, 0xffc5904b, 0xffac5949, 0xffa3474b, 0xff9a4040, 0xd1923838, 0x49892c2e, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x57ffff00, 0xe9ffff00, 0xfffbf117, 0xfffaf127, 0xffefd627, 0xffcea545, 0xffcca03d, 0xffae6d2b, 0xff974033, 0xba903438, 0x308a2a2e, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x45ffff00, 0xdbffff00, 0xfffbf118, 0xfff9f02a, 0xfff9e51c, 0xfff4c904, 0xffd7a40d, 0xffb77c25, 0xffa76522, 0xff994d1c, 0x5b8b3619, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x33ffff04, 0xccfdf821, 0xfff9ee26, 0xfff9f029, 0xfff9e51c, 0xfff4c706, 0xffddaa00, 0xffb38200, 0xff895407, 0xfc874012, 0x6e87310f, 0x05560000, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x21ffff0a, 0xbafdfa1f, 0xfff9ef29, 0xfff9ef27, 0xfff9e41b, 0xfff5c804, 0xffddaa00, 0xffb58300, 0xff7e4d00, 0xf5611c00, 0x765c0000, 0x12000000, 0x0f000000, 0x0a000000, 0x05000000, 0x01000000, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x04fff400, 0x89fdf318, 0xfff9ed27, 0xfff8ed27, 0xfff9e51c, 0xfff5c805, 0xffdfad00, 0xffb58300, 0xff7e4c00, 0xec601d00, 0x77550000, 0x23000000, 0x24000000, 0x22000000, 0x1d000000, 0x12000000, 0x06000000, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x3bffd056, 0xe8f6c940, 0xffebc243, 0xfff5d722, 0xfff7cd03, 0xffdfab00, 0xffb58300, 0xff7d4c00, 0xe25f1e00, 0x734a0000, 0x2f000000, 0x30000000, 0x2f000000, 0x2b000000, 0x24000000, 0x16000000, 0x08000000, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x06ffccb8, 0x9df9be82, 0xffe8ab76, 0xffd29658, 0xffdaa61f, 0xffd8a506, 0xffb48201, 0xff7f4f00, 0xd95b1c00, 0x6f3c0000, 0x3c000000, 0x3c000000, 0x39000000, 0x30000000, 0x24000000, 0x18000000, 0x0c000000, 0x04000000, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x3bfabb83, 0xeee9ac71, 0xffd1955a, 0xffb77b40, 0xffa96e2d, 0xffa26c13, 0xff804f02, 0xcf5d2100, 0x6b2d0000, 0x46000000, 0x42000000, 0x3b000000, 0x2f000000, 0x21000000, 0x11000000, 0x06000000, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x01ffffff, 0xa1f1ac66, 0xffd29558, 0xffb2763a, 0xff9d6427, 0xff90581e, 0xf285510c, 0xad562600, 0x671e0000, 0x4a000000, 0x41000000, 0x34000000, 0x26000000, 0x17000000, 0x0b000000, 0x03000000, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x44777372, 0xf59e7c5c, 0xffa36d36, 0xff945b1e, 0xf6865015, 0xbf6a3e0e, 0x7f3b2304, 0x55090000, 0x45000000, 0x37000000, 0x27000000, 0x18000000, 0x0b000000, 0x03000000, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x06000000, 0xb45f6162, 0xff4c4b49, 0xf85e4120, 0xc073420a, 0x7a3b2106, 0x53100701, 0x42000000, 0x35000000, 0x25000000, 0x17000000, 0x0a000000, 0x02000000, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x09000000, 0xfc494949, 0xc0242427, 0x7607060f, 0x420c0300, 0x2f000000, 0x24000000, 0x1c000000, 0x12000000, 0x06000000, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff,
	0x00ff00ff, 0x00ff00ff, 0x06000000, 0x15000000, 0x20000000, 0x1f000000, 0x15000000, 0x0a000000, 0x02000000, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff,
};
#else // !__MORPHOS__
// bzip2 compressed ARGB data
const unsigned char icon32[] =
{
#if defined(__PPC__)
	0x00, 0x00, 0x00, 0x18,
	0x00, 0x00, 0x00, 0x14,
	'B', 'Z', '2', '\0',
	0x00, 0x00, 0x03, 0xa8,
#else
	0x18, 0x00, 0x00, 0x00,
	0x14, 0x00, 0x00, 0x00,
	'B', 'Z', '2', '\0',
	0xa8, 0x03, 0x00, 0x00,
#endif

	0x42, 0x5A, 0x68, 0x39, 0x31, 0x41, 0x59, 0x26, 0x53, 0x59, 0xB8, 0xC2, 0x9A, 0x35, 0x00, 0x00,
	0x86, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xEF, 0xFF, 0xBF, 0xEF, 0xFF, 0x7F, 0xBE, 0xEF, 0xFF, 0xFF,
	0xDF, 0xBD, 0xFF, 0xFF, 0xB8, 0x57, 0xFB, 0x7F, 0xFF, 0x5F, 0xFD, 0xFB, 0xF7, 0xFB, 0xFA, 0xD7,
	0x6F, 0xFF, 0xFE, 0xC0, 0x02, 0x4E, 0x08, 0xD0, 0xCB, 0x88, 0xA7, 0x91, 0x13, 0x42, 0x9A, 0x79,
	0x4F, 0x53, 0xF5, 0x09, 0xA7, 0x89, 0xA4, 0xCD, 0x20, 0x34, 0x1A, 0x0D, 0x34, 0x00, 0x19, 0x1E,
	0xA3, 0x43, 0xD4, 0x1A, 0x01, 0xA0, 0x0D, 0x00, 0x0F, 0x53, 0x23, 0x4D, 0x0D, 0x03, 0xD2, 0x0F,
	0x53, 0x47, 0xA8, 0xF5, 0x34, 0x3D, 0x4C, 0x86, 0x26, 0x23, 0xD4, 0x00, 0xD0, 0x7A, 0x80, 0xD0,
	0xC8, 0xD0, 0x32, 0x6D, 0x13, 0x6A, 0x60, 0xD1, 0x10, 0x19, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00,
	0x34, 0x00, 0x00, 0x68, 0x00, 0x00, 0x01, 0xA0, 0xC4, 0x00, 0x00, 0x0C, 0x80, 0x0D, 0x00, 0x00,
	0xD3, 0x23, 0x40, 0xC8, 0x06, 0x13, 0x40, 0xD0, 0x32, 0x34, 0x1A, 0x32, 0x68, 0x00, 0x00, 0x39,
	0x34, 0xF5, 0x34, 0x03, 0x40, 0x1A, 0x06, 0x43, 0x40, 0x68, 0x0D, 0x00, 0x0C, 0x40, 0x34, 0x00,
	0x31, 0x00, 0x00, 0x00, 0xC2, 0x00, 0x00, 0x00, 0x34, 0x68, 0x06, 0x80, 0x00, 0x00, 0x00, 0x34,
	0x00, 0x06, 0x86, 0x80, 0x00, 0x00, 0x18, 0xA1, 0x14, 0x46, 0x9A, 0x00, 0xC8, 0x01, 0xB5, 0x0F,
	0x50, 0xD0, 0x1A, 0x7A, 0x86, 0x4D, 0x34, 0x34, 0xD0, 0x03, 0x40, 0x00, 0x00, 0x00, 0x06, 0x80,
	0xD0, 0xC8, 0x0D, 0x0D, 0x00, 0x00, 0x1A, 0x00, 0x00, 0x00, 0x3D, 0x20, 0x00, 0x00, 0xD0, 0x00,
	0x01, 0x90, 0x92, 0x13, 0x81, 0x1C, 0x53, 0x49, 0x20, 0x40, 0x15, 0x2C, 0x05, 0xA1, 0x78, 0x26,
	0x0C, 0x06, 0x1E, 0x1E, 0x21, 0x48, 0x92, 0x14, 0x5A, 0x31, 0x65, 0x16, 0xC2, 0x36, 0x39, 0xAC,
	0x79, 0x81, 0xAB, 0x55, 0xD2, 0x07, 0xC1, 0x61, 0x5B, 0x5E, 0x90, 0x90, 0x84, 0xE9, 0x17, 0xED,
	0x2D, 0x94, 0xB6, 0x4A, 0xDA, 0xD8, 0xAE, 0x32, 0xD1, 0x96, 0x18, 0x26, 0xF4, 0x80, 0x27, 0x00,
	0x88, 0x37, 0xB4, 0x08, 0x88, 0x40, 0x08, 0x94, 0x1F, 0x93, 0x91, 0x68, 0x20, 0x9B, 0x22, 0x00,
	0x0A, 0xB4, 0x08, 0x0F, 0x28, 0x1F, 0x67, 0xE5, 0xE8, 0xA5, 0xD2, 0xBC, 0xB1, 0xC1, 0x12, 0x40,
	0x0E, 0x44, 0x08, 0xD4, 0x0D, 0x49, 0x00, 0xE4, 0x05, 0xC1, 0x02, 0x1A, 0x89, 0x12, 0x96, 0xF6,
	0x24, 0x49, 0x78, 0x20, 0x49, 0x09, 0x02, 0x4A, 0x44, 0x10, 0x82, 0x05, 0x22, 0x09, 0x48, 0x04,
	0x81, 0x04, 0x83, 0x34, 0x66, 0xB7, 0xD3, 0xDC, 0x06, 0x3C, 0x03, 0xF6, 0xD7, 0xDA, 0xD1, 0x16,
	0x84, 0x97, 0x65, 0x91, 0xEF, 0x43, 0x86, 0xC7, 0x49, 0x35, 0x85, 0xA2, 0x8F, 0x53, 0x9E, 0x6C,
	0x3B, 0x9E, 0x87, 0x11, 0x12, 0xF4, 0x16, 0x1B, 0x03, 0x58, 0x59, 0x87, 0xDF, 0xE0, 0x5D, 0x7D,
	0xB1, 0xE2, 0x5A, 0x06, 0x0F, 0x38, 0xC1, 0x8A, 0x47, 0xEA, 0x87, 0xDD, 0x95, 0xCA, 0x07, 0x64,
	0xB2, 0xF1, 0xFE, 0xD2, 0xEC, 0xC6, 0x77, 0x33, 0xB6, 0x4D, 0x1A, 0x4B, 0x92, 0x91, 0x1D, 0xDE,
	0x66, 0xE1, 0x99, 0xE6, 0xBE, 0xC4, 0x88, 0x80, 0x7A, 0xF3, 0xC5, 0xCC, 0x19, 0x5B, 0x42, 0x3C,
	0xE5, 0x73, 0x1B, 0x48, 0x23, 0x67, 0xFE, 0x33, 0xD2, 0x57, 0xCA, 0x7A, 0xE6, 0x7D, 0xC6, 0xFD,
	0x20, 0x42, 0x6D, 0xEC, 0x54, 0x80, 0x9E, 0x88, 0x67, 0x68, 0xA1, 0x8C, 0xD3, 0x5B, 0x46, 0x0B,
	0x67, 0x91, 0x4C, 0xD9, 0x13, 0x3C, 0xEB, 0x43, 0x01, 0x82, 0x83, 0xBB, 0xD0, 0x61, 0x4B, 0x4E,
	0xA7, 0x5E, 0x59, 0x5E, 0x4A, 0x82, 0xA0, 0x31, 0x13, 0xC8, 0xD0, 0xB7, 0x3D, 0x3D, 0x45, 0x5A,
	0x8A, 0x9B, 0x91, 0x49, 0x78, 0x2E, 0x9D, 0xB3, 0x14, 0xBA, 0x4F, 0x2C, 0xD5, 0xAF, 0x6F, 0x32,
	0xD7, 0x07, 0x49, 0x4A, 0xB7, 0x30, 0x91, 0x94, 0x71, 0x7F, 0x65, 0x4D, 0x53, 0x53, 0x2F, 0x55,
	0x27, 0x55, 0x31, 0x4E, 0xED, 0xDE, 0xE5, 0x57, 0xE3, 0x39, 0x2F, 0xF5, 0x54, 0x56, 0xD8, 0xD9,
	0xDC, 0x02, 0x5C, 0x0F, 0x37, 0x71, 0x51, 0x4F, 0x71, 0xFE, 0x07, 0x09, 0x6A, 0xCB, 0x9A, 0xF3,
	0x31, 0xEF, 0xCB, 0xD0, 0xAB, 0xF3, 0x3C, 0x66, 0x8D, 0x4B, 0x88, 0xCF, 0x48, 0x07, 0xCD, 0xD0,
	0x40, 0x9D, 0xBA, 0xE9, 0xA0, 0x0B, 0xA3, 0x75, 0x21, 0xB7, 0x71, 0x9A, 0x59, 0x8A, 0x6C, 0x55,
	0xAF, 0x93, 0x5D, 0xF9, 0x95, 0x70, 0x79, 0x3B, 0x0C, 0x63, 0xEA, 0xE4, 0x5E, 0x59, 0x97, 0xC6,
	0xF7, 0x59, 0x61, 0x02, 0xC8, 0x2B, 0x81, 0xFD, 0x7F, 0xF4, 0x7F, 0x71, 0x46, 0x10, 0x86, 0xF9,
	0x14, 0xB8, 0xE8, 0xE6, 0x6C, 0x24, 0xAC, 0x65, 0x10, 0xC7, 0xC1, 0xFE, 0x5B, 0xC1, 0xB8, 0xB6,
	0xE3, 0x6D, 0x26, 0x57, 0xEA, 0x63, 0x1F, 0xCF, 0x3B, 0xEA, 0x4A, 0x41, 0x4E, 0xD5, 0xB1, 0x35,
	0x30, 0x90, 0x1C, 0x50, 0x0C, 0x76, 0x6A, 0xD4, 0xC0, 0x0F, 0x06, 0x36, 0x45, 0xA9, 0x81, 0x58,
	0xDB, 0x03, 0x2A, 0x03, 0x74, 0x6D, 0xCF, 0xDF, 0x38, 0x4D, 0x0A, 0x17, 0xC1, 0x00, 0x07, 0x0B,
	0x88, 0x20, 0x61, 0xEA, 0xDE, 0xB9, 0x85, 0x30, 0xA8, 0x2B, 0x23, 0xAB, 0x4D, 0x72, 0x2C, 0xC6,
	0x92, 0x85, 0x7B, 0xF6, 0x80, 0x32, 0x09, 0x21, 0xBE, 0x85, 0x86, 0x35, 0x88, 0x84, 0xDB, 0xA2,
	0x00, 0x0A, 0x25, 0x84, 0xD1, 0x79, 0x32, 0x84, 0xA1, 0xB0, 0x82, 0x71, 0x0C, 0xE8, 0x87, 0x65,
	0xC9, 0x1D, 0x89, 0x0A, 0x0F, 0x89, 0x41, 0xE0, 0xD3, 0x12, 0x43, 0xC7, 0x27, 0x4F, 0xA8, 0xD3,
	0x0F, 0xB3, 0xB4, 0x83, 0x10, 0xA1, 0x10, 0x68, 0x7C, 0x34, 0x4F, 0x98, 0xA9, 0x96, 0x17, 0x42,
	0x60, 0x7A, 0x35, 0x93, 0x08, 0x11, 0x10, 0xEE, 0x6B, 0x4B, 0x67, 0xE3, 0xAC, 0xD6, 0xE7, 0x8B,
	0xC4, 0x10, 0x09, 0x28, 0x04, 0x91, 0x09, 0x00, 0x3F, 0xA8, 0x01, 0xBB, 0xFC, 0x33, 0x19, 0xD3,
	0xF7, 0x43, 0xF7, 0xEF, 0x77, 0x51, 0xDF, 0xB8, 0xBF, 0x54, 0x65, 0x06, 0xE2, 0x73, 0xE3, 0x0A,
	0x2A, 0x6F, 0xF9, 0x37, 0x3F, 0x50, 0xDE, 0x42, 0x24, 0x27, 0xDD, 0x5B, 0xF9, 0x51, 0x85, 0xBD,
	0x09, 0xC4, 0xA7, 0x6C, 0x96, 0xC6, 0x60, 0x5B, 0x5D, 0x26, 0xB5, 0xE0, 0x6D, 0xDF, 0xF6, 0xE5,
	0xA0, 0xC4, 0x30, 0xBC, 0x9E, 0x05, 0x2D, 0xD5, 0xDC, 0x75, 0x7C, 0x7C, 0x6C, 0x5C, 0x4A, 0xCA,
	0x30, 0xBB, 0x17, 0xE8, 0xB0, 0x05, 0x04, 0x89, 0xBC, 0x13, 0xE7, 0xA8, 0x04, 0xCF, 0x40, 0x3E,
	0x90, 0x41, 0x2A, 0x87, 0x11, 0xC2, 0xE9, 0x01, 0x19, 0x20, 0x19, 0x98, 0x77, 0xF8, 0xBB, 0x92,
	0x29, 0xC2, 0x84, 0x85, 0xC6, 0x14, 0xD1, 0xA8,
};
#endif // !__MORPHOS__
