

PROJECT (Animate)

SET(PLUGIN_NAME "Animate")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

ADD_DEFINITIONS ("-D_COMPILING_ANIMATE")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
	"${PROJECT_SOURCE_DIR}/../" 
  "${CADKIT_INC_DIR}/GenericNurbs"
  ${OSG_INC_DIR}
)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

set (SOURCES
  AnimateComponent.cpp
  AnimateFactory.cpp
  Frame.cpp
  KeyFramePath.cpp
  Path.cpp
  RecordedPath.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN()

# Link the Library
LINK_CADKIT( ${PLUGIN_NAME} Usul MenuKit SerializeXML )


