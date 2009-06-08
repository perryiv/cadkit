project (QtCustomGeometry)

INCLUDE ( Cadkit.plugin )

# Add include paths and librares for QT.
INCLUDE(${QT_USE_FILE})

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${PROJECT_SOURCE_DIR}/../" 
                     ${OSG_INC_DIR}
                     ${QT_INCLUDE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR} )

set (SOURCES
  AddGeometryWidget.cpp
  QtCustomGeometryComponent.cpp
)

SET(MOC_HDRS
  AddGeometryWidget.h
)

# Make .h from .ui
SET ( UIS 
	./AddGeometryWidget.ui
)

QT4_WRAP_UI( UIS_HDRS ${UIS} )

# Make the moc files.
QT4_WRAP_CPP( MOC_SRCS ${MOC_HDRS} )

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "QtCustomGeometry" )
SET ( SOURCES ${SOURCES} ${MOC_SRCS} ${UIS_HDRS} ${UIS} )
SET ( COMPILE_GUARD "_COMPILING_QT_CUSTOM_GEOMETRY" )
SET ( CADKIT_LIBRARIES Usul QtTools OsgTools Minerva )
SET ( OTHER_LIBRARIES ${Boost_LIBRARIES} ${QT_LIBRARIES} ${OSG_LIB} ${OPENTHREADS_LIB} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
