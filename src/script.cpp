/*
*	script.cpp
*	
*	Authored by Demez on Februrary 13, 2022
*
*
*	Contains the Python Environment for avatar thinking
*/

#include "script.h"
#include "bindings.h"
#include "manager.h"
#include "render.h"


std::vector< behavior_t* > gBehaviors;


void pyenv_setup()
{
    Py_Initialize();

    PyObject *sys_path = PySys_GetObject("path");
    PyList_Append(sys_path, PyUnicode_FromString( "." ));

    PyRun_SimpleString( "import os" );
    PyRun_SimpleString( "import sys" );
    PyRun_SimpleString( "sys.path.append(\".\")" );
    PyRun_SimpleString( "sys.path.append(\".\\behaviors\")" );

    pyenv_setup_bindings();
}



static PyObject* py_get_avatar_pos( PyObject* self, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames )
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


static PyObject* py_set_avatar_pos( PyObject* self, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames )
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


static PyObject* py_set_avatar_image( PyObject* self, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames )
{
    // check args
    if ( nargs != 2 )
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

    if ( PyErr_Occurred() )
    {
        PyErr_Print();
        return 0;
    }

    avatar_set_image( avatar, img );

    Py_RETURN_NONE;
}


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


PyDoc_STRVAR(avatar_grabbed_doc, "check if avatar grabbed");
PyDoc_STRVAR(get_avatar_pos_doc, "get avatar position in an x and y tuple");
PyDoc_STRVAR(set_avatar_pos_doc, "set avatar position");
PyDoc_STRVAR(set_avatar_image_doc, "set avatar image");


static PyMethodDef methods[] = {
    {"get_avatar_pos", (PyCFunction)py_get_avatar_pos, METH_FASTCALL, get_avatar_pos_doc},
    {"set_avatar_pos", (PyCFunction)py_set_avatar_pos, METH_FASTCALL, set_avatar_pos_doc},
    {"set_avatar_image", (PyCFunction)py_set_avatar_image, METH_FASTCALL, set_avatar_image_doc},
    {"avatar_grabbed", (PyCFunction)py_avatar_grabbed, METH_FASTCALL, avatar_grabbed_doc},
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



void pyenv_setup_bindings()
{
    auto mod = PyModule_Create( &cimeji_module );

    // In Python3, the module doesn't automatically enter "sys.modules", so do it manually.
    PyObject* key = PyUnicode_FromString( "cimeji" );

    int r = PyObject_SetItem( PySys_GetObject( (char *)"modules" ), key, mod );

    assert( r != -1 );

    if ( PyErr_Occurred() )
        PyErr_Print();
}


behavior_t* pyenv_load_behavior( const std::string& path )
{
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;

    pModule = PyImport_ImportModule( ("behaviors." + path).c_str() );

    if ( pModule == NULL )
    {
        PyErr_Print();
        fprintf( stderr, "Failed to load \"%s\"\n", path.c_str() );
        return nullptr;
    }

    if ( !(pFunc = PyObject_GetAttrString( pModule, "think" )) )
    {
        PyErr_Print();
        fprintf( stderr, "Failed to load think func in \"%s\"\n", path.c_str() );
        Py_DECREF( pModule );
        return nullptr;
    }

    behavior_t* behavior = new behavior_t {
        .aPath = path,
        .apModule = pModule,
        .apThinkFunc = pFunc
    };

    gBehaviors.push_back( behavior );

    return behavior;
}


bool pyenv_run_behavior( behavior_t* spBehavior, avatar_t* spAvatar, float sDT )
{
    PyObject* pArgs = PyTuple_New( 2 );

    PyObject* pHandle = PyFloat_FromDouble( (size_t)spAvatar );
    PyObject* pDT = PyFloat_FromDouble( sDT );

    // pDT reference stolen here:
    PyTuple_SetItem( pArgs, 0, pHandle );
    PyTuple_SetItem( pArgs, 1, pDT );

    if ( PyErr_Occurred() )
        PyErr_Print();

    PyObject* pValue = PyObject_CallObject( spBehavior->apThinkFunc, pArgs );

    if ( PyErr_Occurred() )
        PyErr_Print();

    Py_DECREF( pArgs );

    /* pFunc is a new reference */

    /*if ( pFunc && PyCallable_Check(pFunc) ) {
        pArgs = PyTuple_New(argc - 3);
        for (i = 0; i < argc - 3; ++i) {
            pValue = PyLong_FromLong(atoi(argv[i + 3]));
            if (!pValue) {
                Py_DECREF(pArgs);
                Py_DECREF(pModule);
                fprintf(stderr, "Cannot convert argument\n");
                return 1;
            }
            // pValue reference stolen here:
            PyTuple_SetItem(pArgs, i, pValue);
        }
        pValue = PyObject_CallObject(pFunc, pArgs);
        Py_DECREF(pArgs);
        if (pValue != NULL) {
            printf("Result of call: %ld\n", PyLong_AsLong(pValue));
            Py_DECREF(pValue);
        }
        else {
            Py_DECREF(pFunc);
            Py_DECREF(pModule);
            PyErr_Print();
            fprintf(stderr,"Call failed\n");
            return 1;
        }
    }
    else {
        if (PyErr_Occurred())
            PyErr_Print();
        fprintf(stderr, "Cannot find function \"%s\"\n", argv[2]);
    }*/

    return true;
}


avatar_t* pyenv_get_avatar( PyObject* spPyObj )
{
    size_t handle = PyFloat_AsDouble( spPyObj );

    for ( auto avatar: gAvatars )
    {
        if ( (size_t)avatar == handle )
            return avatar;
    }

    return nullptr;
}


window_t* pyenv_get_window( PyObject* spPyObj )
{
    size_t handle = PyFloat_AsDouble( spPyObj );

    return nullptr;
}


