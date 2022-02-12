/*
 *	surface.cpp
 *	
 *	Authored by p0lyh3dron on Februrary 5, 2022
 *
 *
 *	Defines the types and functions that will be
 *	used for surface presentation.  
 */
#include "surface.h"

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
/*
 *	Initializes the surface.
 *
 *	@return		shimeji_surface_t *
 *	    valid pointer to shimeji_surface_t on success, nullptr on failure.
 */
shimeji_surface_t *shimeji_surface_init() {
    shimeji_surface_t *pSurface = ( shimeji_surface_t * )malloc( sizeof( shimeji_surface_t ) );
    if ( !pSurface )
        return 0;

    pSurface->aWidth    = 1920;
    pSurface->aHeight   = 1080;
    pSurface->apDisplay = XOpenDisplay( 0 );
    if ( !pSurface->apDisplay ) {
        free( pSurface );
        return 0;
    }
    pSurface->aRootWin                         = DefaultRootWindow( pSurface->apDisplay );
    pSurface->aWinAttributes.override_redirect = 1;

    if ( !XMatchVisualInfo( pSurface->apDisplay, DefaultScreen( pSurface->apDisplay ), 32, TrueColor, &pSurface->aVInfo ) ) {
        fprintf( stderr, "Could not find a 32-bit TrueColor visual.\n" );
        XCloseDisplay( pSurface->apDisplay );
        free( pSurface );
        return 0;
    }

    pSurface->aWinAttributes.colormap          = XCreateColormap( pSurface->apDisplay, pSurface->aRootWin, pSurface->aVInfo.visual, AllocNone );
    pSurface->aWinAttributes.background_pixel  = 0;
    pSurface->aWinAttributes.border_pixel      = 0;
    pSurface->aOverlayWin                      = XCreateWindow(
        pSurface->apDisplay, pSurface->aRootWin,
        0, 0, pSurface->aWidth, pSurface->aHeight, 0,
        pSurface->aVInfo.depth, InputOutput, 
        pSurface->aVInfo.visual,
        CWOverrideRedirect | CWColormap | CWBackPixel | CWBorderPixel,
        &pSurface->aWinAttributes
    );
    XMapWindow( pSurface->apDisplay, pSurface->aOverlayWin );

    pSurface->aGC = XCreateGC( pSurface->apDisplay, pSurface->aOverlayWin, 0, 0 );

    return pSurface;
}
/* 
 *	Frees the surface.
 *
 *	@param		shimeji_surface_t *
 *	    pointer to the surface to free.
 */
void shimeji_surface_free( shimeji_surface_t *spSurface ) {
    XDestroyWindow( spSurface->apDisplay, spSurface->aOverlayWin );
    XCloseDisplay( spSurface->apDisplay );
    free( spSurface );
}
/*
 *  Clears the surface.
 * 
 *  @param  shimeji_surface_t *
 *    The surface to clear.
 */
void shimeji_surface_clear( shimeji_surface_t *spSurface ) {
    XSync( spSurface->apDisplay, 1 );
    XFlush( spSurface->apDisplay );
    XClearWindow( spSurface->apDisplay, spSurface->aOverlayWin );
    usleep( 50000 );
}
/*
 *  Allows input passthrough for the window.
 * 
 * @param  shimeji_surface_t *
 *     The surface to allow input passthrough for.
 */
void allow_input_passthrough( shimeji_surface_t *spSurface ) {
    XserverRegion region = XFixesCreateRegion ( spSurface->apDisplay, NULL, 0 );

	XFixesSetWindowShapeRegion( spSurface->apDisplay, spSurface->aOverlayWin, ShapeBounding, 0, 0, 0 );
	XFixesSetWindowShapeRegion( spSurface->apDisplay, spSurface->aOverlayWin, ShapeInput, 0, 0, region );

	XFixesDestroyRegion( spSurface->apDisplay, region );
}