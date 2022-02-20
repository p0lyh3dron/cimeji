/*
 *	env.h
 *	
 *	Authored by Demez on Februrary 13, 2022
 *
 *
 *	Functions for interacting with the desktop environment 
 */
#pragma once

#include "../shimeji.h"

using window_t = void*;


struct window_data_t
{
	u16 aPosX;
	u16 aPosY;

	u16 aSizeX;
	u16 aSizeY;
};

struct monitor_t
{
	int aId;
	int aSizeX;
	int aSizeY;
};


/*
 * Init, Destroy, and Update Desktop Environment Helpers
 */
void env_init();
void env_shutdown();
void env_update();


/*
 * Find a Window within a box
 * 
 * NOT IMPLEMENTED
 */
window_t env_find_window_in_box( u16 sTopLeft, u16 sBottomRight );

/*
 * Find a Window intersecting with the avatar window
 * 
 * NOT IMPLEMENTED DUE TO ABOVE FUNCTION
 */
window_t env_find_window( avatar_t* spAvatar );

/*
* Free a handle to a Window
* 
* NOT IMPLEMENTED
*/
void env_free_window( window_t spWindow );

/*
* Get a Window's aboslute position and size
* 
* NOT IMPLEMENTED
*/
window_data_t* env_get_window_data( window_t spWindow );

/*
* Move a Window
* 
* NOT IMPLEMENTED
*/
void env_move_window( window_t spWindow, u16 sX, u16 sY );

/*
* Offset a Window
* 
* NOT IMPLEMENTED
*/
void env_offset_window( window_t spWindow, u16 sX, u16 sY );

/*
* Get monitor the avatar is currently on
* 
* NOT IMPLEMENTED
*/
monitor_t* env_get_current_monitor( avatar_t* spAvatar );

/*
* Get total monitor count
*/
int env_get_monitor_count();

/*
* Get Total Area Size of all monitors
* 
* NOT IMPLEMENTED
*/
void env_get_display_area( int& width, int& height );

/*
* Get Cursor Position
*/
void env_cursor_pos( int& srX, int& srY );

