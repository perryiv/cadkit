
#########################################################
#
#  CMakeLists for QtSceneDelegate.
#
#########################################################

project (QtSceneDelegate)

INCLUDE ( Cadkit.plugin)

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${OSG_INC_DIR}" 
                     "${PROJECT_SOURCE_DIR}/../" 
                      ${QT_INCLUDE_DIR} )

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

set (SOURCES
  SceneDelegateComponent.cpp
  SceneDelegateComponent.h
)

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "QtSceneDelegate" )
SET ( SOURCES ${SOURCES} ${MOC_SRCS} )
SET ( COMPILE_GUARD "_COMPILING_SCENE_DELEGATE" )
SET ( CADKIT_LIBRARIES Usul OsgTools HeliosQtViewsOSG )
SET ( OTHER_LIBRARIES ${OPENTHREADS_LIB} ${OSG_LIB} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
