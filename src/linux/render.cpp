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

#include <pthread.h>
#include <unistd.h>
#include <math.h>
static avatar_t **gpAvatars;
static XImage     *gpImages   [ SHIMEJI_MAX_COUNT ]  = { 0 };

static Pixmap      gPixmap = 0;
/*
 *  Renders the surface.
 *
 *  @param  void *
 *     The surface to render on.
 *  @return void *
 *     The thread's return value, which is unused.
 */
void *render_thread( void *spSurface ) {
    shimeji_surface_t *pSurface = ( shimeji_surface_t * )spSurface;
    cairo_t *pCairo = shimeji_cairo_init( pSurface );
    while( 1 ) {
        shimeji_surface_clear( pSurface );
        for ( int i = 0; i < SHIMEJI_MAX_COUNT; ++i ) {
            if ( gpAvatars[ i ] ) {
                render_draw( pSurface, gpAvatars[ i ] );
            }
        }
    }
    return NULL;
}
/*
 *  Starts the renderer thread.
 *
 *  @param  shimeji_surface_t *
 *      The surface to render on.
 */
void render_start( shimeji_surface_t *spSurface ) {
    gpAvatars = get_avatars();
    allow_input_passthrough( spSurface );
    gPixmap = create_pixmap( spSurface );

    pthread_t p;
    pthread_create( &p, NULL, render_thread, spSurface );
}
/*
 *  Creates an XImage for the surface.
 *
 *  @param  shimeji_surface_t *
 *     The surface to create an XImage for.
 *  @param  avatar_t *
 *     The shimeji to create an XImage for.
 *  @return XImage *
 *     A valid XImage for the shimeji on success, nullptr on failure.
 */
XImage *create_ximage( shimeji_surface_t *spSurface, avatar_t *spShimeji ) {
    return XCreateImage( spSurface->apDisplay,
                         spSurface->aVInfo.visual,
                         spSurface->aVInfo.depth,
                         ZPixmap, 0, ( char* )spShimeji->apData[ 0 ]->apBuf,
                         spShimeji->apData[ 0 ]->aWidth, spShimeji->apData[ 0 ]->aHeight, 32, 0 );
}
/*
 *  Creates a Pixmap for the surface.
 *
 *  @param  shimeji_surface_t *
 *    The surface to create a Pixmap for.
 *  @return Pixmap
 *   A valid Pixmap for the surface on success, 0 on failure.
 */
Pixmap create_pixmap( shimeji_surface_t *spSurface ) {
    return XCreatePixmap( spSurface->apDisplay, spSurface->aOverlayWin, spSurface->aWidth, spSurface->aHeight, spSurface->aVInfo.depth );
}
/*
 *  Adds a shimeji to the render queue.
 *
 *  @param  shimeji_surface_t *
 *     The surface to add to the render queue.
 *  @param  avatar_t *
 *     The shimeji to add to the render queue.
 */
void render_add( shimeji_surface_t *spSurface, avatar_t *spShimeji ) {
    int i = 0;
    while( gpAvatars[ i ] != 0 ) {
        ++i;
    }
    gpAvatars[ i ] = spShimeji;
    gpImages[ i ]   = create_ximage( spSurface, gpAvatars[ i ] );
}
/*
 *  Removes a shimeji from the render queue.
 *
 *  @param  avatar_t *
 *     The shimeji to remove from the render queue.
 */
void render_remove( avatar_t *spShimeji ) {
    int i = 0;
    while( gpAvatars[ i ] != spShimeji ) {
        ++i;
    }
    shimeji_free( gpAvatars[ i ] );    
    gpAvatars[ i ] = 0;
    XDestroyImage( gpImages[ i ] );
}
/*
 *  Draws a shimeji.
 *
 *  @param  shimeji_surface_t *
 *     The surface to draw the shimeji on.
 *  @param  avatar_t *
 *     The shimeji to draw.
 */
void render_draw( shimeji_surface_t *spSurface, avatar_t *spShimeji ) {
    int i = 0;
    while( gpAvatars[ i ] != spShimeji ) {
        ++i;
    }
    XPutImage( 
        spSurface->apDisplay, gPixmap, spSurface->aGC, gpImages[ i ], 0, 0, 
        gpAvatars[ i ]->aPos[ 0 ], gpAvatars[ i ]->aPos[ 1 ], 
        gpAvatars[ i ]->apData[ 0 ]->aWidth, gpAvatars[ i ]->apData[ 0 ]->aHeight 
    );
    XCopyArea( 
        spSurface->apDisplay, gPixmap, spSurface->aOverlayWin,  spSurface->aGC, 0, 0, 
        spSurface->aWidth, spSurface->aHeight, 0, 0 
    );
}





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