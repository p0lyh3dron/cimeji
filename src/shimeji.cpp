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
#include "manager.h"

#include "util.h"
#include "render.h"

#include <malloc.h>
#include <string.h>
#include <spng.h>
#include <unordered_map>

#include <QImage>


bool avatar_load_data( const char *spPath, std::vector< shimeji_data_t* >& srData )
{
    char **apFiles = scan_dir( spPath );

    int i, j;
    for ( i = 0, j = 0; apFiles[ i ] != NULL; i++ )
    {
        if( strstr( apFiles[ i ], ".png" ) )
        {
            shimeji_data_t *pData = new shimeji_data_t;
            pData->apBuf     = NULL;

            FILE *pFile = fopen( apFiles[ i ], "rb" );
            if( !pFile ) {
                fprintf( stderr, "Failed to open file %s.\n", apFiles[ i ] );
                return false;
            }

            spng_ctx *pPng = spng_ctx_new( 0 );
            if( !pPng ) {
                fprintf( stderr, "Failed to allocate memory for shimeji png context.\n" );
                return false;
            }

            struct spng_ihdr ihdr;

            spng_set_png_file( pPng, pFile );
            spng_get_ihdr( pPng, &ihdr );
			
            spng_decoded_image_size( pPng, SPNG_FMT_RGBA8, ( size_t* )&pData->aDataSize );

#ifdef __unix__
            //u8 *pBuf = ( u8 * )malloc( pData->aDataSize );
#endif /* __unix__  */

            pData->apBuf = ( u8 * )malloc( pData->aDataSize );
            if( !pData->apBuf )
            {
                fprintf( stderr, "Failed to allocate memory for shimeji data buffer.\n" );
                return false;
            }

            spng_decode_image( pPng, pData->apBuf, pData->aDataSize, SPNG_FMT_RGBA8, 0 );

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
#endif /* __unix__  */

            pData->aPath   = apFiles[ i ];
            pData->aWidth  = ihdr.width;
            pData->aHeight = ihdr.height;
            pData->aFormat = SPNG_FMT_RGBA8;

            srData.push_back( pData ); 
        }
    }

    return true;
}

/*
 *  Initializes a shimeji.
 *
 *  @param  const char *
 *      The path to the shimeji.
 *  @return	avatar_t *
 *      Valid pointer to avatar_t on success, nullptr on failure.
 */
avatar_t *avatar_create( const char *spPath )
{
    avatar_t *pShimeji = new avatar_t;
    if( !pShimeji )
    {
        return NULL;
    }

    // check if we loaded this directory already
    static std::unordered_map< std::string, std::vector< shimeji_data_t* > > aCache;

    auto it = aCache.find( spPath );
    if ( it != aCache.end() )
    {
        pShimeji->aData = it->second;
    }
    else
    {
        if ( !avatar_load_data( spPath, pShimeji->aData ) )
        {
            delete pShimeji;
            return nullptr;
        }

        aCache[spPath] = pShimeji->aData;
    }

    /* Dumb.  */
    pShimeji->aWidth  = pShimeji->aData[ 0 ]->aWidth;
    pShimeji->aHeight = pShimeji->aData[ 0 ]->aHeight;
    pShimeji->aPath   = spPath;

    gAvatars.push_back( pShimeji );

    return pShimeji;
}


/*
 *  Frees a shimeji.
 *
 *  @param  avatar_t *
 *      The shimeji to free.
 */
void shimeji_free( avatar_t *spShimeji ) {
    for ( int i = 0; i < spShimeji->aData.size(); ++i )
    {
        free( spShimeji->aData[ i ]->apBuf );
        free( spShimeji->aData[ i ] );
    }

    // vec_remove( gAvatars, spShimeji );

    delete spShimeji;
}


void avatar_set_image( avatar_t* spAvatar, u16 sFrame )
{
    if ( !spAvatar )
        return;

    if ( sFrame > spAvatar->aData.size() )
    {
        printf( "out of bounds image: %u\n", sFrame );
        return;
    }

    spAvatar->aFrame = sFrame;
    render_set_image( spAvatar, sFrame );
}


void avatar_set_image( avatar_t* spAvatar, const std::string& srPath )
{
    if ( !spAvatar )
        return;

    u16 i = 0;

    for ( ;i < spAvatar->aData.size(); i++ )
    {
        if ( spAvatar->aData[i]->aPath == spAvatar->aPath + PATH_SEP + srPath )
            break;
    }

    if ( i == spAvatar->aData.size() )
    {
        printf( "non-existent image: %s\n", srPath.c_str() );
        return;
    }

    spAvatar->aFrame = i;
    render_set_image( spAvatar, i );
}


void avatar_set_pos( avatar_t* spAvatar, u16 sX, u16 sY )
{
    if ( !spAvatar )
        return;

    spAvatar->aPos[ 0 ] = sX;
    spAvatar->aPos[ 1 ] = sY;
}


void avatar_set_ang( avatar_t* spAvatar, u16 sAng )
{

}


void avatar_set_scale( avatar_t* spAvatar, char sScaleX, char sScaleY )
{

}

