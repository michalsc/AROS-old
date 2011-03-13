/*
 * Copyright © 2006 Red Hat, Inc.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Author: Carl D. Worth <cworth@cworth.org>
 */

#include "cairo-test.h"

#define SIZE 25

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    /* Paint background white, then draw in black. */
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0); /* white */
    cairo_paint (cr);
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0); /* black */

    cairo_set_line_width (cr, 1.0);
    cairo_translate (cr, 1, 1);

    /* Draw everything first with square caps. */
    cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);

    /* Draw horizontal and vertical segments, each in both
     * directions. */
    cairo_move_to     (cr,  4.5,  0.5);
    cairo_rel_line_to (cr,  2.0,  0.0);

    cairo_move_to     (cr, 10.5,  4.5);
    cairo_rel_line_to (cr,  0.0,  2.0);

    cairo_move_to     (cr,  6.5, 10.5);
    cairo_rel_line_to (cr, -2.0,  0.0);

    cairo_move_to     (cr,  0.5,  6.5);
    cairo_rel_line_to (cr,  0.0, -2.0);

    /* Draw right angle turns in four directions. */
    cairo_move_to     (cr,  0.5,  2.5);
    cairo_rel_line_to (cr,  0.0, -2.0);
    cairo_rel_line_to (cr,  2.0,  0.0);

    cairo_move_to     (cr,  8.5,  0.5);
    cairo_rel_line_to (cr,  2.0,  0.0);
    cairo_rel_line_to (cr,  0.0,  2.0);

    cairo_move_to     (cr, 10.5,  8.5);
    cairo_rel_line_to (cr,  0.0,  2.0);
    cairo_rel_line_to (cr, -2.0,  0.0);

    cairo_move_to     (cr,  2.5, 10.5);
    cairo_rel_line_to (cr, -2.0,  0.0);
    cairo_rel_line_to (cr,  0.0, -2.0);

    /* Draw a closed-path rectangle */
    cairo_rectangle   (cr, 0.5, 12.5, 10.0, 10.0);

    cairo_stroke (cr);

    cairo_translate (cr, 12, 0);

    /* Now draw the same results, but with butt caps. */
    cairo_set_line_cap (cr, CAIRO_LINE_CAP_BUTT);

    /* Draw horizontal and vertical segments, each in both
     * directions. */
    cairo_move_to     (cr,  4.0,  0.5);
    cairo_rel_line_to (cr,  3.0,  0.0);

    cairo_move_to     (cr, 10.5,  4.0);
    cairo_rel_line_to (cr,  0.0,  3.0);

    cairo_move_to     (cr,  7.0, 10.5);
    cairo_rel_line_to (cr, -3.0,  0.0);

    cairo_move_to     (cr,  0.5,  7.0);
    cairo_rel_line_to (cr,  0.0, -3.0);

    /* Draw right angle turns in four directions. */
    cairo_move_to     (cr,  0.5,  3.0);
    cairo_rel_line_to (cr,  0.0, -2.5);
    cairo_rel_line_to (cr,  2.5,  0.0);

    cairo_move_to     (cr,  8.0,  0.5);
    cairo_rel_line_to (cr,  2.5,  0.0);
    cairo_rel_line_to (cr,  0.0,  2.5);

    cairo_move_to     (cr, 10.5,  8.0);
    cairo_rel_line_to (cr,  0.0,  2.5);
    cairo_rel_line_to (cr, -2.5,  0.0);

    cairo_move_to     (cr,  3.0, 10.5);
    cairo_rel_line_to (cr, -2.5,  0.0);
    cairo_rel_line_to (cr,  0.0, -2.5);

    /* Draw a closed-path rectangle */
    cairo_rectangle   (cr, 0.5, 12.5, 10.0, 10.0);

    /* Draw a path that is rectilinear initially, but not completely */
    /* We draw this out of the target window.  The bug that caused this
     * addition was leaks if part of the path was rectilinear but not
     * completely */
    cairo_move_to     (cr,  3.0, 30.5);
    cairo_rel_line_to (cr, -2.5,  0.0);
    cairo_rel_line_to (cr, +2.5, +2.5);

    cairo_stroke (cr);

    return CAIRO_TEST_SUCCESS;
}

CAIRO_TEST (rectilinear_stroke,
	    "Test rectilinear stroke operations (covering only whole pixels)",
	    "stroke", /* keywords */
	    NULL, /* requirements */
	    SIZE, SIZE,
	    NULL, draw)

