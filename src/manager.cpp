/*
 *	manager.cpp
 *	
 *	Authored by p0lyh3dron on Februrary 5, 2022
 *
 *
 *	Defines the types and functions that will be
 *	useful for managing loaded shimejis.  
 */
#pragma once

#include "manager.h"

#include <vector>

std::vector< avatar_t* > gAvatars;
/*
 *  Returns the list of shimejis.
 *
 *  @return avatar_t **
 *    An array of size SHIMEJI_MAX_COUNT containing the loaded shimejis.
 */
std::vector< avatar_t* > get_avatars( void )
{
    return gAvatars;
}



