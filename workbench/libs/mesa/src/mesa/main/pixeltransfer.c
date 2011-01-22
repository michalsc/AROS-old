/*
 * Mesa 3-D graphics library
 *
 * Copyright (C) 1999-2008  Brian Paul   All Rights Reserved.
 * Copyright (C) 2009-2010  VMware, Inc.  All Rights Reserved.
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
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


/**
 * \file pixeltransfer.c
 * Pixel transfer operations (scale, bias, table lookups, etc)
 */


#include "glheader.h"
#include "colormac.h"
#include "pixeltransfer.h"
#include "imports.h"


/*
 * Apply scale and bias factors to an array of RGBA pixels.
 */
void
_mesa_scale_and_bias_rgba(GLuint n, GLfloat rgba[][4],
                          GLfloat rScale, GLfloat gScale,
                          GLfloat bScale, GLfloat aScale,
                          GLfloat rBias, GLfloat gBias,
                          GLfloat bBias, GLfloat aBias)
{
   if (rScale != 1.0 || rBias != 0.0) {
      GLuint i;
      for (i = 0; i < n; i++) {
         rgba[i][RCOMP] = rgba[i][RCOMP] * rScale + rBias;
      }
   }
   if (gScale != 1.0 || gBias != 0.0) {
      GLuint i;
      for (i = 0; i < n; i++) {
         rgba[i][GCOMP] = rgba[i][GCOMP] * gScale + gBias;
      }
   }
   if (bScale != 1.0 || bBias != 0.0) {
      GLuint i;
      for (i = 0; i < n; i++) {
         rgba[i][BCOMP] = rgba[i][BCOMP] * bScale + bBias;
      }
   }
   if (aScale != 1.0 || aBias != 0.0) {
      GLuint i;
      for (i = 0; i < n; i++) {
         rgba[i][ACOMP] = rgba[i][ACOMP] * aScale + aBias;
      }
   }
}


/*
 * Apply pixel mapping to an array of floating point RGBA pixels.
 */
void
_mesa_map_rgba( const struct gl_context *ctx, GLuint n, GLfloat rgba[][4] )
{
   const GLfloat rscale = (GLfloat) (ctx->PixelMaps.RtoR.Size - 1);
   const GLfloat gscale = (GLfloat) (ctx->PixelMaps.GtoG.Size - 1);
   const GLfloat bscale = (GLfloat) (ctx->PixelMaps.BtoB.Size - 1);
   const GLfloat ascale = (GLfloat) (ctx->PixelMaps.AtoA.Size - 1);
   const GLfloat *rMap = ctx->PixelMaps.RtoR.Map;
   const GLfloat *gMap = ctx->PixelMaps.GtoG.Map;
   const GLfloat *bMap = ctx->PixelMaps.BtoB.Map;
   const GLfloat *aMap = ctx->PixelMaps.AtoA.Map;
   GLuint i;
   for (i=0;i<n;i++) {
      GLfloat r = CLAMP(rgba[i][RCOMP], 0.0F, 1.0F);
      GLfloat g = CLAMP(rgba[i][GCOMP], 0.0F, 1.0F);
      GLfloat b = CLAMP(rgba[i][BCOMP], 0.0F, 1.0F);
      GLfloat a = CLAMP(rgba[i][ACOMP], 0.0F, 1.0F);
      rgba[i][RCOMP] = rMap[IROUND(r * rscale)];
      rgba[i][GCOMP] = gMap[IROUND(g * gscale)];
      rgba[i][BCOMP] = bMap[IROUND(b * bscale)];
      rgba[i][ACOMP] = aMap[IROUND(a * ascale)];
   }
}

/**
 * Apply a color table lookup to an array of floating point RGBA colors.
 */
