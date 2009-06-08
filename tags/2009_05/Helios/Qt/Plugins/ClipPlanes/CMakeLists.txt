
#########################################################
#
#  CMakeLists for QtClipPlanes.
#
#########################################################

project (QtClipPlanes)

INCLUDE ( Cadkit.plugin )

# Adds includes and libraries for QT.
INCLUDE(${QT_USE_FILE})

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${OSG_INC_DIR}" 
                     "${PROJECT_SOURCE_DIR}/../" 
                      ${QT_INCLUDE_DIR}
                      ${CMAKE_CURRENT_BINARY_DIR} )


SET ( SOURCES
  ./ClipPlanesComponent.cpp
  ./ClipPlanesComponent.h
  ./ClipPlanes.cpp
  ./ClipPlanes.h
)

SET(MOC_HDRS
  ./ClipPlanes.h
)

# Make .h from .ui
SET ( UIS 
  ClipPlanes.ui
)

QT4_WRAP_UI( UIS_HDRS ${UIS} )

# Make the moc files.
QT4_WRAP_CPP( MOC_SRCS ${MOC_HDRS} )

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "QtClipPlanes" )
SET ( SOURCES ${SOURCES} ${MOC_SRCS} ${UIS_HDRS} )
SET ( COMPILE_GUARD "_COMPILING_CLIP_PLANES" )
SET ( CADKIT_LIBRARIES Usul OsgTools )
SET ( OTHER_LIBRARIES ${QT_LIBRARIES}
					  ${OPENTHREADS_LIB}
					  ${OSG_LIB} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
