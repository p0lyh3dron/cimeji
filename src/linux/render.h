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

#include <qdatastream.h>

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
 *  Clears the render resources.
*/
void render_clear( void );
/*
 *  Creates an XImage for the surface.
 *
 *  @param  avatar_t *
 *     The shimeji to create an XImage for.
 *  @param  int
 *     The index of the shimeji frame.
 * 
 *  @return XImage *
 *     A valid XImage for the shimeji on success, nullptr on failure.
 */
XImage *create_ximage( avatar_t *spShimeji, int i );
/*
 *  Creates a Pixmap for the surface.
 *
 *  @return Pixmap
 *   A valid Pixmap for the surface on success, 0 on failure.
 */
Pixmap create_pixmap( void );
/*
 *  Adds a shimeji to the render queue.
 *
 *  @param  avatar_t *
 *     The shimeji to add to the render queue.
 */
void render_add( avatar_t *spShimeji );
/*
 *  Removes a shimeji from the render queue.
 *
 *  @param  avatar_t *
 *     The shimeji to remove from the render queue.
 */
void render_remove( avatar_t *spShimeji );
/*
 *  Draws a shimeji.
 *
 *  @param  avatar_t *
 *     The shimeji to draw.
 */
void render_draw( avatar_t *spShimeji );
/*
 *  Sets the image index on a avatar.
 *
 *  @param  avatar_t *
 *     The pointer to the avatar
 *  @param  u16
 *     The image index to draw.
 *  @param  bool
 *     Should we flip the image? (may be removed later for scaling instead, idk)
 */
void render_set_image( avatar_t *spAvatar, u16 sIndex, bool sFlip = false );




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