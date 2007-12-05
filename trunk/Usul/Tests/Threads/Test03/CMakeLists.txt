
PROJECT(ThreadTest03)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)


SUBDIRS(
${CADKIT_INC_DIR}/Usul 
${CADKIT_INC_DIR}/Threads/OpenThreads
)

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
		     )

#List the Sources
SET (SOURCES
    Main.cpp
)

SET ( TARGET ThreadTest03 )


IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
SET ( TARGET ${TARGET}d )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Create an executable
ADD_EXECUTABLE( ${TARGET} ${SOURCES} )

# Link the Library	
LINK_CADKIT( ${TARGET} Usul OpenThreadsEx )
