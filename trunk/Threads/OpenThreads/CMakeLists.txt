
PROJECT(Threads)

SET (CMAKE_BUILD_TYPE Debug)
SET (CMAKE_SKIP_RPATH OFF)

#setup output Directories
SET (LIBRARY_OUTPUT_PATH
$ENV{USUL_LIB_DIR}
CACHE PATH
"Single Directory for all Libraries"
)

#setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
$ENV{USUL_LIB_DIR}
CACHE PATH
"SINGLE Directory for all Executables."
)

# ------------ Find OpenThreads Library ----------------------
IF(NOT OPENTHREADS_LIB)
       INCLUDE($ENV{CADKIT_ROOT_DIR}/Usul/Make/FindOSG.cmake)
ENDIF( NOT OPENTHREADS_LIB )

# ------------ Find Usul Library ----------------------
FIND_LIBRARY (USUL_LIBRARY Usul
	$ENV{USUL_LIB_DIR}
	/usr/local/lib
)

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "$ENV{CADKIT_ROOT_DIR}" )
INCLUDE_DIRECTORIES( "$ENV{OPENTHREADS_INC_DIR}" )
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../../" )


# Add _DEBUG definition is it's debug.
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
ADD_DEFINITIONS("-D_DEBUG")
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")


# Add NDEBUG definition is it's release.
IF (CMAKE_BUILD_TYPE STREQUAL "Release")
ADD_DEFINITIONS("-DNDEBUG")
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Release")


#List the Sources
SET (SOURCES
	Mutex.cpp
)

# Create a Shared Library
ADD_LIBRARY(Threads SHARED ${SOURCES} )

# Link the Library	
TARGET_LINK_LIBRARIES(Threads ${USUL_LIBRARY} ${OPENTHREADS_LIB})
