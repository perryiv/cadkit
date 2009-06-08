
#########################################################
#
#  CMakeLists for QtSnapShot.
#
#########################################################

project (QtSnapShot)

# Set the name.  Needed for the macros to set up plugin properties.
SET(PLUGIN_NAME "QtSnapShot")

INCLUDE ( Cadkit.plugin )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${OSG_INC_DIR}" 
                     "${PROJECT_SOURCE_DIR}/../" 
                      ${QT_INCLUDE_DIR}
                      ${CMAKE_CURRENT_BINARY_DIR} )

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

SET (SOURCES
./SnapShotComponent.cpp
./SnapShotComponent.h
./SnapShotFactory.cpp
./SnapShotWidget.cpp
./SnapShotWidget.h
)

SET(MOC_HDRS
  ./SnapShotWidget.h
)

# Make .h from .ui
SET ( UIS 
  SnapShot.ui
)

QT4_WRAP_UI( UIS_HDRS ${UIS} )

# Make the moc files.
QT4_WRAP_CPP( MOC_SRCS ${MOC_HDRS} )

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "QtSnapShot" )
SET ( SOURCES ${SOURCES} ${MOC_SRCS} ${UIS_HDRS} )
SET ( COMPILE_GUARD "_COMPILING_SNAP_SHOT" )
SET ( CADKIT_LIBRARIES Usul OsgTools )
SET ( OTHER_LIBRARIES ${QT_LIBRARIES} ${OSG_LIB} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
