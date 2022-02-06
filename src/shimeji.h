/*
 *	shimeji.h
 *	
 *	Authored by p0lyh3dron on Februrary 5, 2022
 *
 *
 *	Declares the types and functions that will be
 *	used for interfacing with shimejis.  
 */
#pragma once

typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;

typedef enum {
    StandUp = 0,
}behavior_t;

typedef struct {
    u32 aDataSize;
    u8 *apBuf;
    u16 aWidth;
    u16 aHeight;
    u32 aFormat;
}shimeji_data_t;

typedef struct {
    shimeji_data_t **apData;
    u16              aPos[ 2 ];
    behavior_t       aBehavior;
    u16              aWidth;
    u16              aHeight;
}shimeji_t;
/*
 *  Initializes a shimeji.
 *
 *  @param  const char *
 *      The path to the shimeji.
 *  @return	shimeji_t *
 *      Valid pointer to shimeji_t on success, nullptr on failure.
 */
shimeji_t *shimeji_init( const char *spPath );
/*
 *  Frees a shimeji.
 *
 *  @param  shimeji_t *
 *      The shimeji to free.
 */
void shimeji_free( shimeji_t *spShimeji );
/*
 *  Updates the shimeji's values.
 *
 *  @param  shimeji_t *
 *      The shimeji to update.
 *  @param  u16
 *      The new x position.
 *  @param  u16
 *      The new y position.
 *  @param  behavior_t
 *      The new behavior.
 */
void shimeji_set( shimeji_t *spShimeji, u16 sX, u16 sY, behavior_t sBehavior );