
project (DisplaySceneStats)

INCLUDE ( Cadkit.plugin )

# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)
INCLUDE ( FindOSG )

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${PROJECT_SOURCE_DIR}/../" 
                     ${OPENGL_INCLUDE_DIR} 
                     ${OSG_INC_DIR} )

set (SOURCES
./Component.cpp
./Factory.cpp
)

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "DisplaySceneStats" )
SET ( COMPILE_GUARD "_COMPILING_DISPLAY_SCENE_STATS" )
SET ( CADKIT_LIBRARIES Usul )
SET ( OTHER_LIBRARIES ${OSG_LIB} ${OPENTHREADS_LIB} )

# Add the plugin.
CADKIT_ADD_PLUGIN( ${PLUGIN_NAME} )
