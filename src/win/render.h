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
*  @param  shimeji_surface_t *
*     The surface to draw the shimeji on.
*  @param  avatar_t *
*     The shimeji to draw.
*/
void render_draw( shimeji_surface_t *spSurface, avatar_t *spShimeji );

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

*/
void render_clear();

