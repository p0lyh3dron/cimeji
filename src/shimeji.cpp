/*
 *	shimeji.h
 *	
 *	Authored by p0lyh3dron on Februrary 5, 2022
 *
 *
 *	Defines the types and functions that will be
 *	used for interfacing with shimejis.  
 */
#include "shimeji.h"

#include "util.h"

#include <malloc.h>
#include <string.h>
#include <spng.h>
#include <math.h>
/*
 *  Initializes a shimeji.
 *
 *  @param  const char *
 *      The path to the shimeji.
 *  @return	shimeji_t *
 *      Valid pointer to shimeji_t on success, nullptr on failure.
 */
shimeji_t *shimeji_init( const char *spPath ) {
    shimeji_t *pShimeji = ( shimeji_t * )malloc( sizeof( shimeji_t ) );
    pShimeji->apData    = ( shimeji_data_t ** )malloc( 1 * sizeof( shimeji_data_t * ) );
    if( !pShimeji ) {
        return NULL;
    }
    char **apFiles = scan_dir( spPath );
    int i, j;
    for ( i = 0, j = 0; apFiles[ i ] != NULL; i++ ) {
        if( strstr( apFiles[ i ], ".png" ) ) {
            shimeji_data_t *pData = ( shimeji_data_t * )malloc( sizeof( shimeji_data_t ) );
            if( !pData ) {
                fprintf( stderr, "Failed to allocate memory for shimeji data.\n" );
                return NULL;            
            }
            pData->apBuf     = NULL;

            FILE *pFile = fopen( apFiles[ i ], "rb" );
            if( !pFile ) {
                fprintf( stderr, "Failed to open file %s.\n", apFiles[ i ] );
                return NULL;
            }

            spng_ctx *pPng = spng_ctx_new( 0 );
            if( !pPng ) {
                fprintf( stderr, "Failed to allocate memory for shimeji png context.\n" );
                return NULL;
            }

            struct spng_ihdr ihdr;
            u32              len;

            spng_set_png_file( pPng, pFile );
            spng_get_ihdr( pPng, &ihdr );
            spng_decoded_image_size( pPng, SPNG_FMT_RGBA8, ( size_t* )&len );
#ifdef __unix__
            u8 *pBuf = ( u8 * )malloc( len );
#endif /* __unix__  */
            pData->apBuf = ( u8 * )malloc( len );
            if( !pData->apBuf ) {
                fprintf( stderr, "Failed to allocate memory for shimeji data buffer.\n" );
                return NULL;
            }
#ifdef __unix__
            spng_decode_image( pPng, pBuf, len, SPNG_FMT_RGBA8, 0 );
#else
            spng_decode_image( pPng, pData->apBuf, len, SPNG_FMT_RGBA8, 0 );
#endif /* __unix__  */
            spng_ctx_free( pPng );
#ifdef __unix__
            /* RGBA to BGRA  */
            for ( int i = 0; i < len; i++ ) {
                if ( i % 4 == 0 ) {
                    pData->apBuf[ i + 2 ] = pBuf[ i ];
                }
                else if ( i % 4 == 2 ) {
                    pData->apBuf[ i - 2 ] = pBuf[ i ];
                }
                else {
                    pData->apBuf[ i ]     = pBuf[ i ];
                }
            }

            free( pBuf );
#endif /* __unix__  */

            /* ???  */
            pShimeji->aWidth  = 128;
            pShimeji->aHeight = 128;

            printf( "Loaded shimeji with width %d and height %d\n", pShimeji->aWidth, pShimeji->aHeight );

            pShimeji->apData = ( shimeji_data_t ** )realloc( pShimeji->apData, ( i + 1 ) * sizeof( shimeji_data_t * ) );
            if( !pShimeji->apData ) {
                fprintf( stderr, "Failed to allocate memory for shimeji data.\n" );
                return NULL;
            }
            pShimeji->apData[ j++ ] = pData;
        }
    }
    pShimeji->apData = ( shimeji_data_t ** )realloc( pShimeji->apData, ( i ) * sizeof( shimeji_data_t * ) );
    if( !pShimeji->apData ) {
        fprintf( stderr, "Failed to allocate memory for shimeji data.\n" );
        return NULL;
    }
	
    pShimeji->apData[ i - 1 ] = NULL;

    return pShimeji;
}
/*
 *  Frees a shimeji.
 *
 *  @param  shimeji_t *
 *      The shimeji to free.
 */
void shimeji_free( shimeji_t *spShimeji ) {
    for ( int i = 0; spShimeji->apData[ i ] != NULL; ++i ) {
        free( spShimeji->apData[ i ]->apBuf );
        free( spShimeji->apData[ i ] );
    }
    free( spShimeji->apData );
    free( spShimeji );
}
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
void shimeji_set( shimeji_t *spShimeji, u16 sX, u16 sY, behavior_t sBehavior ) {
    spShimeji->aPos[ 0 ] = sX;
    spShimeji->aPos[ 1 ] = sY;
    spShimeji->aBehavior = sBehavior;
}