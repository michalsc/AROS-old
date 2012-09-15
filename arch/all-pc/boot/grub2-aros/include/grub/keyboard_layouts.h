/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2010  Free Software Foundation, Inc.
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

#ifndef GRUB_KEYBOARD_LAYOUTS_H
#define GRUB_KEYBOARD_LAYOUTS_H 1

#define GRUB_KEYBOARD_LAYOUTS_FILEMAGIC "GRUBLAYO"
#define GRUB_KEYBOARD_LAYOUTS_FILEMAGIC_SIZE (sizeof(GRUB_KEYBOARD_LAYOUTS_FILEMAGIC) - 1)
#define GRUB_KEYBOARD_LAYOUTS_VERSION 8

#define GRUB_KEYBOARD_LAYOUTS_ARRAY_SIZE 128

struct grub_keyboard_layout
{
  grub_uint32_t keyboard_map[GRUB_KEYBOARD_LAYOUTS_ARRAY_SIZE];
  grub_uint32_t keyboard_map_shift[GRUB_KEYBOARD_LAYOUTS_ARRAY_SIZE];
  grub_uint32_t keyboard_map_l3[GRUB_KEYBOARD_LAYOUTS_ARRAY_SIZE];
  grub_uint32_t keyboard_map_shift_l3[GRUB_KEYBOARD_LAYOUTS_ARRAY_SIZE];
};

