/*
 *	input.cpp
 *	
 *	Authored by p0lyh3dron on Februrary 5, 2022
 *
 *
 *	Defines the types and functions that will be
 *	used for interacting with shimejis on screen.  
 */
#include "input.h"

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>

#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>
/* Left mouse button.  */
#define I_LMB ( 1 << 0 )
/* Right mouse button.  */
#define I_RMB ( 1 << 1 )
/* Middle mouse button.  */
#define I_MMB ( 1 << 2 )

Display   *gpDisplay;
u32        gInputFlags;
avatar_t *gpGrabbedShimeji;

int gMouseX;
int gMouseY;

static const char *gpInputHandleName = "/dev/input/mice";
static int         gInputHandle;
/*
 *  Returns the type of mouse click. ( Locks execution, be careful )
 *
 *  @return u32
 *    The type of mouse click.
 *    1 << 0 = Left   click
 *    1 << 1 = Right  click
 *    1 << 2 = Middle click
 */
u32 get_mouse_click( void ) {
    u8 data[ 3 ];
    if ( read( gInputHandle, data, sizeof( data ) ) ) {
        if( data[ 0 ] & 0x1 ) {
            gInputFlags |= I_LMB;
        }
        else {
            gInputFlags &= ~I_LMB;
        }
        if( data[ 0 ] & 0x2 ) {
            gInputFlags |= I_RMB;
        }
        else {
            gInputFlags &= ~I_RMB;
        }
        if( data[ 0 ] & 0x4 ) {
            gInputFlags |= I_MMB;
        }
        else {
            gInputFlags &= ~I_MMB;
        }
        return gInputFlags;
    }
}
/*
 *  Does the input handling on a thread.
 *
 *  @param  void *
 *     The thread's argument, which is unused.
 *  @return void *
 *     The thread's return value, which is unused.
 */
void *input_thread( void *spArgs ) {
    while( 1 ) {
        get_mouse_click();
        if ( gInputFlags & I_LMB ) {
            avatar_t *pShimeji = get_shimeji_at_mouse();
            if ( pShimeji ) {
                gpGrabbedShimeji = pShimeji;
            }
        }
        else {
            gpGrabbedShimeji = nullptr;
        }
        if ( gpGrabbedShimeji ) {
            get_mouse_pos( &gMouseX, &gMouseY );
            shimeji_set( gpGrabbedShimeji, gMouseX, gMouseY, StandUp );
        }
    }
}
/*
 * Initializes X11 for input.
*/
void init_input( void ) {
    gpDisplay = XOpenDisplay( NULL );
    if( gpDisplay == NULL ) {
        fprintf( stderr, "Failed to open display.\n" );
        return;
    }

    gInputHandle = open( gpInputHandleName, O_RDONLY );
    if( gInputHandle == -1 ) {
        fprintf( stderr, "Failed to open input handle.\n" );
        return;
    }

    pthread_t p;
    pthread_create( &p, NULL, input_thread, NULL );
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
avatar_t *get_shimeji( int x, int y ) {
    for ( int i = 0; i < SHIMEJI_MAX_COUNT; ++i ) {
        if ( get_avatars()[ i ] ) {
            if ( x >= get_avatars()[ i ]->aPos[ 0 ] && x <= get_avatars()[ i ]->aPos[ 0 ] + get_avatars()[ i ]->aWidth &&
                 y >= get_avatars()[ i ]->aPos[ 1 ] && y <= get_avatars()[ i ]->aPos[ 1 ] + get_avatars()[ i ]->aHeight ) {
                return get_avatars()[ i ];
            }
        }
    }
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
void get_mouse_pos( int *x, int *y ) {
    Window root, child;
    int root_x, root_y;
    unsigned int mask;
    XQueryPointer( gpDisplay, XDefaultRootWindow( gpDisplay ), &root, &child, &root_x, &root_y, x, y, &mask );
}
/*
 *  Returns the shimeji at the mouse's current position.
 *
 *  @return avatar_t *
 *     The shimeji at the mouse's current position, or nullptr if there is no shimeji.
 */
avatar_t *get_shimeji_at_mouse( void ) {
    int x, y;
    get_mouse_pos( &x, &y );
    return get_shimeji( x, y );
}
/*
 *  Closes the input handle.
 */
void close_input( void ) {
    close( gInputHandle );
}