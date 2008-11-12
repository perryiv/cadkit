
PROJECT(VRV)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# Find needed libraries.
INCLUDE(${CMakeModules}/FindOSG.cmake)
INCLUDE(${CMakeModules}/FindVRJuggler.cmake)
FIND_PACKAGE(OpenGL)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} "$ENV{VJ_DEPS_DIR}/lib" )

ADD_DEFINITIONS("-D_COMPILING_VRV")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
         "${CADKIT_INC_DIR}/GenericNurbs"
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     ${VR_JUGGLER_INCLUDES}
		     "$ENV{VJ_DEPS_DIR}/include"
		     ${CPPDOM_INC_DIR}
		     ${GMTL_INC_DIR}
		     )


#List the Sources
SET (SOURCES
  ./Core/BaseApplication.cpp
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
  ./Prefs/Settings.cpp
)

# Create a Shared Library
ADD_LIBRARY(VRV SHARED ${SOURCES} )
CADKIT_ADD_LIBRARY ( VRV )

# Link the Library	
LINK_CADKIT( VRV Usul OsgTools MenuKit MenuKit_OSG XmlTree )
TARGET_LINK_LIBRARIES( VRV
	${OPENGL_LIBRARIES}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSG_UTIL_LIB}
	${VRJ_LIB}
	${VRJ_OGL_LIB}
	${VPR_LIB}
	${GADGET_LIB}
	${Boost_LIBRARIES}
	${Boost_FILESYSTEM_LIBRARY}
)