void
_mesa_lookup_rgba_float(const struct gl_color_table *table,
                        GLuint n, GLfloat rgba[][4])
{
   const GLint max = table->Size - 1;
   const GLfloat scale = (GLfloat) max;
   const GLfloat *lut = table->TableF;
   GLuint i;

   if (!table->TableF || table->Size == 0)
      return;

   switch (table->_BaseFormat) {
      case GL_INTENSITY:
         /* replace RGBA with I */
         for (i = 0; i < n; i++) {
            GLint j = IROUND(rgba[i][RCOMP] * scale);
            GLfloat c = lut[CLAMP(j, 0, max)];
            rgba[i][RCOMP] =
            rgba[i][GCOMP] =
            rgba[i][BCOMP] =
            rgba[i][ACOMP] = c;
         }
         break;
      case GL_LUMINANCE:
         /* replace RGB with L */
         for (i = 0; i < n; i++) {
            GLint j = IROUND(rgba[i][RCOMP] * scale);
            GLfloat c = lut[CLAMP(j, 0, max)];
            rgba[i][RCOMP] =
            rgba[i][GCOMP] =
            rgba[i][BCOMP] = c;
         }
         break;
      case GL_ALPHA:
         /* replace A with A */
         for (i = 0; i < n; i++) {
            GLint j = IROUND(rgba[i][ACOMP] * scale);
            rgba[i][ACOMP] = lut[CLAMP(j, 0, max)];
         }
         break;
      case GL_LUMINANCE_ALPHA:
         /* replace RGBA with LLLA */
         for (i = 0; i < n; i++) {
            GLint jL = IROUND(rgba[i][RCOMP] * scale);
            GLint jA = IROUND(rgba[i][ACOMP] * scale);
            GLfloat luminance, alpha;
            jL = CLAMP(jL, 0, max);
            jA = CLAMP(jA, 0, max);
            luminance = lut[jL * 2 + 0];
            alpha     = lut[jA * 2 + 1];
            rgba[i][RCOMP] =
            rgba[i][GCOMP] =
            rgba[i][BCOMP] = luminance;
            rgba[i][ACOMP] = alpha;;
         }
         break;
      case GL_RED:
         /* replace RGB with RGB */
         for (i = 0; i < n; i++) {
            GLint jR = IROUND(rgba[i][RCOMP] * scale);
            jR = CLAMP(jR, 0, max);
            rgba[i][RCOMP] = lut[jR * 3 + 0];
         }
         break;
      case GL_RG:
         /* replace RG with RG */
         for (i = 0; i < n; i++) {
            GLint jR = IROUND(rgba[i][RCOMP] * scale);
            GLint jG = IROUND(rgba[i][GCOMP] * scale);
            jR = CLAMP(jR, 0, max);
            jG = CLAMP(jG, 0, max);
            rgba[i][RCOMP] = lut[jR * 3 + 0];
            rgba[i][GCOMP] = lut[jG * 3 + 1];
         }
         break;
      case GL_RGB:
         /* replace RGB with RGB */
         for (i = 0; i < n; i++) {
            GLint jR = IROUND(rgba[i][RCOMP] * scale);
            GLint jG = IROUND(rgba[i][GCOMP] * scale);
            GLint jB = IROUND(rgba[i][BCOMP] * scale);
            jR = CLAMP(jR, 0, max);
            jG = CLAMP(jG, 0, max);
            jB = CLAMP(jB, 0, max);
            rgba[i][RCOMP] = lut[jR * 3 + 0];
            rgba[i][GCOMP] = lut[jG * 3 + 1];
            rgba[i][BCOMP] = lut[jB * 3 + 2];
         }
         break;
      case GL_RGBA:
         /* replace RGBA with RGBA */
         for (i = 0; i < n; i++) {
            GLint jR = IROUND(rgba[i][RCOMP] * scale);
            GLint jG = IROUND(rgba[i][GCOMP] * scale);
            GLint jB = IROUND(rgba[i][BCOMP] * scale);
            GLint jA = IROUND(rgba[i][ACOMP] * scale);
            jR = CLAMP(jR, 0, max);
            jG = CLAMP(jG, 0, max);
            jB = CLAMP(jB, 0, max);
            jA = CLAMP(jA, 0, max);
            rgba[i][RCOMP] = lut[jR * 4 + 0];
            rgba[i][GCOMP] = lut[jG * 4 + 1];
            rgba[i][BCOMP] = lut[jB * 4 + 2];
            rgba[i][ACOMP] = lut[jA * 4 + 3];
         }
         break;
      default:
         _mesa_problem(NULL, "Bad format in _mesa_lookup_rgba_float");
         return;
   }
}



/**
 * Apply a color table lookup to an array of ubyte/RGBA colors.
 */
