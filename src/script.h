/*
 *	script.h
 *	
 *	Authored by Demez on Februrary 13, 2022
 *
 *
 *	Contains the Python Environment for avatar thinking
 */
#pragma once

#include "shimeji.h"
#include "env.h"

#ifdef slots
#undef slots
#endif

#ifdef _DEBUG
	#undef _DEBUG
	#include <Python.h>
	#define _DEBUG 1
#else
	#include <Python.h>
#endif


using avatar_handle_t = void*;


// behavior script data
struct behavior_t
{
	std::string aPath;

	PyObject* apModule;
	PyObject* apThinkFunc;
};


extern std::vector< behavior_t* > gBehaviors;

void pyenv_setup();
void pyenv_setup_bindings();

behavior_t* pyenv_load_behavior( const std::string& srPath );

bool pyenv_run_behavior( behavior_t* spBehavior, avatar_t* spAvatar, float sDT );

// --------------------------------------------------------------------------------------
// Below are util functions for handling certain values called from the python script

avatar_t* pyenv_get_avatar( PyObject* spPyObj );
window_t* pyenv_get_window( PyObject* spPyObj );


