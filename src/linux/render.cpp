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

#include <qdatastream.h>

#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
static XImage           **gpImages   [ SHIMEJI_MAX_COUNT ]  = { 0 };

static Pixmap             gPixmap = 0;
static shimeji_surface_t *gpSurface = 0;
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
        for ( int i = 0; i < gAvatars.size(); ++i ) {
            if ( gAvatars[ i ] ) {
                render_draw( gAvatars[ i ] );
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
    gpSurface = spSurface;
    allow_input_passthrough( spSurface );
    gPixmap = create_pixmap();

    pthread_t p;
    pthread_create( &p, NULL, render_thread, spSurface );
}
/*
 *  Clears the render resources.
*/
void render_clear( void ) {
    for ( int i = 0; i < SHIMEJI_MAX_COUNT; ++i ) {
        render_remove( gAvatars[ i ] );
    }
    if ( gPixmap ) {
        //XFreePixmap( gpDisplay, gPixmap );
        gPixmap = 0;
    }
}
/*
 *  Creates an XImage for the surface.
 *
 *  @param  avatar_t *
 *     The shimeji to create an XImage for.
 *  @param  int
 *     The index of the shimeji frame.
 *  @return XImage *
 *     A valid XImage for the shimeji on success, nullptr on failure.
 */
XImage *create_ximage( avatar_t *spShimeji, int i ) {
    return XCreateImage( gpSurface->apDisplay,
                         gpSurface->aVInfo.visual,
                         gpSurface->aVInfo.depth,
                         ZPixmap, 0, ( char* )spShimeji->aData[ i ]->apBuf,
                         spShimeji->aWidth, spShimeji->aHeight, 32, 0 );
}
/*
 *  Creates a Pixmap for the surface.
 *
 *  @return Pixmap
 *   A valid Pixmap for the surface on success, 0 on failure.
 */
Pixmap create_pixmap( void ) {
    return XCreatePixmap( gpSurface->apDisplay, gpSurface->aOverlayWin, gpSurface->aWidth, gpSurface->aHeight, gpSurface->aVInfo.depth );
}
/*
 *  Adds a shimeji to the render queue.
 *
 *  @param  avatar_t *
 *     The shimeji to add to the render queue.
 */
void render_add( avatar_t *spShimeji ) {
#if 0
    int i = 0, j = 0;
    while( gAvatars[ i ] != 0 ) {
        ++i;
    }
    gAvatars[ i ] = spShimeji;
#endif
    int i = gAvatars.size() - 1, j = 0;
    for ( ; spShimeji->aData[ j ] != 0; ++j );
    gpImages[ i ]   = ( XImage** )malloc( sizeof( XImage* ) * ( j + 1 ) );
    for ( j = 0; spShimeji->aData[ j ] != 0; ++j ) {
        gpImages[ i ][ j ] = create_ximage( gAvatars[ i ], j );
    }
    gpImages[ i ][ j + 1 ] = 0;
}
/*
 *  Removes a shimeji from the render queue.
 *
 *  @param  avatar_t *
 *     The shimeji to remove from the render queue.
 */
void render_remove( avatar_t *spShimeji ) {
    int i = 0;
    while( gAvatars[ i ] != spShimeji ) {
        ++i;
    }
    shimeji_free( gAvatars[ i ] );    
    gAvatars[ i ] = 0;
    for ( int j = 0; gpImages[ i ][ j ] != 0; ++j ) {
        XDestroyImage( gpImages[ i ][ j ] );
    }
    free( gpImages[ i ] );
}
/*
 *  Draws a shimeji.
 *
 *  @param  avatar_t *
 *     The shimeji to draw.
 */
void render_draw( avatar_t *spShimeji ) {
    int i = 0;
    while( gAvatars[ i ] != spShimeji ) {
        ++i;
    }
    if ( !gpImages[ i ] ) {
        fprintf( stderr, "render_draw( avatar_t* ): gpImages[ %d ] is null\n", i );
        return;
    }
    XPutImage( 
        gpSurface->apDisplay, gPixmap, gpSurface->aGC, gpImages[ i ][ gAvatars[ i ]->aFrame ], 0, 0, 
        gAvatars[ i ]->aPos[ 0 ], gAvatars[ i ]->aPos[ 1 ], 
        gAvatars[ i ]->aWidth, gAvatars[ i ]->aHeight 
    );
    XCopyArea( 
        gpSurface->apDisplay, gPixmap, gpSurface->aOverlayWin, gpSurface->aGC, 0, 0, 
        gpSurface->aWidth, gpSurface->aHeight, 0, 0 
    );
    XFillRectangle( 
        gpSurface->apDisplay, gPixmap, gpSurface->aGC, 
        0, 0, 
        gpSurface->aWidth, gpSurface->aHeight 
    );
}
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
void render_set_image( avatar_t *spAvatar, u16 sIndex, bool sFlip ) {
    spAvatar->aFrame = sIndex;
    if ( sFlip ) {
        /* Die.  */
    }
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