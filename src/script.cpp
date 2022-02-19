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


