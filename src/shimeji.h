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

#include <string>
#include <vector>

typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;

#ifdef _WIN32
constexpr char PATH_SEP = '\\';
#else
constexpr char PATH_SEP = '/';
#endif

struct shimeji_data_t
{
    u32 aDataSize;
    u8 *apBuf;
    u16 aWidth;
    u16 aHeight;
    u32 aFormat;
    std::string aPath;
};

struct behavior_t;

struct avatar_t
{
    std::vector< shimeji_data_t* > aData;
    std::string      aPath;  // path to data folder
    int              aPos[ 2 ] = {0, 0};
    behavior_t*      apBehavior = nullptr;
    u16              aWidth = 0;
    u16              aHeight = 0;
    u16              aFrame = 0;
    bool             aGrabbed = false;
};

/*
 *  Initializes a shimeji.
 *
 *  @param  const char *
 *      The path to the shimeji.
 *  @return	avatar_t *
 *      Valid pointer to avatar_t on success, nullptr on failure.
 */
avatar_t *avatar_create( const char *spPath );
/*
 *  Frees a shimeji.
 *
 *  @param  avatar_t *
 *      The shimeji to free.
 */
void shimeji_free( avatar_t *spShimeji );

void avatar_set_image( avatar_t* spAvatar, u16 sFrame, bool sFlipped = false );
void avatar_set_image( avatar_t* spAvatar, const std::string& srPath, bool sFlipped = false );

void avatar_set_pos( avatar_t* spAvatar, u16 sX, u16 sY );

// TODO: be able to set a rotation anchor point, like on the 2d sprites in chocolate
void avatar_set_ang( avatar_t* spAvatar, u16 sAng ); 

void avatar_set_scale( avatar_t* spAvatar, char sScaleX, char sScaleY );

