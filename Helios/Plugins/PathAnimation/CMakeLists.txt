PROJECT (PathAnimation)

SET(PLUGIN_NAME "PathAnimation")

SET(CMakeModules "${CADKIT_INC_DIR}/CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

ADD_DEFINITIONS ("-D_COMPILING_PATH_ANIMATION")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
  "${PROJECT_SOURCE_DIR}/../"
	"${CADKIT_INC_DIR}/" 
  "${CADKIT_INC_DIR}/GenericNurbs"
  "${CADKIT_INC_DIR}/Experimental"
  ${OSG_INC_DIR}
)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

set (SOURCES
./PathAnimationComponent.cpp
./CurvePlayer.cpp
./CameraPath.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN( ${PLUGIN_NAME} )

# Link the Library
LINK_CADKIT( ${PLUGIN_NAME} Usul MenuKit SerializeXML OsgTools )

TARGET_LINK_LIBRARIES( ${PLUGIN_NAME} ${OSG_LIB} )
