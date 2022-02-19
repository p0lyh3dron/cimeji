/*
 *	bindings.h
 *	
 *	Authored by Demez on Februrary 13, 2022
 *
 *
 *	Contains the Python Bindings
 */
#pragma once

#include "script.h"


#if 0
static PyObject* py_get_avatar_pos( PyObject* self , PyObject* handle )
{
    avatar_t* avatar = pyenv_get_avatar(handle);
    if ( !avatar )
        return nullptr;

    PyObject* pArgs = PyTuple_New( 2 );

    PyObject* pX = PyLong_FromLong( avatar->aPos[0] );
    PyObject* pY = PyLong_FromLong( avatar->aPos[1] );

    // pX and pY references stolen here:
    PyTuple_SetItem( pArgs, 0, pX );
    PyTuple_SetItem( pArgs, 1, pY );

    return pArgs;
}
#endif

