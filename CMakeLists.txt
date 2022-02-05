cmake_minimum_required( VERSION 3.18 FATAL_ERROR )

project( Cimeji )

set( SRC_FILES
  src/main.cpp
  src/app.cpp
  src/linux/surface.cpp
)

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

add_executable( cimeji ${SRC_FILES} )

target_link_libraries(
	cimeji
	
	Qt6::Core
	Qt6::Widgets
	Qt6::Gui
)

if( MSVC )
	include( qt6.cmake )
	windeployqt( cimeji )
endif()
