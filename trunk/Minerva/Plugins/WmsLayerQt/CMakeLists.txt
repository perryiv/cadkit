project (WmsLayerQt)

SET(PLUGIN_NAME "WmsLayerQt")

SET( CMakeModules "$ENV{CADKIT_INC_DIR}/CMakeModules" )
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

SET(COMPILE_GUARD "_COMPILING_WMS_LAYER_QT")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")


set(QT_MAX_VERSION "4.9.9")
set(QT_USE_OPENGL TRUE)

FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})

# the next line sets up include and link directories and defines some variables that we will use.
# you can modify the behavior by setting some variables, e.g.
INCLUDE(${QT_USE_FILE})


LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${CADKIT_INC_DIR}/Experimental"
                     "${PROJECT_SOURCE_DIR}/../" 
                     ${OSG_INC_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR} )

SET (SOURCES
  AddWmsLayerWidget.cpp
  AlphasDialog.cpp
  EditWmsLayerWidget.cpp
  OptionsDialog.cpp
  OptionWidget.cpp
  WmsLayerItem.cpp
  WmsLayerQtFactory.cpp
  WmsLayerQtComponent.cpp
)


SET(MOC_HDRS
  AddWmsLayerWidget.h
  AlphasDialog.h
  EditWmsLayerWidget.h
  OptionsDialog.h
  OptionWidget.h
)

# Make .h from .ui
SET ( UIS 
  AlphasDialog.ui 
  WmsWidget.ui 
  EditWmsLayerWidget.ui 
  OptionsDialog.ui 
)

QT4_WRAP_UI( UIS_HDRS ${UIS} )

# Make the moc files.
QT4_WRAP_CPP( MOC_SRCS ${MOC_HDRS} ${UIS_HDRS} )

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} ${MOC_SRCS} ${UIS_HDRS} )

CADKIT_PLUGIN()

# Link the Library
LINK_CADKIT( ${PLUGIN_NAME} Usul Minerva )
