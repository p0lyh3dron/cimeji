/*
 *  Returns all the files in a directory.
 *
 *  @param  const char *
 *      The path to the directory.
 *  @return char **
 *       A null-terminated array of all the files in the directory.
 */
#include "util.h"

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <stdlib.h>
/*
 *  Returns all the files in a directory.
 *
 *  @param  const char *
 *      The path to the directory.
 *  @return char **
 *       A null-terminated array of all the files in the directory.
 */
char **scan_dir( const char *spPath ) {
    int            i          = 0;
    char         **apFiles    = NULL;
    struct dirent *pDirEntry;
    DIR           *pDirStream = opendir( spPath );
    if( !pDirStream ) {
        fprintf( stderr, "Failed to open directory: %s\n", spPath );
        return NULL;
    }
    while( ( pDirEntry = readdir( pDirStream ) ) != NULL ) {
        if( strncmp( pDirEntry->d_name, ".", 1 ) == 0 || strncmp( pDirEntry->d_name, "..", 2 ) == 0 ) {
            continue;
        }
        if ( pDirEntry->d_type != DT_REG ) {
            continue;
        }
        char buf[ PATH_MAX + 1 ];
        realpath( spPath, buf );
        strncat( buf, "/", 2 );
        strncat( buf, pDirEntry->d_name, strlen( pDirEntry->d_name ) );
        apFiles = ( char ** )realloc( apFiles, sizeof( const char * ) * ( i + 1 ) );
        if( !apFiles ) {
            fprintf( stderr, "Failed to allocate memory for file list.\n" );
            return NULL;
        }
        apFiles[ i ] = ( char * )malloc( strlen( buf ) + 1 );
        strncpy( apFiles[ i ], buf, strlen( buf ) );
        apFiles[ i ][ strlen( buf ) ] = '\0';
        ++i;
    }
    apFiles = ( char ** )realloc( apFiles, sizeof( const char * ) * ( i + 1 ) );
    apFiles[ i ] = NULL;
    closedir( pDirStream );
    return apFiles;
}