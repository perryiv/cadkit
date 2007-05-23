
PROJECT(VRVPrefs)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")
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
INCLUDE(${CMakeModules}/FindVRJuggler.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

ADD_DEFINITIONS("-D_COMPILING_VRV_PREFS")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
		     "$ENV{VJ_DEPS_DIR}/include"
		     )


#List the Sources
SET (SOURCES
./Settings.cpp
./Parser.cpp
)

# Create a Shared Library
ADD_LIBRARY( VRVPrefs SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( VRVPrefs Usul )


