
PROJECT(CadViewer)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)


# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)

INCLUDE(${CMakeModules}/FindOSG.cmake)
INCLUDE(${CMakeModules}/FindBoost.cmake)
INCLUDE(${CMakeModules}/FindVRJuggler.cmake)

SUBDIRS(
${CADKIT_ROOT_DIR}/Usul 
${CADKIT_ROOT_DIR}/Threads/OpenThreads
${CADKIT_ROOT_DIR}/OsgTools
${CADKIT_ROOT_DIR}/XmlTree
${CADKIT_ROOT_DIR}/MenuKit
${CADKIT_ROOT_DIR}/MenuKit/OSG
${CADKIT_ROOT_DIR}/VRV
${CADKIT_ROOT_DIR}/CadViewer/Plugins/scene_operations
${CADKIT_ROOT_DIR}/CadViewer/Plugins/display_scene_stats
${CADKIT_ROOT_DIR}/VRV/GUI
${CADKIT_ROOT_DIR}/VRV/IO
${CADKIT_ROOT_DIR}/Helios/Plugins/SceneModel
)

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
		     "${CADKIT_INC_DIR}/CadViewer"
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     ${VRJ_INC_DIR}
		     "${VRJ_INC_DIR}/tweek/CORBA"
		     "${VRJ_INC_DIR}/tweek/idl"
		     "$ENV{VJ_DEPS_DIR}/include"
		     )

#List the Sources
SET (SOURCES
./Functors/ToolPair.cpp
./Functors/MoveTool.cpp
./Functors/Tool.cpp
./Functors/ScaleTool.cpp
./Pick/Intersect.cpp
./Pick/Select.cpp
./Pick/Seek.cpp
./AppCallbacks.cpp
./main.cpp
./Application.cpp
)


SET ( TARGET cv )

# Create an executable
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
SET ( TARGET ${TARGET}d )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

ADD_EXECUTABLE( ${TARGET} ${SOURCES} )

# Link the Library	
LINK_CADKIT( ${TARGET} Usul OpenThreadsEx OsgTools VRV MenuKit MenuKit_OSG XmlTree )
TARGET_LINK_LIBRARIES( ${TARGET} 
  ${VR_JUGGLER_LIBS}
  ${VJ_DEPS_LIBS}
  ${OPENTHREADS_LIB}
  ${OSG_LIB}
  ${OSG_DB_LIB}
  ${OSG_GA_LIB}
  ${OSG_TEXT_LIB}
  ${OSG_UTIL_LIB}
  ${OSG_FX_LIB}
  ${OPENGL_LIBRARIES}
)
