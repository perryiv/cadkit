
PROJECT(OpenThreadsEx)



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

    
    
