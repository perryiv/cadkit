
PROJECT(VRV)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# Find needed libraries.
INCLUDE(${CMakeModules}/FindOSG.cmake)
INCLUDE(${CMakeModules}/FindVRJuggler.cmake)

SET( Boost_USE_FILESYSTEM TRUE )
INCLUDE(${CMakeModules}/FindBoost.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

ADD_DEFINITIONS("-D_COMPILING_VRV")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
         "${CADKIT_INC_DIR}/GenericNurbs"
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     ${VRJ_INC_DIR}
		     "$ENV{VJ_DEPS_DIR}/include"
		     )


#List the Sources
SET (SOURCES
  ./Core/Application.cpp
  ./Commands/BackgroundColor.cpp
  ./Commands/Camera.cpp
  ./Commands/LoadDocument.cpp
  ./Commands/Navigator.cpp
  ./Functors/Intersect.cpp
  ./Jobs/SaveImage.cpp
  ./Devices/JoystickDevice.cpp
  ./Devices/ButtonGroup.cpp
  ./Devices/ButtonDevice.cpp
  ./Devices/TrackerDevice.cpp
  ./Devices/Callback.cpp
  ./Prefs/Parser.cpp
  ./Prefs/Settings.cpp
)

# Create a Shared Library
ADD_LIBRARY(VRV SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( VRV Usul OsgTools )
TARGET_LINK_LIBRARIES( VRV
	${OSG_LIB}
	${VR_JUGGLER_LIBS}
	${Boost_FILESYSTEM_LIBRARY}
	${CPPDOM_LIB}
)
