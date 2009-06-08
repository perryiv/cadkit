
#########################################################
#
#  CMakeLists for QtAnimationControl.
#
#########################################################

PROJECT ( QtAnimationControl )

INCLUDE ( Cadkit.plugin )

# Add include paths and librares for QT.
INCLUDE(${QT_USE_FILE})

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${PROJECT_SOURCE_DIR}/../" 
                      ${QT_INCLUDE_DIR} )

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# Set the source and headers.
SET ( SOURCES
  ./AnimationControlComponent.cpp
  ./AnimationControlComponent.h
)

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "QtAnimationControl" )
SET ( COMPILE_GUARD "_COMPILING_ANIMATION_CONTROL" )
SET ( CADKIT_LIBRARIES Usul QtTools )
SET ( OTHER_LIBRARIES ${QT_LIBRARIES} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