void
_mesa_lookup_rgba_ubyte(const struct gl_color_table *table,
                        GLuint n, GLubyte rgba[][4])
{
   const GLubyte *lut = table->TableUB;
   const GLfloat scale = (GLfloat) (table->Size - 1) / (GLfloat)255.0;
   GLuint i;

   if (!table->TableUB || table->Size == 0)
      return;

   switch (table->_BaseFormat) {
   case GL_INTENSITY:
      /* replace RGBA with I */
      if (table->Size == 256) {
         for (i = 0; i < n; i++) {
            const GLubyte c = lut[rgba[i][RCOMP]];
            rgba[i][RCOMP] =
            rgba[i][GCOMP] =
            rgba[i][BCOMP] =
            rgba[i][ACOMP] = c;
         }
      }
      else {
         for (i = 0; i < n; i++) {
            GLint j = IROUND((GLfloat) rgba[i][RCOMP] * scale);
            rgba[i][RCOMP] =
            rgba[i][GCOMP] =
            rgba[i][BCOMP] =
            rgba[i][ACOMP] = lut[j];
         }
      }
      break;
   case GL_LUMINANCE:
      /* replace RGB with L */
      if (table->Size == 256) {
         for (i = 0; i < n; i++) {
            const GLubyte c = lut[rgba[i][RCOMP]];
            rgba[i][RCOMP] =
            rgba[i][GCOMP] =
            rgba[i][BCOMP] = c;
         }
      }
      else {
         for (i = 0; i < n; i++) {
            GLint j = IROUND((GLfloat) rgba[i][RCOMP] * scale);
            rgba[i][RCOMP] =
            rgba[i][GCOMP] =
            rgba[i][BCOMP] = lut[j];
         }
      }
      break;
   case GL_ALPHA:
      /* replace A with A */
      if (table->Size == 256) {
         for (i = 0; i < n; i++) {
            rgba[i][ACOMP] = lut[rgba[i][ACOMP]];
         }
      }
      else {
         for (i = 0; i < n; i++) {
            GLint j = IROUND((GLfloat) rgba[i][ACOMP] * scale);
            rgba[i][ACOMP] = lut[j];
         }
      }
      break;
   case GL_LUMINANCE_ALPHA:
      /* replace RGBA with LLLA */
      if (table->Size == 256) {
         for (i = 0; i < n; i++) {
            GLubyte l = lut[rgba[i][RCOMP] * 2 + 0];
            GLubyte a = lut[rgba[i][ACOMP] * 2 + 1];;
            rgba[i][RCOMP] =
            rgba[i][GCOMP] =
            rgba[i][BCOMP] = l;
            rgba[i][ACOMP] = a;
         }
      }
      else {
         for (i = 0; i < n; i++) {
            GLint jL = IROUND((GLfloat) rgba[i][RCOMP] * scale);
            GLint jA = IROUND((GLfloat) rgba[i][ACOMP] * scale);
            GLubyte luminance = lut[jL * 2 + 0];
            GLubyte alpha     = lut[jA * 2 + 1];
            rgba[i][RCOMP] =
            rgba[i][GCOMP] =
            rgba[i][BCOMP] = luminance;
            rgba[i][ACOMP] = alpha;
         }
      }
      break;
   case GL_RGB:
      if (table->Size == 256) {
         for (i = 0; i < n; i++) {
            rgba[i][RCOMP] = lut[rgba[i][RCOMP] * 3 + 0];
            rgba[i][GCOMP] = lut[rgba[i][GCOMP] * 3 + 1];
            rgba[i][BCOMP] = lut[rgba[i][BCOMP] * 3 + 2];
         }
      }
      else {
         for (i = 0; i < n; i++) {
            GLint jR = IROUND((GLfloat) rgba[i][RCOMP] * scale);
            GLint jG = IROUND((GLfloat) rgba[i][GCOMP] * scale);
            GLint jB = IROUND((GLfloat) rgba[i][BCOMP] * scale);
            rgba[i][RCOMP] = lut[jR * 3 + 0];
            rgba[i][GCOMP] = lut[jG * 3 + 1];
            rgba[i][BCOMP] = lut[jB * 3 + 2];
         }
      }
      break;
   case GL_RGBA:
      if (table->Size == 256) {
         for (i = 0; i < n; i++) {
            rgba[i][RCOMP] = lut[rgba[i][RCOMP] * 4 + 0];
            rgba[i][GCOMP] = lut[rgba[i][GCOMP] * 4 + 1];
            rgba[i][BCOMP] = lut[rgba[i][BCOMP] * 4 + 2];
            rgba[i][ACOMP] = lut[rgba[i][ACOMP] * 4 + 3];
         }
      }
      else {
         for (i = 0; i < n; i++) {
            GLint jR = IROUND((GLfloat) rgba[i][RCOMP] * scale);
            GLint jG = IROUND((GLfloat) rgba[i][GCOMP] * scale);
            GLint jB = IROUND((GLfloat) rgba[i][BCOMP] * scale);
            GLint jA = IROUND((GLfloat) rgba[i][ACOMP] * scale);
            CLAMPED_FLOAT_TO_CHAN(rgba[i][RCOMP], lut[jR * 4 + 0]);
            CLAMPED_FLOAT_TO_CHAN(rgba[i][GCOMP], lut[jG * 4 + 1]);
            CLAMPED_FLOAT_TO_CHAN(rgba[i][BCOMP], lut[jB * 4 + 2]);
            CLAMPED_FLOAT_TO_CHAN(rgba[i][ACOMP], lut[jA * 4 + 3]);
         }
      }
      break;
   default:
      _mesa_problem(NULL, "Bad format in _mesa_lookup_rgba_chan");
      return;
   }
}



