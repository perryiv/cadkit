
PROJECT(ThreadTest01)

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

SET ( TARGET ThreadTest01 )

# Create an executable
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
SET ( TARGET ThreadTest01d )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")


# Create an executable
IF (CMAKE_BUILD_TYPE STREQUAL "Release")
SET ( TARGET ThreadTest01 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Release")

ADD_EXECUTABLE( ${TARGET} ${SOURCES} )

# Link the Library	
LINK_CADKIT( ${TARGET} Usul OpenThreadsEx )
