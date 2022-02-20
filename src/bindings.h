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
#include "env.h"


#define PY_FUNC( func, help ) \
    PyDoc_STRVAR( func##_doc, help ); \
    static PyObject* py_##func( PyObject* self, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames ) \


PY_FUNC( avatar_get_pos, "get avatar position in an x and y tuple" )
{
    // check args
    if ( nargs != 1 )
    {
        printf( "\"get_avatar_pos()\" Needs only avatar passed into it: arg count: %zu\n", nargs );
        return nullptr;
    }

    PyObject* handle = args[0];

    avatar_t* avatar = pyenv_get_avatar( handle );
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


PyDoc_STRVAR(avatar_set_pos_doc, "set avatar position");

static PyObject* py_avatar_set_pos( PyObject* self, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames )
{
    // check args
    if ( nargs != 3 )
    {
        printf( "\"set_avatar_pos()\" Needs Avatar, X, and Y pos passed into it: arg count: %zu\n", nargs );
        return nullptr;
    }

    avatar_t* avatar = pyenv_get_avatar( args[0] );
    if ( !avatar )
        return nullptr;

    PyObject* pyX = args[1];
    PyObject* pyY = args[2];

    int x = PyFloat_AsDouble( pyX );

    if ( PyErr_Occurred() )
    {
        PyErr_Print();
        return 0;
    }

    int y = PyFloat_AsDouble( pyY );

    if ( PyErr_Occurred() )
    {
        PyErr_Print();
        return 0;
    }

    avatar->aPos[0] = x;
    avatar->aPos[1] = y;

    Py_RETURN_NONE;
}


PyDoc_STRVAR(avatar_set_image_doc, "set avatar image");

static PyObject* py_avatar_set_image( PyObject* self, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames )
{
    // check args
    if ( nargs < 2 )
    {
        printf( "\"set_avatar_pos()\" Needs Avatar and image path passed into it: arg count: %zu\n", nargs );
        return nullptr;
    }

    avatar_t* avatar = pyenv_get_avatar( args[0] );
    if ( !avatar )
        return nullptr;

    PyObject* pyImg = args[1];

    Py_ssize_t size;
    const char* img = PyUnicode_AsUTF8AndSize( pyImg, &size );

    bool flip = false;

    if ( nargs == 3 )
    {
        // probably not the right function but whatever lol
        flip = PyFloat_AsDouble( args[2] );
    }

    if ( PyErr_Occurred() )
    {
        PyErr_Print();
        return 0;
    }

    avatar_set_image( avatar, img, flip );

    Py_RETURN_NONE;
}


PyDoc_STRVAR(avatar_grabbed_doc, "check if avatar grabbed");

static PyObject* py_avatar_grabbed( PyObject* self, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames )
{
    if ( nargs != 1 )
    {
        printf( "\"avatar_grabbed()\" Needs Avatar passed into it: arg count: %zu\n", nargs );
        return nullptr;
    }

    avatar_t* avatar = pyenv_get_avatar( args[0] );
    if ( !avatar )
        return nullptr;

    PyObject* ret = PyBool_FromLong( avatar->aGrabbed );

    return ret;
}


// -------------------------------------------------------------------------
// Desktop Environment Bindings


PyDoc_STRVAR(env_find_window_doc, "Find a window within the bounds of the avatar");

static PyObject* py_env_find_window( PyObject* self, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames )
{
    if ( nargs != 1 )
    {
        printf( "\"env_find_window()\" Needs Avatar passed into it: arg count: %zu\n", nargs );
        return nullptr;
    }

    avatar_t* avatar = pyenv_get_avatar( args[0] );
    if ( !avatar )
        return nullptr;

    window_t window = env_find_window( avatar );

    PyObject* pyWindow = PyFloat_FromDouble( (size_t)window );

    return pyWindow;
}


PyDoc_STRVAR(env_cursor_pos_doc, "get cursor position");

static PyObject* py_env_cursor_pos( PyObject* self, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames )
{
    // check args
    if ( nargs != 0 )
    {
        printf( "\"get_avatar_pos()\" Needs no parameters passed into it\n" );
        return nullptr;
    }

    int x, y;
    env_cursor_pos( x, y );

    PyObject* pArgs = PyTuple_New( 2 );

    PyObject* pX = PyLong_FromLong( x );
    PyObject* pY = PyLong_FromLong( y );

    // pX and pY references stolen here:
    PyTuple_SetItem( pArgs, 0, pX );
    PyTuple_SetItem( pArgs, 1, pY );

    return pArgs;
}


PY_FUNC( env_get_monitor_count, "get monitor count" )
{
    // check args
    if ( nargs != 0 )
    {
        printf( "\"get_avatar_pos()\" Needs no parameters passed into it\n" );
        return nullptr;
    }

    int count = env_get_monitor_count();

    PyObject* pCount = PyLong_FromLong( count );

    return pCount;
}


// -------------------------------------------------------------------------
// Setup Methods


#define ADD_METHOD( func, method ) {#func, (PyCFunction)py_##func, method, func##_doc}


static PyMethodDef methods[] = {
    ADD_METHOD( avatar_get_pos,             METH_FASTCALL ),
    ADD_METHOD( avatar_set_pos,             METH_FASTCALL ),
    ADD_METHOD( avatar_set_image,           METH_FASTCALL ),
    ADD_METHOD( avatar_grabbed,             METH_FASTCALL ),

    ADD_METHOD( env_find_window,            METH_FASTCALL ),
    ADD_METHOD( env_cursor_pos,             METH_FASTCALL ),

    ADD_METHOD( env_get_monitor_count,      METH_FASTCALL ),

    {NULL, NULL, 0, NULL},
};


PyDoc_STRVAR(cimeji_module_doc, "interact with cimeji program");


static PyModuleDef cimeji_module = {
    PyModuleDef_HEAD_INIT,
    "cimeji",
    cimeji_module_doc,
    -1,
    methods,
    NULL,
    NULL,
    NULL,
    NULL
};


#undef PY_FUNC
#undef ADD_METHOD

