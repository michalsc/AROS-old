/* serial.h - serial device interface */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2000, 2001  Free Software Foundation, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef GRUB_SERIAL_HEADER
#define GRUB_SERIAL_HEADER	1

/* Macros.  */

/* The offsets of UART registers.  */
#define UART_TX		0
#define UART_RX		0
#define UART_DLL	0
#define UART_IER	1
#define UART_DLH	1
#define UART_IIR	2
#define UART_FCR	2
#define UART_LCR	3
#define UART_MCR	4
#define UART_LSR	5
#define UART_MSR	6
#define UART_SR		7

/* For LSR bits.  */
#define UART_DATA_READY		0x01
#define UART_EMPTY_TRANSMITTER	0x20

/* The type of parity.  */
#define UART_NO_PARITY		0x00
#define UART_ODD_PARITY		0x08
#define UART_EVEN_PARITY	0x18

/* The type of word length.  */
#define UART_5BITS_WORD	0x00
#define UART_6BITS_WORD	0x01
#define UART_7BITS_WORD	0x02
#define UART_8BITS_WORD	0x03

/* The type of the length of stop bit.  */
#define UART_1_STOP_BIT		0x00
#define UART_2_STOP_BITS	0x04

/* the switch of DLAB.  */
#define UART_DLAB	0x80

/* Enable the FIFO.  */
#define UART_ENABLE_FIFO	0xC7

/* Turn on DTR, RTS, and OUT2.  */
#define UART_ENABLE_MODEM	0x0B


/* Function prototypes.  */

/* The serial part of grub_putchar.  */
void serial_putchar (int c);

/* The serial part of getkey.  */
int serial_getkey (void);

/* The serial part of checkkey.  */
int serial_checkkey (void);

/* The serial part of cls.  */
void serial_cls (void);

/* The serial part of getxy.  */
int serial_getxy (void);

/* The serial part of gotoxy.  */
void serial_gotoxy (int x, int y);

/* Return the port number for the UNITth serial device.  */
unsigned short serial_get_port (int unit);

/* Initialize a serial device.  */
int serial_init (unsigned short port, unsigned int speed,
		 int word_len, int parity, int stop_bit_len);

/* Check if a serial port is set up.  */
int serial_exists (void);

#ifdef GRUB_UTIL
/* Set the file name of a serial device (or a pty device). This is a
   function specific to the grub shell.  */
void set_serial_device (const char *device);
#endif /* GRUB_UTIL */

#endif /* ! GRUB_SERIAL_HEADER */
