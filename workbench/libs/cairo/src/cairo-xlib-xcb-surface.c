/* cairo - a vector graphics library with display and print output
 *
 * Copyright © 2002 University of Southern California
 * Copyright © 2009 Intel Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it either under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * (the "LGPL") or, at your option, under the terms of the Mozilla
 * Public License Version 1.1 (the "MPL"). If you do not alter this
 * notice, a recipient may use your version of this file under either
 * the MPL or the LGPL.
 *
 * You should have received a copy of the LGPL along with this library
 * in the file COPYING-LGPL-2.1; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA 02110-1335, USA
 * You should have received a copy of the MPL along with this library
 * in the file COPYING-MPL-1.1
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY
 * OF ANY KIND, either express or implied. See the LGPL or the MPL for
 * the specific language governing rights and limitations.
 *
 * The Original Code is the cairo graphics library.
 *
 * The Initial Developer of the Original Code is University of Southern
 * California.
 *
 * Contributor(s):
 *	Carl D. Worth <cworth@cworth.org>
 *	Chris Wilson <chris@chris-wilson.co.uk>
 */

#include "cairoint.h"

#include "cairo-xlib.h"
#include "cairo-xcb.h"

#include "cairo-xcb-private.h"
#include "cairo-xlib-xrender-private.h"

#include <X11/Xlib-xcb.h>

static cairo_surface_t *
_cairo_xlib_xcb_surface_create (void *dpy,
				void *scr,
				void *visual,
				void *format,
				cairo_surface_t *xcb);

static cairo_surface_t *
_cairo_xlib_xcb_surface_create_similar (void			*abstract_other,
					cairo_content_t		 content,
					int			 width,
					int			 height)
{
    cairo_xlib_xcb_surface_t *other = abstract_other;
    cairo_surface_t *xcb;

    xcb = other->xcb->base.backend->create_similar (other->xcb, content, width, height);
    if (unlikely (xcb == NULL || xcb->status))
	return xcb;

    return _cairo_xlib_xcb_surface_create (other->display, other->screen, NULL, NULL, xcb);
}

static cairo_status_t
_cairo_xlib_xcb_surface_finish (void *abstract_surface)
{
    cairo_xlib_xcb_surface_t *surface = abstract_surface;
    cairo_status_t status;

    cairo_surface_finish (&surface->xcb->base);
    status = surface->xcb->base.status;
    cairo_surface_destroy (&surface->xcb->base);

    return status;
}

static cairo_status_t
_cairo_xlib_xcb_surface_acquire_source_image (void *abstract_surface,
					      cairo_image_surface_t **image_out,
					      void **image_extra)
{
    cairo_xlib_xcb_surface_t *surface = abstract_surface;
    return _cairo_surface_acquire_source_image (&surface->xcb->base,
						image_out, image_extra);
}

static void
_cairo_xlib_xcb_surface_release_source_image (void *abstract_surface,
					      cairo_image_surface_t *image_out,
					      void *image_extra)
{
    cairo_xlib_xcb_surface_t *surface = abstract_surface;
    _cairo_surface_release_source_image (&surface->xcb->base, image_out, image_extra);
}

static cairo_bool_t
_cairo_xlib_xcb_surface_get_extents (void *abstract_surface,
				     cairo_rectangle_int_t *extents)
{
    cairo_xlib_xcb_surface_t *surface = abstract_surface;
    return _cairo_surface_get_extents (&surface->xcb->base, extents);
}

static void
_cairo_xlib_xcb_surface_get_font_options (void *abstract_surface,
					  cairo_font_options_t *options)
{
    cairo_xlib_xcb_surface_t *surface = abstract_surface;
    surface->xcb->base.backend->get_font_options (surface->xcb, options);
}

static cairo_int_status_t
_cairo_xlib_xcb_surface_paint (void			*abstract_surface,
			       cairo_operator_t		 op,
			       const cairo_pattern_t	*source,
			       cairo_clip_t		*clip)
{
    cairo_xlib_xcb_surface_t *surface = abstract_surface;
    return surface->xcb->base.backend->paint (surface->xcb, op, source, clip);
}

static cairo_int_status_t
_cairo_xlib_xcb_surface_mask (void			*abstract_surface,
			      cairo_operator_t		 op,
			      const cairo_pattern_t	*source,
			      const cairo_pattern_t	*mask,
			      cairo_clip_t		*clip)
{
    cairo_xlib_xcb_surface_t *surface = abstract_surface;
    return surface->xcb->base.backend->mask (surface->xcb, op, source, mask, clip);
}

