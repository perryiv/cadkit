
PROJECT(Threads)

SET (CMAKE_BUILD_TYPE Debug)
#SET (CMAKE_SKIP_RPATH ON)

#setup output Directories
SET (LIBRARY_OUTPUT_PATH
${PROJECT_SOURCE_DIR}/Build/${CMAKE_BUILD_TYPE}
CACHE PATH
"Single Directory for all Libraries"
)

#setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
${PROJECT_SOURCE_DIR}/Build/${CMAKE_BUILD_TYPE}
CACHE PATH
"SINGLE Directory for all Executables."
)

# ------------ Find OpenThreads Library ----------------------
FIND_LIBRARY(OPENTHREADS_LIB OpenThreads
	/usr/local/lib
	/usr/lib
)

# ------------ Find Usul Library ----------------------
FIND_LIBRARY (USUL_LIBRARY Usul
	${PROJECT_SOURCE_DIR}/../../Usul/Build/${CMAKE_BUILD_TYPE}
	/usr/local/lib
)

#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../../" )

#List the Sources
SET (SOURCES
	Mutex.cpp
)

# Create a Shared Library
ADD_LIBRARY(Threads SHARED ${SOURCES} )

# Link the Library	
TARGET_LINK_LIBRARIES(Threads ${USUL_LIBRARY} ${OPENTHREADS_LIB})
	