/*
 *	render.cpp
 *	
 *	Authored by p0lyh3dron on Februrary 5, 2022
 *
 *
 *	Defines the types and functions that will be
 *	used for rendering.  
 */
#include "render.h"
/* 
 *  Initializes a cairo context.
 *
 *  @param  shimeji_surface_t *
 *      The surface to initialize the cairo context on.
 *  @return cairo_t *
 *      The initialized cairo context.
 */
cairo_t *shimeji_cairo_init( shimeji_surface_t *spSurface ) {
    cairo_surface_t* surf = cairo_xlib_surface_create( 
			spSurface->apDisplay,     spSurface->aOverlayWin, 
			spSurface->aVInfo.visual, spSurface->aWidth, 
			spSurface->aHeight 
		);
	cairo_t* cr = cairo_create( surf );

    return cr;
}
/*
 *	Draws a rectangle.
 *
 *	@param		cairo_t *
 *	    The cairo context to draw on.
 */
void draw_rectangle( cairo_t *cr, int x, int y, int w, int h ) {
	cairo_set_source_rgba( cr, 1.0, 0.0, 0.0, 0.5 );
	cairo_rectangle( cr, x, y, 128, 128 );
	cairo_fill( cr );
}