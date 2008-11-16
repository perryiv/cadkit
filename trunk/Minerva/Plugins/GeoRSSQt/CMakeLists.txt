
#########################################################
#
#  CMakeLists for QtGeoRSS.
#
#########################################################

project (QtGeoRSS)

SET(PLUGIN_NAME "QtGeoRSS")

SET( CMakeModules "${CADKIT_INC_DIR}/CMakeModules" )
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

set(COMPILE_GUARD "_COMPILING_OSSIM_LAYER_QT")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${CADKIT_INC_DIR}/Experimental"
                     "${PROJECT_SOURCE_DIR}/../" 
                     ${OSG_INC_DIR}
                     ${QT_INCLUDE_DIR} )

set (SOURCES
  AddOssimLayerWidget.cpp
  OssimLayerQtComponent.cpp
  OssimLayerQtFactory.cpp
)

SET(MOC_HDRS
  AddOssimLayerWidget.h
)

# Make the moc files.
QT4_WRAP_CPP( MOC_SRCS ${MOC_HDRS} )

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} ${MOC_SRCS} )

CADKIT_PLUGIN()

# Link the Library
LINK_CADKIT( ${PLUGIN_NAME} Usul QtTools Minerva )

TARGET_LINK_LIBRARIES( ${PLUGIN_NAME}
	${Boost_LIBRARIES}
)

