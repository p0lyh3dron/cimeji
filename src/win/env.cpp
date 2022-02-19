/*
*	env.cpp
*	
*	Authored by Demez on Februrary 13, 2022
*
*
*	Functions for interacting with the desktop environment 
*/
#include "env.h"

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <stdlib.h>

#include <string> 

// win32
#include <direct.h>
#include <fileapi.h> 
#include <shlwapi.h> 
#include <minwinbase.h> 


/*
* Find a Window within a box
* 
* NOT IMPLEMENTED
*/
window_t env_find_window_in_box( u16 sTopLeft, u16 sBottomRight )
{
	return nullptr;
}


/*
* Find a Window intersecting with the avatar window
*/
window_t env_find_window( avatar_t* spAvatar )
{
	if ( spAvatar == nullptr )
		return nullptr;

	return env_find_window_in_box(
		spAvatar->aPos[0] + spAvatar->aWidth,
		spAvatar->aPos[1] + spAvatar->aHeight
	);
}


/*
* Free a handle to a Window
* 
* NOT IMPLEMENTED
*/
void env_free_window( window_t spWindow )
{
	if ( spWindow == nullptr )
		return;
}


/*
* Get a Window's aboslute position and size
* 
* NOT IMPLEMENTED
*/
window_data_t* env_get_window_data( window_t spWindow )
{
	if ( spWindow == nullptr )
		return nullptr;

	return nullptr;
}


/*
* Move a Window
* 
* NOT IMPLEMENTED
*/
void env_move_window( window_t spWindow, u16 sX, u16 sY )
{
	if ( spWindow == nullptr )
		return;
}


/*
* Offset a Window
* 
* NOT IMPLEMENTED
*/
void env_offset_window( window_t spWindow, u16 sX, u16 sY )
{
	if ( spWindow == nullptr )
		return;
}


/*
* Get monitor the avatar is currently on
* 
* NOT IMPLEMENTED
*/
monitor_t* env_get_current_monitor( avatar_t* spAvatar )
{
	if ( spAvatar == nullptr )
		return nullptr;
}


/*
* Get Total Area Size of all monitors
* 
* NOT IMPLEMENTED
*/
display_area_t* env_get_display_area()
{
	return nullptr;
}
