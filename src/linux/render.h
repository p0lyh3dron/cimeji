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
#include "../manager.h"
/*
 *  Renders the surface.
 *
 *  @param  void *
 *     The surface to render on.
 *  @return void *
 *     The thread's return value, which is unused.
 */
void *render_thread( void *spSurface );
/*
 *  Starts the renderer thread.
 *
 *  @param  shimeji_surface_t *
 *      The surface to render on.
 */
void render_start( shimeji_surface_t *spSurface );
/*
 *  Creates an XImage for the surface.
 *
 *  @param  shimeji_surface_t *
 *     The surface to create an XImage for.
 *  @param  shimeji_t *
 *     The shimeji to create an XImage for.
 *  @return XImage *
 *     A valid XImage for the shimeji on success, nullptr on failure.
 */
XImage *create_ximage( shimeji_surface_t *spSurface, shimeji_t *spShimeji );
/*
 *  Creates a Pixmap for the surface.
 *
 *  @param  shimeji_surface_t *
 *    The surface to create a Pixmap for.
 *  @return Pixmap
 *   A valid Pixmap for the surface on success, 0 on failure.
 */
Pixmap create_pixmap( shimeji_surface_t *spSurface );
/*
 *  Adds a shimeji to the render queue.
 *
 *  @param  shimeji_surface_t *
 *     The surface to add to the render queue.
 *  @param  shimeji_t *
 *     The shimeji to add to the render queue.
 */
void render_add( shimeji_surface_t *spSurface, shimeji_t *spShimeji );
/*
 *  Removes a shimeji from the render queue.
 *
 *  @param  shimeji_t *
 *     The shimeji to remove from the render queue.
 */
void render_remove( shimeji_t *spShimeji );
/*
 *  Draws a shimeji.
 *
 *  @param  shimeji_surface_t *
 *     The surface to draw the shimeji on.
 *  @param  shimeji_t *
 *     The shimeji to draw.
 */
void render_draw( shimeji_surface_t *spSurface, shimeji_t *spShimeji );




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