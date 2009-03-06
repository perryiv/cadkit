
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
#  Find boost.
#
#########################################################

SET ( BOOST_INCLUDEDIR $ENV{BOOST_INC_DIR} )
SET ( BOOST_LIBRARYDIR $ENV{BOOST_LIB_DIR} )
FIND_PACKAGE( Boost 1.36 COMPONENTS date_time filesystem regex threads system )

ADD_SUBDIRECTORY ( Helios )

OPTION ( BUILD_CADKIT_TESTS "Build CadKit test programs."  OFF )

IF ( BUILD_CADKIT_TESTS )
	ADD_SUBDIRECTORY ( Tests )
ENDIF ( BUILD_CADKIT_TESTS )
