project (QtClipPlanes)

# Set the name.  Needed for the macros to set up plugin properties.
SET(PLUGIN_NAME "QtClipPlanes")

SET(CMakeModules "${CADKIT_INC_DIR}/CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

ADD_DEFINITIONS ("-D_COMPILING_CLIP_PLANES")

# Adds includes and libraries for QT.
INCLUDE(${QT_USE_FILE})

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${CADKIT_INC_DIR}/Experimental"
                     "${OSG_INC_DIR}" 
                     "${PROJECT_SOURCE_DIR}/../" 
                      ${QT_INCLUDE_DIR}
                      ${CMAKE_CURRENT_BINARY_DIR} )


SET ( SOURCES
  ./ClipPlanesComponent.cpp
  ./ClipPlanes.cpp
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
QT4_WRAP_CPP( MOC_SRCS ${MOC_HDRS} ${UIS_HDRS} )

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} ${MOC_SRCS} ${UIS_HDRS} )

CADKIT_PLUGIN()

# Link the Library with Cadkit Libraries.
LINK_CADKIT( ${PLUGIN_NAME} Usul OsgTools )


# Link the Library	
TARGET_LINK_LIBRARIES( ${PLUGIN_NAME}
	${QT_LIBRARIES}
	${OPENTHREADS_LIB}
	${OSG_LIB}
)
