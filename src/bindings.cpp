/*
*	bindings.cpp
*	
*	Authored by Demez on Februrary 13, 2022
*
*
*	Contains the Python Bindings
*/

#include "bindings.h"
#include "manager.h"


// https://llllllllll.github.io/c-extension-tutorial/writing-a-function.html
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


void pyenv_setup_bindings()
{
    PyDoc_STRVAR(get_avatar_pos_doc, "get avatar position in an x and y tuple");

    PyMethodDef get_avatar_pos_method = {
        "get_avatar_pos",                /* The name as a C string. */
        (PyCFunction) py_get_avatar_pos,  /* The C function to invoke. */
        METH_O,               /* Flags telling Python how to invoke ``py_get_avatar_pos`` */
        get_avatar_pos_doc,              /* The docstring as a C string. */
    };

    // now export this thing

    PyMethodDef methods[] = {
        get_avatar_pos_method,
        {NULL},
    };

    PyDoc_STRVAR(cimeji_module_doc, "interact with cimeji program");

    PyModuleDef cimeji_module = {
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

    auto what = PyModule_Create(&cimeji_module);

    if ( PyErr_Occurred() )
        PyErr_Print();
}
#endif