static cairo_int_status_t
_cairo_xlib_xcb_surface_stroke (void				*abstract_surface,
				cairo_operator_t		 op,
				const cairo_pattern_t		*source,
				cairo_path_fixed_t		*path,
				const cairo_stroke_style_t	*style,
				const cairo_matrix_t		*ctm,
				const cairo_matrix_t		*ctm_inverse,
				double				 tolerance,
				cairo_antialias_t		 antialias,
				cairo_clip_t			*clip)
{
    cairo_xlib_xcb_surface_t *surface = abstract_surface;
    return surface->xcb->base.backend->stroke (surface->xcb,
					       op, source, path, style,
					       ctm, ctm_inverse,
					       tolerance, antialias, clip);
}

static cairo_int_status_t
_cairo_xlib_xcb_surface_fill (void			*abstract_surface,
			      cairo_operator_t		 op,
			      const cairo_pattern_t	*source,
			      cairo_path_fixed_t	*path,
			      cairo_fill_rule_t		 fill_rule,
			      double			 tolerance,
			      cairo_antialias_t		 antialias,
			      cairo_clip_t		*clip)
{
    cairo_xlib_xcb_surface_t *surface = abstract_surface;
    return surface->xcb->base.backend->fill (surface->xcb,
					     op, source, path,
					     fill_rule, tolerance, antialias,
					     clip);
}

static cairo_int_status_t
_cairo_xlib_xcb_surface_glyphs (void			*abstract_surface,
				cairo_operator_t	 op,
				const cairo_pattern_t	*source,
				cairo_glyph_t		*glyphs,
				int			 num_glyphs,
				cairo_scaled_font_t	*scaled_font,
				cairo_clip_t		*clip,
				int *num_remaining)
{
    cairo_xlib_xcb_surface_t *surface = abstract_surface;
    return surface->xcb->base.backend->show_glyphs (surface->xcb, op, source,
						    glyphs, num_glyphs, scaled_font,
						    clip, num_remaining);
}

static cairo_status_t
_cairo_xlib_xcb_surface_flush (void *abstract_surface)
{
    cairo_xlib_xcb_surface_t *surface = abstract_surface;
    return surface->xcb->base.backend->flush (surface->xcb);
}

static cairo_status_t
_cairo_xlib_xcb_surface_mark_dirty (void *abstract_surface,
				    int x, int y,
				    int width, int height)
{
    cairo_xlib_xcb_surface_t *surface = abstract_surface;
    return surface->xcb->base.backend->mark_dirty_rectangle (surface->xcb, x, y, width, height);
}

static const cairo_surface_backend_t _cairo_xlib_xcb_surface_backend = {
    CAIRO_SURFACE_TYPE_XLIB,
    _cairo_xlib_xcb_surface_create_similar,
    _cairo_xlib_xcb_surface_finish,
    _cairo_xlib_xcb_surface_acquire_source_image,
    _cairo_xlib_xcb_surface_release_source_image,
    NULL, NULL, NULL, /* dest acquire/release/clone */

    NULL, /* composite */
    NULL, /* fill */
    NULL, /* trapezoids */
    NULL, /* span */
    NULL, /* check-span */

    NULL, /* copy_page */
    NULL, /* show_page */
    _cairo_xlib_xcb_surface_get_extents,
    NULL, /* old-glyphs */
    _cairo_xlib_xcb_surface_get_font_options,

    _cairo_xlib_xcb_surface_flush,
    _cairo_xlib_xcb_surface_mark_dirty,
    NULL, NULL, /* font/glyph fini */

    _cairo_xlib_xcb_surface_paint,
    _cairo_xlib_xcb_surface_mask,
    _cairo_xlib_xcb_surface_stroke,
    _cairo_xlib_xcb_surface_fill,
    _cairo_xlib_xcb_surface_glyphs,
};

static cairo_surface_t *
_cairo_xlib_xcb_surface_create (void *dpy,
				void *scr,
				void *visual,
				void *format,
				cairo_surface_t *xcb)
{
    cairo_xlib_xcb_surface_t *surface;

    if (unlikely (xcb->status))
	return xcb;

    surface = malloc (sizeof (*surface));
    if (unlikely (surface == NULL)) {
	cairo_surface_destroy (xcb);
	return _cairo_surface_create_in_error (CAIRO_STATUS_NO_MEMORY);
    }

    _cairo_surface_init (&surface->base,
			 &_cairo_xlib_xcb_surface_backend,
			 xcb->device,
			 xcb->content);

    surface->display = dpy;
    surface->screen = scr;
    surface->visual = visual;
    surface->format = format;
    surface->xcb = (cairo_xcb_surface_t *) xcb;

    return &surface->base;
}

