
PROJECT(OpenThreadsEx)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# is this just a Apple thing or just a gcc thing?
SET (CMAKE_SKIP_RPATH OFF)


#setup output Directories
SET (LIBRARY_OUTPUT_PATH
${CADKIT_BIN_DIR}
CACHE PATH
"Single Directory for all Libraries"
)

#setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
${CADKIT_BIN_DIR}
CACHE PATH
"SINGLE Directory for all Executables."
)


# ------------ Find OpenThreads Library ----------------------
IF(NOT OPENTHREADS_LIB)
       INCLUDE(${CMakeModules}/FindOSG.cmake)
ENDIF( NOT OPENTHREADS_LIB )

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "$ENV{CADKIT_ROOT_DIR}" )

IF ( OT_INC_DIR )
INCLUDE_DIRECTORIES( "$ENV{OPENTHREADS_INC_DIR}" )
ENDIF ( OT_INC_DIR )

INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../../" )

ADD_DEFINITIONS("-D_COMPILING_OPEN_THREADS_EXTENSIONS")

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

#List the Sources
SET (SOURCES
	Mutex.cpp
	Thread.cpp
)

# Create a Shared Library
ADD_LIBRARY(OpenThreadsEx SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( OpenThreadsEx Usul )
TARGET_LINK_LIBRARIES(OpenThreadsEx ${OPENTHREADS_LIB})
