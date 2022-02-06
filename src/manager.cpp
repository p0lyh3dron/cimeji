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

static shimeji_t *gpShimejis[ SHIMEJI_MAX_COUNT ];
/*
 *  Returns the list of shimejis.
 *
 *  @return shimeji_t **
 *    An array of size SHIMEJI_MAX_COUNT containing the loaded shimejis.
 */
shimeji_t **get_shimejis( void ) {
    return gpShimejis;
}