typedef enum grub_keyboard_key
  {
    GRUB_KEYBOARD_KEY_A = 0x04,
    GRUB_KEYBOARD_KEY_B = 0x05,
    GRUB_KEYBOARD_KEY_C = 0x06,
    GRUB_KEYBOARD_KEY_D = 0x07,
    GRUB_KEYBOARD_KEY_E = 0x08,
    GRUB_KEYBOARD_KEY_F = 0x09,
    GRUB_KEYBOARD_KEY_G = 0x0a,
    GRUB_KEYBOARD_KEY_H = 0x0b,
    GRUB_KEYBOARD_KEY_I = 0x0c,
    GRUB_KEYBOARD_KEY_J = 0x0d,
    GRUB_KEYBOARD_KEY_K = 0x0e,
    GRUB_KEYBOARD_KEY_L = 0x0f,
    GRUB_KEYBOARD_KEY_M = 0x10,
    GRUB_KEYBOARD_KEY_N = 0x11,
    GRUB_KEYBOARD_KEY_O = 0x12,
    GRUB_KEYBOARD_KEY_P = 0x13,
    GRUB_KEYBOARD_KEY_Q = 0x14,
    GRUB_KEYBOARD_KEY_R = 0x15,
    GRUB_KEYBOARD_KEY_S = 0x16,
    GRUB_KEYBOARD_KEY_T = 0x17,
    GRUB_KEYBOARD_KEY_U = 0x18,
    GRUB_KEYBOARD_KEY_V = 0x19,
    GRUB_KEYBOARD_KEY_W = 0x1a,
    GRUB_KEYBOARD_KEY_X = 0x1b,
    GRUB_KEYBOARD_KEY_Y = 0x1c,
    GRUB_KEYBOARD_KEY_Z = 0x1d,
    GRUB_KEYBOARD_KEY_1 = 0x1e,
    GRUB_KEYBOARD_KEY_2 = 0x1f,
    GRUB_KEYBOARD_KEY_3 = 0x20,
    GRUB_KEYBOARD_KEY_4 = 0x21,
    GRUB_KEYBOARD_KEY_5 = 0x22,
    GRUB_KEYBOARD_KEY_6 = 0x23,
    GRUB_KEYBOARD_KEY_7 = 0x24,
    GRUB_KEYBOARD_KEY_8 = 0x25,
    GRUB_KEYBOARD_KEY_9 = 0x26,
    GRUB_KEYBOARD_KEY_0 = 0x27,
    GRUB_KEYBOARD_KEY_ENTER = 0x28,
    GRUB_KEYBOARD_KEY_ESCAPE = 0x29,
    GRUB_KEYBOARD_KEY_BACKSPACE = 0x2a,
    GRUB_KEYBOARD_KEY_TAB = 0x2b,
    GRUB_KEYBOARD_KEY_SPACE = 0x2c,
    GRUB_KEYBOARD_KEY_DASH = 0x2d,
    GRUB_KEYBOARD_KEY_EQUAL = 0x2e,
    GRUB_KEYBOARD_KEY_LBRACKET = 0x2f,
    GRUB_KEYBOARD_KEY_RBRACKET = 0x30,
    GRUB_KEYBOARD_KEY_BACKSLASH = 0x32,
    GRUB_KEYBOARD_KEY_SEMICOLON = 0x33,
    GRUB_KEYBOARD_KEY_DQUOTE = 0x34,
    GRUB_KEYBOARD_KEY_RQUOTE = 0x35,
    GRUB_KEYBOARD_KEY_COMMA = 0x36,
    GRUB_KEYBOARD_KEY_DOT = 0x37,
    GRUB_KEYBOARD_KEY_SLASH = 0x38,
    GRUB_KEYBOARD_KEY_CAPS_LOCK  = 0x39,
    GRUB_KEYBOARD_KEY_F1 = 0x3a,
    GRUB_KEYBOARD_KEY_F2 = 0x3b,
    GRUB_KEYBOARD_KEY_F3 = 0x3c,
    GRUB_KEYBOARD_KEY_F4 = 0x3d,
    GRUB_KEYBOARD_KEY_F5 = 0x3e,
    GRUB_KEYBOARD_KEY_F6 = 0x3f,
    GRUB_KEYBOARD_KEY_F7 = 0x40,
    GRUB_KEYBOARD_KEY_F8 = 0x41,
    GRUB_KEYBOARD_KEY_F9 = 0x42,
    GRUB_KEYBOARD_KEY_F10 = 0x43,
    GRUB_KEYBOARD_KEY_F11 = 0x44,
    GRUB_KEYBOARD_KEY_F12 = 0x45,
    GRUB_KEYBOARD_KEY_SCROLL_LOCK  = 0x47,
    GRUB_KEYBOARD_KEY_INSERT = 0x49,
    GRUB_KEYBOARD_KEY_HOME = 0x4a,
    GRUB_KEYBOARD_KEY_PPAGE = 0x4b,
    GRUB_KEYBOARD_KEY_DELETE = 0x4c,
    GRUB_KEYBOARD_KEY_END = 0x4d,
    GRUB_KEYBOARD_KEY_NPAGE = 0x4e,
    GRUB_KEYBOARD_KEY_RIGHT = 0x4f,
    GRUB_KEYBOARD_KEY_LEFT = 0x50,
    GRUB_KEYBOARD_KEY_DOWN = 0x51,
    GRUB_KEYBOARD_KEY_UP = 0x52,
    GRUB_KEYBOARD_KEY_NUM_LOCK = 0x53,
    GRUB_KEYBOARD_KEY_NUMSLASH = 0x54,
    GRUB_KEYBOARD_KEY_NUMMUL = 0x55,
    GRUB_KEYBOARD_KEY_NUMMINUS = 0x56,
    GRUB_KEYBOARD_KEY_NUMPLUS = 0x57,
    GRUB_KEYBOARD_KEY_NUMENTER = 0x58,
    GRUB_KEYBOARD_KEY_NUM1 = 0x59,
    GRUB_KEYBOARD_KEY_NUM2 = 0x5a,
    GRUB_KEYBOARD_KEY_NUM3 = 0x5b,
    GRUB_KEYBOARD_KEY_NUM4 = 0x5c,
    GRUB_KEYBOARD_KEY_NUM5 = 0x5d,
    GRUB_KEYBOARD_KEY_NUM6 = 0x5e,
    GRUB_KEYBOARD_KEY_NUM7 = 0x5f,
    GRUB_KEYBOARD_KEY_NUM8 = 0x60,
    GRUB_KEYBOARD_KEY_NUM9 = 0x61,
    GRUB_KEYBOARD_KEY_NUM0 = 0x62,
    GRUB_KEYBOARD_KEY_NUMDOT = 0x63,
    GRUB_KEYBOARD_KEY_102ND = 0x64,
    GRUB_KEYBOARD_KEY_LEFT_CTRL = 0xe0,
    GRUB_KEYBOARD_KEY_LEFT_SHIFT = 0xe1,
    GRUB_KEYBOARD_KEY_LEFT_ALT = 0xe2,
    GRUB_KEYBOARD_KEY_RIGHT_CTRL = 0xe4,
    GRUB_KEYBOARD_KEY_RIGHT_SHIFT = 0xe5,
    GRUB_KEYBOARD_KEY_RIGHT_ALT = 0xe6,
  } grub_keyboard_key_t;

unsigned EXPORT_FUNC(grub_term_map_key) (grub_keyboard_key_t code, int status);

#ifndef GRUB_MACHINE_EMU
extern void grub_keylayouts_init (void);
extern void grub_keylayouts_fini (void);
#endif

#endif /* GRUB_KEYBOARD_LAYOUTS  */
