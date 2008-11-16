
#########################################################
#
#  CMakeLists for LayerManager.
#
#########################################################

project (LayerManager)

INCLUDE ( Cadkit.plugin )

# Add include paths and librares for QT.
INCLUDE(${QT_USE_FILE})

# ------------ Find curl Library ----------------------
INCLUDE ( FindCURL )

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
QT4_WRAP_CPP( MOC_SRCS ${MOC_HDRS} )

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "LayerManager" )
SET ( SOURCES ${SOURCES} ${MOC_SRCS} ${UIS_HDRS} )
SET ( COMPILE_GUARD "_COMPILING_LAYER_MANAGER" )
SET ( CADKIT_LIBRARIES Usul Minerva QtTools MenuKit HeliosQtViewsOSG )
SET ( OTHER_LIBRARIES ${QT_LIBRARIES}
					  ${QT_QTOPENGL_LIBRARY}
					  ${OSG_LIB}
					  ${CURL_LIBRARY} )

# If we are building on Windows, link to these libraries.
IF(WIN32)
	SET( OTHER_LIBRARIES ${OTHER_LIBRARIES} Ws2_32 wldap32 )
ENDIF(WIN32)

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
