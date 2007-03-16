PROJECT(OpenThreadsEx)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")


# setup output Directories
SET (LIBRARY_OUTPUT_PATH
	${PROJECT_BINARY_DIR}/Bin
	CACHE PATH
	"Single Directory for all Libraries"
)

#setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
	${PROJECT_BINARY_DIR}/Bin
	CACHE PATH
	"Single Directory for all Executables."
)

# Make sure Usul is built
SUBDIRS (${PROJECT_SOURCE_DIR}/../Usul)

# Find OSG
INCLUDE ( ${CMakeModules}/FindOSG.cmake)

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../" )

#List the Sources
SET (SOURCES
	OpenThreads/Mutex.cpp
)

# Create a Shared Library
ADD_LIBRARY(OpenThreadsEx SHARED ${SOURCES} )

# Link the Library	
TARGET_LINK_LIBRARIES(OpenThreadsEx 
	Usul 
	${OPENTHREADS_LIB}
)

    
    
