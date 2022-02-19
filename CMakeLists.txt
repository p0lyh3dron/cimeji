cmake_minimum_required( VERSION 3.18 FATAL_ERROR )

project( Cimeji )

set( SRC_FILES
  src/main.cpp
  src/app.cpp
  src/app.h
  src/shimeji.cpp
  src/shimeji.h
  src/manager.cpp
  src/manager.h
  src/script.cpp
  src/script.h
  src/bindings.cpp
  src/bindings.h
)

if( MSVC )
set( PLAT_FILES
    src/win/render.cpp
    src/win/render.h
    src/win/util.cpp
    src/win/util.h
    src/win/input.cpp
    src/win/input.h
    src/win/env.cpp
    src/win/env.h
)
else()
set( PLAT_FILES
    src/linux/render.cpp
    src/linux/surface.cpp
    src/linux/util.cpp
    src/linux/input.cpp
)
endif()

include( core.cmake )

# funny
if( MSVC )
	set( CMAKE_PREFIX_PATH "C:/tools/Qt/6.2.2/msvc2019_64" )
else()
  set( CMAKE_PREFIX_PATH "/home/karl/Qt/6.2.3/gcc_64/" )
endif()

find_package(
	Qt6 COMPONENTS
	Core
	Widgets
	Gui
	REQUIRED
)

find_package(
	Python COMPONENTS
	Interpreter
	Development
	REQUIRED
)

#message( "Python_STDLIB " ${Python_STDLIB} )
#message( "Python_SITELIB " ${Python_SITELIB} )
#message( "Python_SITEARCH " ${Python_SITEARCH} )

message( "Python_INCLUDE_DIRS " ${Python_INCLUDE_DIRS} )
message( "Python_LIBRARIES " ${Python_LIBRARIES} )
message( "Python_LIBRARY_DIRS " ${Python_LIBRARY_DIRS} )
message( "Python_RUNTIME_LIBRARY_DIRS " ${Python_RUNTIME_LIBRARY_DIRS} )

add_executable( cimeji ${SRC_FILES} ${PLAT_FILES} )

target_include_directories(
	cimeji PRIVATE
	
	${Python_INCLUDE_DIRS}
)

# actually stupid
string( REGEX REPLACE "_d.lib" ".lib" Python_LIBRARIES "${Python_LIBRARIES}" )

target_link_libraries(
	cimeji
	
	Qt6::Core
	Qt6::Widgets
	Qt6::Gui
	
	${Python_LIBRARIES}
)

if( MSVC )
	include( qt6.cmake )
	windeployqt( cimeji )
	
	qt6_disable_unicode_defines( cimeji )
	
	# copy spng dll
	add_custom_command( TARGET cimeji POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SPNG_LIB_DIR}/spng.dll $<TARGET_FILE_DIR:cimeji>
	)
endif()
