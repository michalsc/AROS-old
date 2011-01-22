/*
 * Mesa 3-D graphics library
 * Version:  7.9
 *
 * Copyright (C) 2010 LunarG Inc.
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Chia-I Wu <olv@lunarg.com>
 */

#ifndef _NATIVE_BUFFER_H_
#define _NATIVE_BUFFER_H_

#include "pipe/p_compiler.h"

struct native_display;
struct pipe_resource;

/**
 * Buffer interface of the native display.  It allows native buffers to be
 * imported and exported.
 *
 * Just like a native window or a native pixmap, a native buffer is another
 * native type.  Its definition depends on the native display.
 *
 * For DRM platform, the type of a native buffer is struct winsys_handle.
 */
struct native_display_buffer {
   struct pipe_resource *(*import_buffer)(struct native_display *ndpy,
                                          const struct pipe_resource *templ,
                                          void *buf);

   /**
    * The resource must be creatred with PIPE_BIND_SHARED.
    */
   boolean (*export_buffer)(struct native_display *ndpy,
                            struct pipe_resource *res,
                            void *buf);
};

#endif /* _NATIVE_BUFFER_H_ */
