/*
 *	render.h
 *	
 *	Authored by p0lyh3dron on Februrary 5, 2022
 *
 *
 *	Declares the types and functions that will be
 *	used for rendering.  
 */
#pragma once

#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>

#include "surface.h"
/* 
 *  Initializes a cairo context.
 *
 *  @param  shimeji_surface_t *
 *      The surface to initialize the cairo context on.
 *  @return cairo_t *
 *      The initialized cairo context.
 */
cairo_t *shimeji_cairo_init( shimeji_surface_t *spSurface );
/*
 *	Draws a rectangle.
 *
 *	@param		cairo_t *
 *	    The cairo context to draw on.
 *  @param      int
 *      The x coordinate of the rectangle.
 *  @param      int
 *      The y coordinate of the rectangle.
 *  @param      int
 *      The width of the rectangle.
 *  @param      int
 *      The height of the rectangle.
 */
void draw_rectangle( cairo_t *cr, int x, int y, int w, int h );