/*
 * Map color indexes to float rgba values.
 */
void
_mesa_map_ci_to_rgba( const struct gl_context *ctx, GLuint n,
                      const GLuint index[], GLfloat rgba[][4] )
{
   GLuint rmask = ctx->PixelMaps.ItoR.Size - 1;
   GLuint gmask = ctx->PixelMaps.ItoG.Size - 1;
   GLuint bmask = ctx->PixelMaps.ItoB.Size - 1;
   GLuint amask = ctx->PixelMaps.ItoA.Size - 1;
   const GLfloat *rMap = ctx->PixelMaps.ItoR.Map;
   const GLfloat *gMap = ctx->PixelMaps.ItoG.Map;
   const GLfloat *bMap = ctx->PixelMaps.ItoB.Map;
   const GLfloat *aMap = ctx->PixelMaps.ItoA.Map;
   GLuint i;
   for (i=0;i<n;i++) {
      rgba[i][RCOMP] = rMap[index[i] & rmask];
      rgba[i][GCOMP] = gMap[index[i] & gmask];
      rgba[i][BCOMP] = bMap[index[i] & bmask];
      rgba[i][ACOMP] = aMap[index[i] & amask];
   }
}


/**
 * Map ubyte color indexes to ubyte/RGBA values.
 */
void
_mesa_map_ci8_to_rgba8(const struct gl_context *ctx,
                       GLuint n, const GLubyte index[],
                       GLubyte rgba[][4])
{
   GLuint rmask = ctx->PixelMaps.ItoR.Size - 1;
   GLuint gmask = ctx->PixelMaps.ItoG.Size - 1;
   GLuint bmask = ctx->PixelMaps.ItoB.Size - 1;
   GLuint amask = ctx->PixelMaps.ItoA.Size - 1;
   const GLubyte *rMap = ctx->PixelMaps.ItoR.Map8;
   const GLubyte *gMap = ctx->PixelMaps.ItoG.Map8;
   const GLubyte *bMap = ctx->PixelMaps.ItoB.Map8;
   const GLubyte *aMap = ctx->PixelMaps.ItoA.Map8;
   GLuint i;
   for (i=0;i<n;i++) {
      rgba[i][RCOMP] = rMap[index[i] & rmask];
      rgba[i][GCOMP] = gMap[index[i] & gmask];
      rgba[i][BCOMP] = bMap[index[i] & bmask];
      rgba[i][ACOMP] = aMap[index[i] & amask];
   }
}


void
_mesa_scale_and_bias_depth(const struct gl_context *ctx, GLuint n,
                           GLfloat depthValues[])
{
   const GLfloat scale = ctx->Pixel.DepthScale;
   const GLfloat bias = ctx->Pixel.DepthBias;
   GLuint i;
   for (i = 0; i < n; i++) {
      GLfloat d = depthValues[i] * scale + bias;
      depthValues[i] = CLAMP(d, 0.0F, 1.0F);
   }
}


void
_mesa_scale_and_bias_depth_uint(const struct gl_context *ctx, GLuint n,
                                GLuint depthValues[])
{
   const GLdouble max = (double) 0xffffffff;
   const GLdouble scale = ctx->Pixel.DepthScale;
   const GLdouble bias = ctx->Pixel.DepthBias * max;
   GLuint i;
   for (i = 0; i < n; i++) {
      GLdouble d = (GLdouble) depthValues[i] * scale + bias;
      d = CLAMP(d, 0.0, max);
      depthValues[i] = (GLuint) d;
   }
}

