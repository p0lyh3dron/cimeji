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

#include "shimeji.h"
/*
 *  Returns the list of shimejis.
 *
 *  @return shimeji_t *
 *    An array of size SHIMEJI_MAX_COUNT containing the loaded shimejis.
 */
shimeji_t **get_shimejis( void );