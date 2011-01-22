/**************************************************************************
 * 
 * Copyright 2007 Tungsten Graphics, Inc., Cedar Park, Texas.
 * All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL TUNGSTEN GRAPHICS AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 **************************************************************************/

#include <stdio.h>

#include "st_context.h"
#include "st_format.h"
#include "st_texture.h"
#include "st_cb_fbo.h"
#include "main/enums.h"

#include "pipe/p_state.h"
#include "pipe/p_context.h"
#include "pipe/p_defines.h"
#include "util/u_inlines.h"
#include "util/u_format.h"
#include "util/u_rect.h"
#include "util/u_math.h"


#define DBG if(0) printf


/**
 * Allocate a new pipe_resource object
 * width0, height0, depth0 are the dimensions of the level 0 image
 * (the highest resolution).  last_level indicates how many mipmap levels
 * to allocate storage for.  For non-mipmapped textures, this will be zero.
 */
struct pipe_resource *
st_texture_create(struct st_context *st,
                  enum pipe_texture_target target,
		  enum pipe_format format,
		  GLuint last_level,
		  GLuint width0,
		  GLuint height0,
		  GLuint depth0,
                  GLuint bind )
{
   struct pipe_resource pt, *newtex;
   struct pipe_screen *screen = st->pipe->screen;

   assert(target < PIPE_MAX_TEXTURE_TYPES);
   assert(width0 > 0);
   assert(height0 > 0);
   assert(depth0 > 0);

   DBG("%s target %s format %s last_level %d\n", __FUNCTION__,
       _mesa_lookup_enum_by_nr(target),
       _mesa_lookup_enum_by_nr(format), last_level);

   assert(format);
   assert(screen->is_format_supported(screen, format, target, 0,
                                      PIPE_BIND_SAMPLER_VIEW, 0));

   memset(&pt, 0, sizeof(pt));
   pt.target = target;
   pt.format = format;
   pt.last_level = last_level;
   pt.width0 = width0;
   pt.height0 = height0;
   pt.depth0 = depth0;
   pt.array_size = (target == PIPE_TEXTURE_CUBE ? 6 : 1);
   pt.usage = PIPE_USAGE_DEFAULT;
   pt.bind = bind;
   pt.flags = 0;

   newtex = screen->resource_create(screen, &pt);

   assert(!newtex || pipe_is_referenced(&newtex->reference));

   return newtex;
}


/**
 * Check if a texture image can be pulled into a unified mipmap texture.
 */
GLboolean
st_texture_match_image(const struct pipe_resource *pt,
                       const struct gl_texture_image *image,
                       GLuint face, GLuint level)
{
   /* Images with borders are never pulled into mipmap textures. 
    */
   if (image->Border) 
      return GL_FALSE;

   /* Check if this image's format matches the established texture's format.
    */
   if (st_mesa_format_to_pipe_format(image->TexFormat) != pt->format)
      return GL_FALSE;

   /* Test if this image's size matches what's expected in the
    * established texture.
    */
   if (image->Width != u_minify(pt->width0, level) ||
       image->Height != u_minify(pt->height0, level) ||
       image->Depth != u_minify(pt->depth0, level))
      return GL_FALSE;

   return GL_TRUE;
}


/**
 * Map a texture image and return the address for a particular 2D face/slice/
 * layer.  The stImage indicates the cube face and mipmap level.  The slice
 * of the 3D texture is passed in 'zoffset'.
 * \param usage  one of the PIPE_TRANSFER_x values
 * \param x, y, w, h  the region of interest of the 2D image.
 * \return address of mapping or NULL if any error
 */
GLubyte *
st_texture_image_map(struct st_context *st, struct st_texture_image *stImage,
                     GLuint zoffset, enum pipe_transfer_usage usage,
                     GLuint x, GLuint y, GLuint w, GLuint h)
{
   struct pipe_context *pipe = st->pipe;
   struct pipe_resource *pt = stImage->pt;

   DBG("%s \n", __FUNCTION__);

   stImage->transfer = pipe_get_transfer(st->pipe, pt, stImage->level,
                                         stImage->face + zoffset,
                                         usage, x, y, w, h);

   if (stImage->transfer)
      return pipe_transfer_map(pipe, stImage->transfer);
   else
      return NULL;
}


void
st_texture_image_unmap(struct st_context *st,
                       struct st_texture_image *stImage)
{
   struct pipe_context *pipe = st->pipe;

   DBG("%s\n", __FUNCTION__);

