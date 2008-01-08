/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2002,2005,2006,2007  Free Software Foundation, Inc.
 *
 *  GRUB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GRUB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GRUB_EFI_CONSOLE_HEADER
#define GRUB_EFI_CONSOLE_HEADER	1

/* Define scan codes.  */
#define GRUB_CONSOLE_KEY_LEFT		0x4B00
#define GRUB_CONSOLE_KEY_RIGHT		0x4D00
#define GRUB_CONSOLE_KEY_UP		0x4800
#define GRUB_CONSOLE_KEY_DOWN		0x5000
#define GRUB_CONSOLE_KEY_IC		0x5200
#define GRUB_CONSOLE_KEY_DC		0x5300
#define GRUB_CONSOLE_KEY_BACKSPACE	0x0008
#define GRUB_CONSOLE_KEY_HOME		0x4700
#define GRUB_CONSOLE_KEY_END		0x4F00
#define GRUB_CONSOLE_KEY_NPAGE		0x4900
#define GRUB_CONSOLE_KEY_PPAGE		0x5100

#include <grub/types.h>
#include <grub/symbol.h>

/* Initialize the console system.  */
void grub_console_init (void);

/* Finish the console system.  */
void grub_console_fini (void);

#endif /* ! GRUB_EFI_CONSOLE_HEADER */
