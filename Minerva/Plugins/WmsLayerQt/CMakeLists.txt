
#########################################################
#
#  CMakeLists for WmsLayerQt.
#
#########################################################

PROJECT ( WmsLayerQt )

INCLUDE ( Cadkit.plugin )

# Add include paths and librares for QT.
INCLUDE(${QT_USE_FILE})

# Set the link directories.
LINK_DIRECTORIES( ${CADKIT_BIN_DIR} ${Boost_LIBRARY_DIRS} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR}
					 ${CADKIT_INC_DIR} 
                     "${PROJECT_SOURCE_DIR}/../" 
                     ${OSG_INC_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR} )

# Set the sources.
SET (SOURCES
  EditWmsLayerWidget.cpp
  WmsLayerQtComponent.cpp
)

# Moc these headers.
SET(MOC_HDRS
  EditWmsLayerWidget.h
)

# Make .h from .ui
SET ( UIS 
  EditWmsLayerWidget.ui 
)

QT4_WRAP_UI( UIS_HDRS ${UIS} )

# Make the moc files.
QT4_WRAP_CPP( MOC_SRCS ${MOC_HDRS} )

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "WmsLayerQt" )
SET ( SOURCES ${SOURCES} ${MOC_SRCS} ${UIS_HDRS} )
SET ( COMPILE_GUARD "_COMPILING_WMS_LAYER_QT" )
SET ( CADKIT_LIBRARIES Usul Minerva MinervaQtWidgets )
SET ( OTHER_LIBRARIES ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${QT_LIBRARIES} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
