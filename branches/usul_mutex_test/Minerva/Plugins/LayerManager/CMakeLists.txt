project (LayerManager)

SET(PLUGIN_NAME "LayerManager")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

set(COMPILE_GUARD "_COMPILING_LAYER_MANAGER")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

set(QT_MAX_VERSION "4.9.9")
set(QT_USE_OPENGL TRUE)

FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})

# the next line sets up include and link directories and defines some variables that we will use.
# you can modify the behavior by setting some variables, e.g.
INCLUDE(${QT_USE_FILE})

# ------------ Find Ossim Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSSIM.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${CADKIT_INC_DIR}/Experimental"
                     ${OSG_INC_DIR} 
                     "${PROJECT_SOURCE_DIR}/../" 
                     ${OSSIM_INC_DIR} 
                     ${CMAKE_CURRENT_BINARY_DIR} )

set (SOURCES
    LayerManagerComponent.cpp
    Favorites.cpp
    LayersTree.cpp
)

SET(MOC_HDRS
  Favorites.h
  LayersTree.h
)

# Make .h from .ui
SET(UIS Favorites.ui)

QT4_WRAP_UI( UIS_HDRS ${UIS} )

# Make the moc files.
QT4_WRAP_CPP( MOC_SRCS ${MOC_HDRS} ${UIS_HDRS} )

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} ${MOC_SRCS} ${UIS_HDRS} )

CADKIT_PLUGIN()

# Link the Library
LINK_CADKIT( ${PLUGIN_NAME} Usul Minerva QtTools )	

TARGET_LINK_LIBRARIES( ${PLUGIN_NAME}
	${OSSIM_LIB}
)
