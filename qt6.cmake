# get absolute path to qmake, then use it to find windeployqt executable

get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

function(windeployqt target)

    # POST_BUILD step
    # - after build, we have a bin/lib for analyzing qt dependencies
    # - we run windeployqt on target and deploy Qt libs

	if( CMAKE_BUILD_TYPE STREQUAL Debug )
		set( WINDEP_CFG "--debug" )
	else()
		set( WINDEP_CFG "--release" )
	endif()

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND "${_qt_bin_dir}/windeployqt.exe"    
                ${WINDEP_CFG}     
                --verbose 1
                --no-svg
                --no-opengl
                --no-opengl-sw
                --no-compiler-runtime
                --no-system-d3d-compiler
                \"$<TARGET_FILE:${target}>\"
        COMMENT "Deploying Qt libraries using windeployqt for compilation target '${target}'..."
    )

endfunction()
