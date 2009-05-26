
cmake_minimum_required(VERSION "2.6")

PROJECT (CadKit)

# Set variables needed below...
SET ( CADKIT_INC_DIR ${PROJECT_SOURCE_DIR} )
SET ( CMakeModules "${PROJECT_SOURCE_DIR}/CMakeModules")

# Path to look for .cmake files.
SET( CMAKE_MODULE_PATH ${CMakeModules} )

INCLUDE ( Cadkit )

#########################################################
#
#  Set some Apple specific properties.
#
#########################################################

IF (APPLE)
	
	option(OSX_DWARF_DEBUG "Use Dwarf Debug Style" ON)
	IF (OSX_DWARF_DEBUG )
		SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-2")
	ENDIF(OSX_DWARF_DEBUG )

	# CMake doesn't define _DEBUG when using Xcode.
	IF(${CMAKE_GENERATOR} MATCHES "Xcode")
		SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG" )
	ENDIF(${CMAKE_GENERATOR} MATCHES "Xcode")
	
ENDIF (APPLE)


#########################################################
#
#  Find boost.
#
#########################################################

SET ( BOOST_INCLUDEDIR $ENV{BOOST_INC_DIR} )
SET ( BOOST_LIBRARYDIR $ENV{BOOST_LIB_DIR} )
FIND_PACKAGE( Boost 1.39 COMPONENTS date_time filesystem regex thread system )

ADD_SUBDIRECTORY ( Helios )

OPTION ( BUILD_CADKIT_TESTS "Build CadKit test programs."  OFF )

IF ( BUILD_CADKIT_TESTS )
	ADD_SUBDIRECTORY ( Tests )
ENDIF ( BUILD_CADKIT_TESTS )
