/* This file was automatically imported with 
   import_gcry.py. Please don't modify it */
#include <grub/dl.h>
GRUB_MOD_LICENSE ("GPLv3+");
/* idea.c  -  IDEA function
 * Copyright 1997, 1998, 1999, 2001 Werner Koch (dd9jn)
 * Copyright 2013 g10 Code GmbH
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * WERNER KOCH BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Werner Koch shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from Werner Koch.
 *
 * Patents on IDEA have expired:
 *   Europe: EP0482154 on 2011-05-16,
 *   Japan:  JP3225440 on 2011-05-16,
 *   U.S.:   5,214,703 on 2012-01-07.
 */

/*
 * Please see http://www.noepatents.org/ to learn why software patents
 * are bad for society and what you can do to fight them.
 *
 * The code herein is based on the one from:
 *   Bruce Schneier: Applied Cryptography. John Wiley & Sons, 1996.
 *   ISBN 0-471-11709-9.
 */



#include "types.h"  /* for byte and u32 typedefs */
#include "g10lib.h"
#include "cipher.h"


#define IDEA_KEYSIZE 16
#define IDEA_BLOCKSIZE 8
#define IDEA_ROUNDS 8
#define IDEA_KEYLEN (6*IDEA_ROUNDS+4)

typedef struct {
    u16 ek[IDEA_KEYLEN];
    u16 dk[IDEA_KEYLEN];
    int have_dk;
} IDEA_context;



static u16
mul_inv( u16 x )
{
    u16 t0, t1;
    u16 q, y;

    if( x < 2 )
	return x;
    t1 = 0x10001UL / x;
    y =  0x10001UL % x;
    if( y == 1 )
	return (1-t1) & 0xffff;

    t0 = 1;
    do {
	q = x / y;
	x = x % y;
	t0 += q * t1;
	if( x == 1 )
	    return t0;
	q = y / x;
	y = y % x;
	t1 += q * t0;
    } while( y != 1 );
    return (1-t1) & 0xffff;
}



static void
expand_key( const byte *userkey, u16 *ek )
{
    int i,j;

    for(j=0; j < 8; j++ ) {
	ek[j] = (*userkey << 8) + userkey[1];
	userkey += 2;
    }
    for(i=0; j < IDEA_KEYLEN; j++ ) {
	i++;
	ek[i+7] = ek[i&7] << 9 | ek[(i+1)&7] >> 7;
	ek += i & 8;
	i &= 7;
    }
}


static void
invert_key( u16 *ek, u16 dk[IDEA_KEYLEN] )
{
    int i;
    u16 t1, t2, t3;
    u16 temp[IDEA_KEYLEN];
    u16 *p = temp + IDEA_KEYLEN;

    t1 = mul_inv( *ek++ );
    t2 = -*ek++;
    t3 = -*ek++;
    *--p = mul_inv( *ek++ );
    *--p = t3;
    *--p = t2;
    *--p = t1;

    for(i=0; i < IDEA_ROUNDS-1; i++ ) {
	t1 = *ek++;
	*--p = *ek++;
	*--p = t1;

	t1 = mul_inv( *ek++ );
	t2 = -*ek++;
	t3 = -*ek++;
	*--p = mul_inv( *ek++ );
	*--p = t2;
	*--p = t3;
	*--p = t1;
    }
    t1 = *ek++;
    *--p = *ek++;
    *--p = t1;

    t1 = mul_inv( *ek++ );
    t2 = -*ek++;
    t3 = -*ek++;
    *--p = mul_inv( *ek++ );
    *--p = t3;
    *--p = t2;
    *--p = t1;
    memcpy(dk, temp, sizeof(temp) );
    memset(temp, 0, sizeof(temp) );  /* burn temp */
}


