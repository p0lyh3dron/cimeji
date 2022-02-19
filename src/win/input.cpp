/*
 *  Returns all the files in a directory.
 *
 *  @param  const char *
 *      The path to the directory.
 *  @return char **
 *       A null-terminated array of all the files in the directory.
 */
#include "input.h"

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

u32 get_mouse_click( void )
{
	return 0;
}


/*
*  Does the input handling on a thread.
*
*  @param  void *
*     The thread's argument, which is unused.
*  @return void *
*     The thread's return value, which is unused.
*/
void *input_thread( void *spArgs )
{
	return 0;
}


/*
* Initializes X11 for input.
*/
void init_input( void )
{
}


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
avatar_t *get_shimeji( int x, int y )
{
	return 0;
}


/*
* Returns the cursor's position
*
*  @param int *
*     Location to store the x position.
*  @param int *
*     Location to store the y position.
*/
void get_mouse_pos( int *x, int *y )
{
	POINT cursor;
	GetCursorPos( &cursor );

	if ( x ) *x = cursor.x;
	if ( y ) *y = cursor.y;
}


/*
*  Returns the shimeji at the mouse's current position.
*
*  @return avatar_t *
*     The shimeji at the mouse's current position, or nullptr if there is no shimeji.
*/
avatar_t *get_shimeji_at_mouse( void )
{
	return 0;
}


/*
*  Closes the input handle.
*/
void close_input( void )
{
}