/**
 * Apply various pixel transfer operations to an array of RGBA pixels
 * as indicated by the transferOps bitmask
 */
void
_mesa_apply_rgba_transfer_ops(struct gl_context *ctx, GLbitfield transferOps,
                              GLuint n, GLfloat rgba[][4])
{
   /* scale & bias */
   if (transferOps & IMAGE_SCALE_BIAS_BIT) {
      _mesa_scale_and_bias_rgba(n, rgba,
                                ctx->Pixel.RedScale, ctx->Pixel.GreenScale,
                                ctx->Pixel.BlueScale, ctx->Pixel.AlphaScale,
                                ctx->Pixel.RedBias, ctx->Pixel.GreenBias,
                                ctx->Pixel.BlueBias, ctx->Pixel.AlphaBias);
   }
   /* color map lookup */
   if (transferOps & IMAGE_MAP_COLOR_BIT) {
      _mesa_map_rgba( ctx, n, rgba );
   }

   /* clamping to [0,1] */
   if (transferOps & IMAGE_CLAMP_BIT) {
      GLuint i;
      for (i = 0; i < n; i++) {
         rgba[i][RCOMP] = CLAMP(rgba[i][RCOMP], 0.0F, 1.0F);
         rgba[i][GCOMP] = CLAMP(rgba[i][GCOMP], 0.0F, 1.0F);
         rgba[i][BCOMP] = CLAMP(rgba[i][BCOMP], 0.0F, 1.0F);
         rgba[i][ACOMP] = CLAMP(rgba[i][ACOMP], 0.0F, 1.0F);
      }
   }
}


/*
 * Apply color index shift and offset to an array of pixels.
 */
void
_mesa_shift_and_offset_ci(const struct gl_context *ctx,
                          GLuint n, GLuint indexes[])
{
   GLint shift = ctx->Pixel.IndexShift;
   GLint offset = ctx->Pixel.IndexOffset;
   GLuint i;
   if (shift > 0) {
      for (i=0;i<n;i++) {
         indexes[i] = (indexes[i] << shift) + offset;
      }
   }
   else if (shift < 0) {
      shift = -shift;
      for (i=0;i<n;i++) {
         indexes[i] = (indexes[i] >> shift) + offset;
      }
   }
   else {
      for (i=0;i<n;i++) {
         indexes[i] = indexes[i] + offset;
      }
   }
}



/**
 * Apply color index shift, offset and table lookup to an array
 * of color indexes;
 */
void
_mesa_apply_ci_transfer_ops(const struct gl_context *ctx,
                            GLbitfield transferOps,
                            GLuint n, GLuint indexes[])
{
   if (transferOps & IMAGE_SHIFT_OFFSET_BIT) {
      _mesa_shift_and_offset_ci(ctx, n, indexes);
   }
   if (transferOps & IMAGE_MAP_COLOR_BIT) {
      const GLuint mask = ctx->PixelMaps.ItoI.Size - 1;
      GLuint i;
      for (i = 0; i < n; i++) {
         const GLuint j = indexes[i] & mask;
         indexes[i] = IROUND(ctx->PixelMaps.ItoI.Map[j]);
      }
   }
}


/**
 * Apply stencil index shift, offset and table lookup to an array
 * of stencil values.
 */
void
_mesa_apply_stencil_transfer_ops(const struct gl_context *ctx, GLuint n,
                                 GLstencil stencil[])
{
   if (ctx->Pixel.IndexShift != 0 || ctx->Pixel.IndexOffset != 0) {
      const GLint offset = ctx->Pixel.IndexOffset;
      GLint shift = ctx->Pixel.IndexShift;
      GLuint i;
      if (shift > 0) {
         for (i = 0; i < n; i++) {
            stencil[i] = (stencil[i] << shift) + offset;
         }
      }
      else if (shift < 0) {
         shift = -shift;
         for (i = 0; i < n; i++) {
            stencil[i] = (stencil[i] >> shift) + offset;
         }
      }
      else {
         for (i = 0; i < n; i++) {
            stencil[i] = stencil[i] + offset;
         }
      }
   }
   if (ctx->Pixel.MapStencilFlag) {
      GLuint mask = ctx->PixelMaps.StoS.Size - 1;
      GLuint i;
      for (i = 0; i < n; i++) {
         stencil[i] = (GLstencil)ctx->PixelMaps.StoS.Map[ stencil[i] & mask ];
      }
   }
}
