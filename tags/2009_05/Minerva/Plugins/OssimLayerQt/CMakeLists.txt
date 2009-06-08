
#########################################################
#
#  CMakeLists for OssimLayerQt.
#  This plugin needs to be renamed, because it has nothing to do with Ossim anymore.
#
#########################################################

project (OssimLayerQt)

INCLUDE ( Cadkit.plugin )

# Adds includes and libraries for QT.
INCLUDE(${QT_USE_FILE})

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} ${Boost_LIBRARY_DIRS} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR}
					 ${CADKIT_INC_DIR} 
                     "${PROJECT_SOURCE_DIR}/../" 
                     ${OSG_INC_DIR}
                     ${QT_INCLUDE_DIR} )

set (SOURCES
  AddOssimLayerWidget.cpp
  AddOssimLayerWidget.h
  OssimLayerQtComponent.cpp
  OssimLayerQtComponent.h
)

SET(MOC_HDRS
  AddOssimLayerWidget.h
)

# Make the moc files.
QT4_WRAP_CPP( MOC_SRCS ${MOC_HDRS} )

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "OssimLayerQt" )
SET ( SOURCES ${SOURCES} ${MOC_SRCS} )
SET ( COMPILE_GUARD "_COMPILING_OSSIM_LAYER_QT" )
SET ( CADKIT_LIBRARIES Usul QtTools Minerva )
SET ( OTHER_LIBRARIES ${QT_LIBRARIES} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
