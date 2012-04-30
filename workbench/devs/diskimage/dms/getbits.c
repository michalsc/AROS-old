/*
 *     xDMS  v1.3  -  Portable DMS archive unpacker  -  Public Domain
 *     Written by     Andre Rodrigues de la Rocha  <adlroc@usa.net>
 *     Functions/macros to get a variable number of bits
 * 
 */

#include "xdms.h"

const ULONG mask_bits[]={
	0x000000L,0x000001L,0x000003L,0x000007L,0x00000fL,0x00001fL,
	0x00003fL,0x00007fL,0x0000ffL,0x0001ffL,0x0003ffL,0x0007ffL,
	0x000fffL,0x001fffL,0x003fffL,0x007fffL,0x00ffffL,0x01ffffL,
	0x03ffffL,0x07ffffL,0x0fffffL,0x1fffffL,0x3fffffL,0x7fffffL,
	0xffffffL
};

void initbitbuf (struct xdms_data *xdms, UBYTE *in) {
	xdms->bitbuf = 0;
	xdms->bitcount = 0;
	xdms->indata = in;
	DROPBITS(0);
}
