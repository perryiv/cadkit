
PROJECT(FindMinMax)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)


# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
		     )

LINK_DIRECTORIES ( ${CADKIT_BIN_DIR} )

#List the Sources
SET (SOURCES
    ../WrfModel/Parser.cpp
    Main.cpp
)

SET ( TARGET FindMinMax  )

# Create an executable
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
SET ( TARGET ${TARGER}d )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")


ADD_EXECUTABLE( ${TARGET} ${SOURCES} )

# Link the Library	
LINK_CADKIT( ${TARGET} Usul dl )
