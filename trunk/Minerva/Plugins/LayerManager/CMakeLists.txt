project (LayerManager)

SET(PLUGIN_NAME "LayerManager")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake )

set(COMPILE_GUARD "_COMPILING_LAYER_MANAGER")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

ADD_DEFINITIONS(${QT_DEFINITIONS})

# the next line sets up include and link directories and defines some variables that we will use.
# you can modify the behavior by setting some variables, e.g.
INCLUDE(${QT_USE_FILE})

# ------------ Find curl Library ----------------------
INCLUDE( ${CMakeModules}/FindCURL.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} ${Boost_LIBRARY_DIRS} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR}
					 ${CADKIT_INC_DIR} 
                     "${CADKIT_INC_DIR}/Experimental"
                     ${OSG_INC_DIR} 
                     "${PROJECT_SOURCE_DIR}/../" 
                     ${CMAKE_CURRENT_BINARY_DIR}
					 ${CURL_INCLUDE_DIR} )

					 
IF(MSVC)
	ADD_DEFINITIONS("-DCURL_STATICLIB")
ENDIF(MSVC)

# Add the source files.
set (SOURCES
    LayerManagerComponent.cpp
    Favorites.cpp
    FlyTo.cpp
    LayersTree.cpp
)

SET(MOC_HDRS
  Favorites.h
  FlyTo.h
  LayersTree.h
)

# Make .h from .ui
SET(UIS Favorites.ui)

QT4_WRAP_UI( UIS_HDRS ${UIS} )

# Make the moc files.
QT4_WRAP_CPP( MOC_SRCS ${MOC_HDRS} ${UIS_HDRS} )

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} ${MOC_SRCS} ${UIS_HDRS} )

CADKIT_PLUGIN( ${PLUGIN_NAME} )

# Link the Library
LINK_CADKIT( ${PLUGIN_NAME} Usul Minerva QtTools MenuKit HeliosQtViewsOSG )	

TARGET_LINK_LIBRARIES( ${PLUGIN_NAME}
	${QT_LIBRARIES}
	${QT_QTOPENGL_LIBRARY}
	${OSG_LIB}
	${CURL_LIBRARY}
)

# If we are building on Windows, link to these libraries.
IF(WIN32)
	TARGET_LINK_LIBRARIES( ${PLUGIN_NAME} Ws2_32 wldap32 )
ENDIF(WIN32)
