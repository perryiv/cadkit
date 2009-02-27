
cmake_minimum_required(VERSION "2.6")

PROJECT (CadKit)

# Set variables needed below...
SET ( CADKIT_INC_DIR ${PROJECT_SOURCE_DIR} )
SET ( CMakeModules "${PROJECT_SOURCE_DIR}/CMakeModules")

INCLUDE ( ${CMakeModules}/Cadkit.cmake )

ADD_SUBDIRECTORY ( Helios )

OPTION ( BUILD_CADKIT_TESTS "Build CadKit test programs."  OFF )

IF ( BUILD_CADKIT_TESTS )
	ADD_SUBDIRECTORY ( Tests )
ENDIF ( BUILD_CADKIT_TESTS )
