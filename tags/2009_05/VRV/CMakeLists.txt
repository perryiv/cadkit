
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
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
             "${CADKIT_INC_DIR}/GenericNurbs"
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     ${VR_JUGGLER_INCLUDES}
		     "$ENV{VJ_DEPS_DIR}/include"
		     ${CPPDOM_INC_DIR}
		     ${GMTL_INC_DIR}
		     ${Boost_INCLUDE_DIR}
		     )

#########################################################
#
#  Add Source groups.
#
#########################################################

SOURCE_GROUP ( Commands      REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Commands/* )
SOURCE_GROUP ( Common        REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Common/* )
SOURCE_GROUP ( Core          REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Core/* )
SOURCE_GROUP ( Devices       REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Devices/* )
SOURCE_GROUP ( Functors      REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Functors/* )
SOURCE_GROUP ( Interfaces    REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Interfaces/* )
SOURCE_GROUP ( Jobs          REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Jobs/* )
SOURCE_GROUP ( Prefs         REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Prefs/* )

SET ( HEADERS
	./Commands/BackgroundColor.h
	./Commands/Camera.h
	./Commands/LoadDocument.h
	./Commands/Navigator.h
	./Common/Buttons.h
	./Common/Constants.h
	./Common/Libraries.h
	./Core/Application.h
	./Core/BaseApplication.h
	./Core/Exceptions.h
	./Core/FunctorHelpers.h
	./Core/JugglerFunctors.h
	./Core/Program.h
	./Core/SharedBoolean.h
	./Core/SharedData.h
	./Core/SharedMap.h
	./Core/SharedVector.h
	./Core/TimerCallback.h
	./Core/TimerServer.h
	./Core/VprIO.h
	./Devices/ButtonDevice.h
	./Devices/ButtonGroup.h
	./Devices/Callback.h
	./Devices/JoystickDevice.h
	./Devices/Messages.h
	./Devices/Referenced.h
	./Devices/TrackerDevice.h
	./Export.h
	./Functors/Intersect.h
	./Interfaces/IAuxiliaryScene.h
	./Interfaces/IModelAdd.h
	./Interfaces/INavigationScene.h
	./Interfaces/IPostModelLoad.h
	./Jobs/SaveImage.h
	./Prefs/Settings.h
)

#List the Sources
SET (SOURCES
  ./Core/BaseApplication.cpp
  ./Core/Application.cpp
  ./Core/TimerCallback.cpp
  ./Core/TimerServer.cpp
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
ADD_LIBRARY(VRV SHARED ${SOURCES} ${HEADERS} )
CADKIT_ADD_LIBRARY ( VRV )

# Link the Library	
LINK_CADKIT( VRV Usul OsgTools MenuKit MenuKit_OSG XmlTree )
TARGET_LINK_LIBRARIES( VRV
	${OPENGL_LIBRARIES}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSG_UTIL_LIB}
	${VRJ_LIBRARY}
	${VRJ_OGL_LIBRARY}
	${VPR_LIBRARY}
	${GADGET_LIBRARY}
	${Boost_LIBRARIES}
	${Boost_FILESYSTEM_LIBRARY}
)
