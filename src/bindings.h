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


PyDoc_STRVAR(avatar_get_pos_doc, "get avatar position in an x and y tuple");

static PyObject* py_avatar_get_pos( PyObject* self, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames )
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


static PyMethodDef methods[] = {
    {"avatar_get_pos",      (PyCFunction)py_avatar_get_pos, METH_FASTCALL, avatar_get_pos_doc},
    {"avatar_set_pos",      (PyCFunction)py_avatar_set_pos, METH_FASTCALL, avatar_set_pos_doc},
    {"avatar_set_image",    (PyCFunction)py_avatar_set_image, METH_FASTCALL, avatar_set_image_doc},
    {"avatar_grabbed",      (PyCFunction)py_avatar_grabbed, METH_FASTCALL, avatar_grabbed_doc},

    {"env_find_window",     (PyCFunction)py_avatar_grabbed, METH_FASTCALL, avatar_grabbed_doc},

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