static void
cipher( byte *outbuf, const byte *inbuf, u16 *key )
{
    u16 s2, s3;
    u16 in[4];
    int r = IDEA_ROUNDS;
#define x1 (in[0])
#define x2 (in[1])
#define x3 (in[2])
#define x4 (in[3])
#define MUL(x,y) \
	do {u16 _t16; u32 _t32; 		    \
	    if( (_t16 = (y)) ) {		    \
		if( (x = (x)&0xffff) ) {	    \
		    _t32 = (u32)x * _t16;	    \
		    x = _t32 & 0xffff;		    \
		    _t16 = _t32 >> 16;		    \
		    x = ((x)-_t16) + (x<_t16?1:0);  \
		}				    \
		else {				    \
		    x = 1 - _t16;		    \
		}				    \
	    }					    \
	    else {				    \
		x = 1 - x;			    \
	    }					    \
	} while(0)

    memcpy (in, inbuf, sizeof in);
#ifndef WORDS_BIGENDIAN
    x1 = (x1>>8) | (x1<<8);
    x2 = (x2>>8) | (x2<<8);
    x3 = (x3>>8) | (x3<<8);
    x4 = (x4>>8) | (x4<<8);
#endif
    do {
	MUL(x1, *key++);
	x2 += *key++;
	x3 += *key++;
	MUL(x4, *key++ );

	s3 = x3;
	x3 ^= x1;
	MUL(x3, *key++);
	s2 = x2;
	x2 ^=x4;
	x2 += x3;
	MUL(x2, *key++);
	x3 += x2;

	x1 ^= x2;
	x4 ^= x3;

	x2 ^= s3;
	x3 ^= s2;
    } while( --r );
    MUL(x1, *key++);
    x3 += *key++;
    x2 += *key++;
    MUL(x4, *key);

#ifndef WORDS_BIGENDIAN
    x1 = (x1>>8) | (x1<<8);
    x2 = (x2>>8) | (x2<<8);
    x3 = (x3>>8) | (x3<<8);
    x4 = (x4>>8) | (x4<<8);
#endif
    memcpy (outbuf+0, &x1, 2);
    memcpy (outbuf+2, &x3, 2);
    memcpy (outbuf+4, &x2, 2);
    memcpy (outbuf+6, &x4, 2);
#undef MUL
#undef x1
#undef x2
#undef x3
#undef x4
}


static int
do_setkey( IDEA_context *c, const byte *key, unsigned int keylen )
{
    static int initialized = 0;
    static const char *selftest_failed = 0;

    if( !initialized ) {
	initialized = 1;
	selftest_failed = selftest();
	if( selftest_failed )
	    log_error( "%s\n", selftest_failed );
    }
    if( selftest_failed )
	return GPG_ERR_SELFTEST_FAILED;

    assert(keylen == 16);
    c->have_dk = 0;
    expand_key( key, c->ek );
    invert_key( c->ek, c->dk );
    return 0;
}

static gcry_err_code_t
idea_setkey (void *context, const byte *key, unsigned int keylen)
{
    IDEA_context *ctx = context;
    int rc = do_setkey (ctx, key, keylen);
    _gcry_burn_stack (23+6*sizeof(void*));
    return rc;
}

static void
encrypt_block( IDEA_context *c, byte *outbuf, const byte *inbuf )
{
    cipher( outbuf, inbuf, c->ek );
}

static void
idea_encrypt (void *context, byte *out, const byte *in)
{
    IDEA_context *ctx = context;
    encrypt_block (ctx, out, in);
    _gcry_burn_stack (24+3*sizeof (void*));
}

static void
decrypt_block( IDEA_context *c, byte *outbuf, const byte *inbuf )
{
    if( !c->have_dk ) {
       c->have_dk = 1;
       invert_key( c->ek, c->dk );
    }
    cipher( outbuf, inbuf, c->dk );
}

static void
idea_decrypt (void *context, byte *out, const byte *in)
{
    IDEA_context *ctx = context;
    decrypt_block (ctx, out, in);
    _gcry_burn_stack (24+3*sizeof (void*));
}




gcry_cipher_spec_t _gcry_cipher_spec_idea =
{
    "IDEA", NULL, NULL, IDEA_BLOCKSIZE, 128,
    sizeof (IDEA_context),
    idea_setkey, idea_encrypt, idea_decrypt
    ,
#ifdef GRUB_UTIL
    .modname = "gcry_idea",
#endif
};


GRUB_MOD_INIT(gcry_idea)
{
  grub_cipher_register (&_gcry_cipher_spec_idea);
}

GRUB_MOD_FINI(gcry_idea)
{
  grub_cipher_unregister (&_gcry_cipher_spec_idea);
}
