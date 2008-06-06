project (QtSnapShot)

# Set the name.  Needed for the macros to set up plugin properties.
SET(PLUGIN_NAME "QtSnapShot")

SET(CMakeModules "${CADKIT_INC_DIR}/CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

ADD_DEFINITIONS ("-D_COMPILING_SNAP_SHOT")



# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${OSG_INC_DIR}" 
                     "${PROJECT_SOURCE_DIR}/../" 
                      ${QT_INCLUDE_DIR}
                      ${CMAKE_CURRENT_BINARY_DIR} )


SET (SOURCES
./SnapShotComponent.cpp
./SnapShotFactory.cpp
./SnapShotWidget.cpp
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
QT4_WRAP_CPP( MOC_SRCS ${MOC_HDRS} ${UIS_HDRS} )

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} ${MOC_SRCS} ${UIS_HDRS} )

CADKIT_PLUGIN()

# Link the Library with Cadkit Libraries.
LINK_CADKIT( ${PLUGIN_NAME} Usul OsgTools HeliosQtViewsOSG )


# Link the Library	
TARGET_LINK_LIBRARIES( ${PLUGIN_NAME}
  ${OPENTHREADS_LIB}
	${OSG_LIB}
)

INSTALL(TARGETS ${PLUGIN_NAME}
  LIBRARY DESTINATION bin
)
