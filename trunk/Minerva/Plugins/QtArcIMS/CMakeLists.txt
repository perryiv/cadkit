
#########################################################
#
#  CMakeLists for WmsLayerQt.
#
#########################################################

PROJECT (QtArcIMS)

INCLUDE ( Cadkit.plugin )

INCLUDE(${QT_USE_FILE})

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${PROJECT_SOURCE_DIR}/../" 
                     ${OSG_INC_DIR}
                     ${QT_INCLUDE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR} )

SET (SOURCES
  QtArcIMSComponent.cpp
  QtArcIMSComponent.h
)

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "QtArcIMS" )
SET ( COMPILE_GUARD "_COMPILING_ARC_IMS_LAYER_QT" )
SET ( CADKIT_LIBRARIES Usul Minerva MinervaQtWidgets )
SET ( OTHER_LIBRARIES ${QT_LIBRARIES} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
