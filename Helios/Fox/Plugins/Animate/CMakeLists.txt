

project (FoxAnimate)

ADD_DEFINITIONS ("-D_COMPILING_ANIMATE")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" "${FOXTOOLKIT_INCLUDE}" "${GENERIC_NURBS_INCLUDE_DIR}" )

set (SOURCES
  AnimateComponent.cpp
  AnimateFactory.cpp
  FrameDumpDialog.cpp
  Movie.cpp
)



# Create a Shared Library
ADD_LIBRARY(FoxAnimate SHARED ${SOURCES} )

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
SET_TARGET_PROPERTIES (FoxAnimate
	PROPERTIES SUFFIX ".plugd"
)
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
SET_TARGET_PROPERTIES (FoxAnimate
	PROPERTIES SUFFIX ".plug"
)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(FoxAnimate 
  Usul 
  FoxTools
  OsgTools
  ${FOXTOOLKIT_LIB}
  ${OSG_LIB}
)

