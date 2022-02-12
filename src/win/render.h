#pragma once

#include "../shimeji.h"


struct shimeji_surface_t
{
};


shimeji_surface_t* shimeji_surface_init();
void shimeji_surface_free( shimeji_surface_t* spSurface );

/*
*  Starts the renderer thread.
*
*  @param  shimeji_surface_t *
*      The surface to render on.
*/
void render_start( shimeji_surface_t* spSurface );

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

