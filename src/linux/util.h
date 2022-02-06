/*
 *	util.h
 *	
 *	Authored by p0lyh3dron on Februrary 5, 2022
 *
 *
 *	Declares the types and functions that will be
 *	useful in other parts of program execution.  
 */
#pragma once
/*
 *  Returns all the files in a directory.
 *
 *  @param  const char *
 *      The path to the directory.
 *  @return char **
 *       A null-terminated array of all the files in the directory.
 */
char **scan_dir( const char *spPath );