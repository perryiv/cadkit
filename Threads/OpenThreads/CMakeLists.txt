
PROJECT(OpenThreadsEx)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# Setup output Directories
SET (LIBRARY_OUTPUT_PATH
${CADKIT_BIN_DIR}
CACHE PATH
"Single Directory for all Libraries"
)

# Setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
${CADKIT_BIN_DIR}
CACHE PATH
"SINGLE Directory for all Executables."
)


# ------------ Find OpenThreads Library ----------------------
INCLUDE(${CMakeModules}/FindOSG.cmake)

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../../" 
                     "$ENV{CADKIT_INC_DIR}" 
                     ${OT_INC_DIR} )

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
TARGET_LINK_LIBRARIES(OpenThreadsEx ${OPENTHREADS_LIB} )
