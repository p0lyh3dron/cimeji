/*
 *	surface.h
 *	
 *	Authored by p0lyh3dron on Februrary 5, 2022
 *
 *
 *	Declares the types and functions that will be
 *	used for surface presentation.  
 */
#pragma once

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>

#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>

typedef unsigned int u32;

typedef struct {
    u32                  aWidth;
    u32                  aHeight;

	Display             *apDisplay;
	Window               aRootWin;
    int                  aDefaultScreen;
    XSetWindowAttributes aWinAttributes;
    XVisualInfo          aVInfo;
    Window               aOverlayWin;
}shimeji_surface_t;
/*
 *	Initializes the surface.
 *
 *	@return		shimeji_surface_t *
 *	    valid pointer to shimeji_surface_t on success, nullptr on failure.
 */
shimeji_surface_t *shimeji_surface_init();
/* 
 *	Frees the surface.
 *
 *	@param		shimeji_surface_t *
 *	    pointer to the surface to free.
 */
void shimeji_surface_free( shimeji_surface_t *spSurface );
/*
 *  Clears the surface.
 * 
 *  @param  shimeji_surface_t *
 *    The surface to clear.
 */
void shimeji_surface_clear( shimeji_surface_t *spSurface );
/*
 *  Allows input passthrough for the window.
 * 
 * @param  Window
 *     The window to allow input passthrough for.
 */
void allow_input_passthrough( shimeji_surface_t *spSurface );