static Screen *
_cairo_xlib_screen_from_visual (Display *dpy, Visual *visual)
{
    int s, d, v;

    for (s = 0; s < ScreenCount (dpy); s++) {
	Screen *screen;

	screen = ScreenOfDisplay (dpy, s);
	if (visual == DefaultVisualOfScreen (screen))
	    return screen;

	for (d = 0; d < screen->ndepths; d++) {
	    Depth  *depth;

	    depth = &screen->depths[d];
	    for (v = 0; v < depth->nvisuals; v++)
		if (visual == &depth->visuals[v])
		    return screen;
	}
    }

    return NULL;
}

cairo_surface_t *
cairo_xlib_surface_create (Display     *dpy,
			   Drawable	drawable,
			   Visual      *visual,
			   int		width,
			   int		height)
{
    Screen *scr;
    xcb_visualtype_t xcb_visual;

    scr = _cairo_xlib_screen_from_visual (dpy, visual);
    if (scr == NULL)
	return _cairo_surface_create_in_error (_cairo_error (CAIRO_STATUS_INVALID_VISUAL));

    xcb_visual.visual_id = visual->visualid;
#if defined(__cplusplus) || defined(c_plusplus)
    xcb_visual._class = visual->c_class;
#else
    xcb_visual._class = visual->class;
#endif
    xcb_visual.bits_per_rgb_value = visual->bits_per_rgb;
    xcb_visual.colormap_entries = visual->map_entries;
    xcb_visual.red_mask = visual->red_mask;
    xcb_visual.green_mask = visual->green_mask;
    xcb_visual.blue_mask = visual->blue_mask;

    return _cairo_xlib_xcb_surface_create (dpy, scr, visual, NULL,
					   cairo_xcb_surface_create (XGetXCBConnection (dpy),
								     drawable,
								     &xcb_visual,
								     width, height));
}

cairo_surface_t *
cairo_xlib_surface_create_for_bitmap (Display  *dpy,
				      Pixmap	bitmap,
				      Screen   *scr,
				      int	width,
				      int	height)
{
    return _cairo_xlib_xcb_surface_create (dpy, scr, NULL, NULL,
					   cairo_xcb_surface_create_for_bitmap (XGetXCBConnection (dpy),
										(xcb_screen_t *) scr,
										bitmap,
										width, height));
}

#if CAIRO_HAS_XLIB_XRENDER_SURFACE
static xcb_screen_t *
_cairo_xcb_screen_from_root (xcb_connection_t *connection,
			     xcb_window_t id)
{
    xcb_screen_iterator_t s;

    s = xcb_setup_roots_iterator (xcb_get_setup (connection));
    for (; s.rem; xcb_screen_next (&s)) {
	if (s.data->root == id)
	    return s.data;
    }

    return NULL;
}
cairo_surface_t *
cairo_xlib_surface_create_with_xrender_format (Display		    *dpy,
					       Drawable		    drawable,
					       Screen		    *scr,
					       XRenderPictFormat    *format,
					       int		    width,
					       int		    height)
{
    xcb_render_pictforminfo_t xcb_format;
    xcb_connection_t *connection;
    xcb_screen_t *screen;

    xcb_format.id = format->id;
    xcb_format.type = format->type;
    xcb_format.depth = format->depth;
    xcb_format.direct.red_shift = format->direct.red;
    xcb_format.direct.red_mask = format->direct.redMask;
    xcb_format.direct.green_shift = format->direct.green;
    xcb_format.direct.green_mask = format->direct.greenMask;
    xcb_format.direct.blue_shift = format->direct.blue;
    xcb_format.direct.blue_mask = format->direct.blueMask;
    xcb_format.direct.alpha_shift = format->direct.alpha;
    xcb_format.direct.alpha_mask = format->direct.alphaMask;
    xcb_format.colormap = format->colormap;

    connection = XGetXCBConnection (dpy);
    screen = _cairo_xcb_screen_from_root (connection, (xcb_window_t) scr->root);

    return _cairo_xlib_xcb_surface_create (dpy, scr, NULL, format,
					   cairo_xcb_surface_create_with_xrender_format (connection, screen,
											 drawable,
											 &xcb_format,
											 width, height));
}

XRenderPictFormat *
cairo_xlib_surface_get_xrender_format (cairo_surface_t *surface)
{
    cairo_xlib_xcb_surface_t *xlib_surface = (cairo_xlib_xcb_surface_t *) surface;

    /* Throw an error for a non-xlib surface */
    if (surface->type != CAIRO_SURFACE_TYPE_XLIB) {
	_cairo_error_throw (CAIRO_STATUS_SURFACE_TYPE_MISMATCH);
	return NULL;
    }

    return xlib_surface->format;
}
#endif

