
PROJECT(vrjGA)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# setup output Directories
SET (LIBRARY_OUTPUT_PATH
	${CADKIT_BIN_DIR}
	CACHE PATH
	"Single Directory for all Libraries"
)

# setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
	${CADKIT_BIN_DIR}
	CACHE PATH
	"Single Directory for all Executables."
)


# Find needed libraries.
INCLUDE(${CMakeModules}/FindOSG.cmake)
INCLUDE(${CMakeModules}/FindVRJuggler.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

ADD_DEFINITIONS("-D_COMPILING_VRJ_GA")

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
./Referenced.cpp
./TrackerDevice.cpp
./JoystickDevice.cpp
./ButtonGroup.cpp
./ButtonDevice.cpp
./Callback.cpp
)

# Create a Shared Library
ADD_LIBRARY(vrjGA SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( vrjGA Usul OsgTools )
TARGET_LINK_LIBRARIES( vrjGA
	${OSG_LIB}
	${VR_JUGGLER_LIBS}
)

