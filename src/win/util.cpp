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
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <stdlib.h>

#include <string> 

// win32
#include <direct.h>
#include <fileapi.h> 
#include <shlwapi.h> 
#include <minwinbase.h> 


/*
 *  Returns all the files in a directory.
 *
 *  @param  const char *
 *      The path to the directory.
 *  @return char **
 *       A null-terminated array of all the files in the directory.
 */
char **scan_dir( const char *spPath )
{
    WIN32_FIND_DATA ffd;
    int             i       = 0;
    char          **apFiles = NULL;

    std::string scanDir = spPath;

    HANDLE hFind = FindFirstFile( (scanDir + "\\*").c_str(), &ffd );

    if ( INVALID_HANDLE_VALUE == hFind )
        return apFiles;

    while ( FindNextFile( hFind, &ffd ) != 0 )
    {
        bool isDir = ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

        // bool isDir = filesys->IsDir( path + "\\" + ffd.cFileName, true );

        if ( isDir || strncmp( ffd.cFileName, "..", sizeof(ffd.cFileName) ) == 0 )
            continue;

        char buf[ MAX_PATH ];
        memset( buf, 0, MAX_PATH );
        strncat( buf, scanDir.c_str(), scanDir.length() );
        strncat( buf, "\\", 2 );
        strncat( buf, ffd.cFileName, strlen( ffd.cFileName ) );

        apFiles = ( char ** )realloc( apFiles, sizeof( const char * ) * ( i + 1 ) );
        if( !apFiles ) {
            fprintf( stderr, "Failed to allocate memory for file list.\n" );
            return NULL;
        }

        int bufLen = strlen( buf );

        apFiles[ i ] = ( char * )malloc( bufLen + 1 );
        strncpy( apFiles[ i ], buf, bufLen );
        apFiles[ i ][ bufLen ] = '\0';
        ++i;
    }

    apFiles = ( char ** )realloc( apFiles, sizeof( const char * ) * ( i + 1 ) );
    apFiles[ i ] = NULL;

    FindClose( hFind );

    return apFiles;
}