   pipe_transfer_unmap(pipe, stImage->transfer);

   pipe->transfer_destroy(pipe, stImage->transfer);
}



/**
 * Upload data to a rectangular sub-region.  Lots of choices how to do this:
 *
 * - memcpy by span to current destination
 * - upload data as new buffer and blit
 *
 * Currently always memcpy.
 */
static void
st_surface_data(struct pipe_context *pipe,
		struct pipe_transfer *dst,
		unsigned dstx, unsigned dsty,
		const void *src, unsigned src_stride,
		unsigned srcx, unsigned srcy, unsigned width, unsigned height)
{
   void *map = pipe_transfer_map(pipe, dst);

   assert(dst->resource);
   util_copy_rect(map,
                  dst->resource->format,
                  dst->stride,
                  dstx, dsty, 
                  width, height, 
                  src, src_stride, 
                  srcx, srcy);

   pipe_transfer_unmap(pipe, dst);
}


/* Upload data for a particular image.
 */
void
st_texture_image_data(struct st_context *st,
                      struct pipe_resource *dst,
                      GLuint face,
                      GLuint level,
                      void *src,
                      GLuint src_row_stride, GLuint src_image_stride)
{
   struct pipe_context *pipe = st->pipe;
   GLuint depth = u_minify(dst->depth0, level);
   GLuint i;
   const GLubyte *srcUB = src;
   struct pipe_transfer *dst_transfer;

   DBG("%s\n", __FUNCTION__);

   for (i = 0; i < depth; i++) {
      dst_transfer = pipe_get_transfer(st->pipe, dst, level, face + i,
                                       PIPE_TRANSFER_WRITE, 0, 0,
                                       u_minify(dst->width0, level),
                                       u_minify(dst->height0, level));

      st_surface_data(pipe, dst_transfer,
		      0, 0,                             /* dstx, dsty */
		      srcUB,
		      src_row_stride,
		      0, 0,                             /* source x, y */
		      u_minify(dst->width0, level),
                      u_minify(dst->height0, level));    /* width, height */

      pipe->transfer_destroy(pipe, dst_transfer);

      srcUB += src_image_stride;
   }
}


/**
 * For debug only: get/print center pixel in the src resource.
 */
static void
print_center_pixel(struct pipe_context *pipe, struct pipe_resource *src)
{
   struct pipe_transfer *xfer;
   struct pipe_box region;
   ubyte *map;

   region.x = src->width0 / 2;
   region.y = src->height0 / 2;
   region.z = 0;
   region.width = 1;
   region.height = 1;
   region.depth = 1;

   xfer = pipe->get_transfer(pipe, src, 0, PIPE_TRANSFER_READ, &region);
   map = pipe->transfer_map(pipe, xfer);

   printf("center pixel: %d %d %d %d\n", map[0], map[1], map[2], map[3]);

   pipe->transfer_unmap(pipe, xfer);
   pipe->transfer_destroy(pipe, xfer);
}


/**
 * Copy the image at level=0 in 'src' to the 'dst' resource at 'dstLevel'.
 * This is used to copy mipmap images from one texture buffer to another.
 * This typically happens when our initial guess at the total texture size
 * is incorrect (see the guess_and_alloc_texture() function).
 */
void
st_texture_image_copy(struct pipe_context *pipe,
                      struct pipe_resource *dst, GLuint dstLevel,
                      struct pipe_resource *src, GLuint srcLevel,
                      GLuint face)
{
   GLuint width = u_minify(dst->width0, dstLevel);
   GLuint height = u_minify(dst->height0, dstLevel);
   GLuint depth = u_minify(dst->depth0, dstLevel);
   struct pipe_box src_box;
   GLuint i;

   assert(u_minify(src->width0, srcLevel) == width);
   assert(u_minify(src->height0, srcLevel) == height);
   assert(u_minify(src->depth0, srcLevel) == depth);

   src_box.x = 0;
   src_box.y = 0;
   src_box.width = width;
   src_box.height = height;
   src_box.depth = 1;
   /* Loop over 3D image slices */
   /* could (and probably should) use "true" 3d box here -
      but drivers can't quite handle it yet */
   for (i = face; i < face + depth; i++) {
      src_box.z = i;

      if (0)  {
         print_center_pixel(pipe, src);
      }

      pipe->resource_copy_region(pipe,
                                 dst,
                                 dstLevel,
                                 0, 0, i,/* destX, Y, Z */
                                 src,
                                 srcLevel,
                                 &src_box);
   }
}

