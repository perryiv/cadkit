
PROJECT(osgVRJ)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# Find needed libraries.
INCLUDE(${CMakeModules}/FindOSG.cmake)
INCLUDE(${CMakeModules}/FindVRJuggler.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

ADD_DEFINITIONS("-D_COMPILING_OSG_VRJ")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     ${VRJ_INC_DIR}
		     "$ENV{VJ_DEPS_DIR}/include"
		     )


#List the Sources
SET (SOURCES
  ./Application.cpp
)

# Create a Shared Library
ADD_LIBRARY(osgVRJ SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( osgVRJ Usul OsgTools )
TARGET_LINK_LIBRARIES( osgVRJ
	${OSG_LIB}
	${VR_JUGGLER_LIBS}
)

