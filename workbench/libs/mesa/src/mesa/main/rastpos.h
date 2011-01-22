/**
 * \file rastpos.h
 * Raster position operations.
 */

/*
 * Mesa 3-D graphics library
 * Version:  4.1
 *
 * Copyright (C) 1999-2002  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef RASTPOS_H
#define RASTPOS_H


#include "compiler.h"
#include "mfeatures.h"

struct _glapi_table;
struct gl_context;

#if FEATURE_rastpos

extern void
_mesa_init_rastpos_dispatch(struct _glapi_table *disp);

#else /* FEATURE_rastpos */

static INLINE void
_mesa_init_rastpos_dispatch(struct _glapi_table *disp)
{
}

#endif /* FEATURE_rastpos */

extern void 
_mesa_init_rastpos(struct gl_context *ctx);

/*@}*/

#endif /* RASTPOS_H */
