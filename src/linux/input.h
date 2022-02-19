/*
 *	input.h
 *	
 *	Authored by p0lyh3dron on Februrary 5, 2022
 *
 *
 *	Declares the types and functions that will be
 *	used for interacting with shimejis on screen.  
 */
#pragma once

#include "../manager.h"
/*
 *  Returns the type of mouse click. ( Locks execution, be careful )
 *
 *  @return u32
 *    The type of mouse click.
 *    1 << 0 = Left   click
 *    1 << 1 = Right  click
 *    1 << 2 = Middle click
 */
u32 get_mouse_click( void );
/*
 *  Does the input handling on a thread.
 *
 *  @param  void *
 *     The thread's argument, which is unused.
 *  @return void *
 *     The thread's return value, which is unused.
 */
void *input_thread( void *spArgs );
/*
 * Initializes X11 for input.
*/
void init_input( void );
/*
 *  Returns the shimeji at the given coordinates.
 *
 *  @param  int
 *      The x coordinate.
 *  @param  int
 *      The y coordinate.
 *  @return avatar_t *
 *      The shimeji at the given coordinates, or nullptr if there is no shimeji.
 */
avatar_t *get_shimeji( int x, int y );
/*
 * Returns the cursor's position
 *
 *  @param int *
 *     Location to store the x position.
 *  @param int *
 *     Location to store the y position.
 */
void get_mouse_pos( int *x, int *y );
/*
 *  Returns the shimeji at the mouse's current position.
 *
 *  @return avatar_t *
 *     The shimeji at the mouse's current position, or nullptr if there is no shimeji.
 */
avatar_t *get_shimeji_at_mouse( void );
/*
 *  Closes the input handle.
 */
void close_input( void );