void
cairo_xlib_surface_set_size (cairo_surface_t *abstract_surface,
			     int              width,
			     int              height)
{
    cairo_xlib_xcb_surface_t *surface = (cairo_xlib_xcb_surface_t *) abstract_surface;
    cairo_status_t status;

    if (unlikely (abstract_surface->status))
	return;

    if (surface->base.type != CAIRO_SURFACE_TYPE_XLIB) {
	status = _cairo_surface_set_error (abstract_surface,
		                           CAIRO_STATUS_SURFACE_TYPE_MISMATCH);
	return;
    }

    cairo_xcb_surface_set_size (&surface->xcb->base, width, height);
}

void
cairo_xlib_surface_set_drawable (cairo_surface_t   *abstract_surface,
				 Drawable	    drawable,
				 int		    width,
				 int		    height)
{
    cairo_xlib_xcb_surface_t *surface = (cairo_xlib_xcb_surface_t *)abstract_surface;
    cairo_status_t status;

    if (unlikely (abstract_surface->status))
	return;

    if (surface->base.type != CAIRO_SURFACE_TYPE_XLIB) {
	status = _cairo_surface_set_error (abstract_surface,
		                           CAIRO_STATUS_SURFACE_TYPE_MISMATCH);
	return;
    }

    ASSERT_NOT_REACHED;
}

Display *
cairo_xlib_surface_get_display (cairo_surface_t *abstract_surface)
{
    cairo_xlib_xcb_surface_t *surface = (cairo_xlib_xcb_surface_t *) abstract_surface;

    if (surface->base.type != CAIRO_SURFACE_TYPE_XLIB) {
	_cairo_error_throw (CAIRO_STATUS_SURFACE_TYPE_MISMATCH);
	return NULL;
    }

    return surface->display;
}

Drawable
cairo_xlib_surface_get_drawable (cairo_surface_t *abstract_surface)
{
    cairo_xlib_xcb_surface_t *surface = (cairo_xlib_xcb_surface_t *) abstract_surface;

    if (surface->base.type != CAIRO_SURFACE_TYPE_XLIB) {
	_cairo_error_throw (CAIRO_STATUS_SURFACE_TYPE_MISMATCH);
	return 0;
    }

    return surface->xcb->drawable;
}

Screen *
cairo_xlib_surface_get_screen (cairo_surface_t *abstract_surface)
{
    cairo_xlib_xcb_surface_t *surface = (cairo_xlib_xcb_surface_t *) abstract_surface;

    if (surface->base.type != CAIRO_SURFACE_TYPE_XLIB) {
	_cairo_error_throw (CAIRO_STATUS_SURFACE_TYPE_MISMATCH);
	return NULL;
    }

    return surface->screen;
}

Visual *
cairo_xlib_surface_get_visual (cairo_surface_t *abstract_surface)
{
    cairo_xlib_xcb_surface_t *surface = (cairo_xlib_xcb_surface_t *) abstract_surface;

    if (surface->base.type != CAIRO_SURFACE_TYPE_XLIB) {
	_cairo_error_throw (CAIRO_STATUS_SURFACE_TYPE_MISMATCH);
	return NULL;
    }

    return surface->visual;
}

int
cairo_xlib_surface_get_depth (cairo_surface_t *abstract_surface)
{
    cairo_xlib_xcb_surface_t *surface = (cairo_xlib_xcb_surface_t *) abstract_surface;

    if (surface->base.type != CAIRO_SURFACE_TYPE_XLIB) {
	_cairo_error_throw (CAIRO_STATUS_SURFACE_TYPE_MISMATCH);
	return 0;
    }

    return surface->xcb->depth;
}

int
cairo_xlib_surface_get_width (cairo_surface_t *abstract_surface)
{
    cairo_xlib_xcb_surface_t *surface = (cairo_xlib_xcb_surface_t *) abstract_surface;

    if (surface->base.type != CAIRO_SURFACE_TYPE_XLIB) {
	_cairo_error_throw (CAIRO_STATUS_SURFACE_TYPE_MISMATCH);
	return 0;
    }

    return surface->xcb->width;
}

int
cairo_xlib_surface_get_height (cairo_surface_t *abstract_surface)
{
    cairo_xlib_xcb_surface_t *surface = (cairo_xlib_xcb_surface_t *) abstract_surface;

    if (surface->base.type != CAIRO_SURFACE_TYPE_XLIB) {
	_cairo_error_throw (CAIRO_STATUS_SURFACE_TYPE_MISMATCH);
	return 0;
    }

    return surface->xcb->height;
}
