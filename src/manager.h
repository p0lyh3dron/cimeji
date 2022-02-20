/*
 *	manager.h
 *	
 *	Authored by p0lyh3dron on Februrary 5, 2022
 *
 *
 *	Declares the types and functions that will be
 *	useful for managing loaded shimejis.  
 */
#pragma once

#define SHIMEJI_MAX_COUNT 255

#include <qdatastream.h>

#include "shimeji.h"

extern std::vector< avatar_t* > gAvatars;

/*
 *  Returns the list of shimejis.
 *
 *  @return avatar_t *
 *    An array of size SHIMEJI_MAX_COUNT containing the loaded shimejis.
 */
std::vector< avatar_t* > get_avatars( void );

