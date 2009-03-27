
#########################################################
#
#  CMakeLists for PostGIS Qt GUI.
#
#########################################################

project (PostGISLayerQt)

INCLUDE ( Cadkit.plugin)

IF(MSVC)
	ADD_DEFINITIONS("-DNOMINMAX")
ENDIF(MSVC)

INCLUDE ( ${QT_USE_FILE} )

#########################################################
#
#  Set link directories.
#
#########################################################

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} ${Boost_LIBRARY_DIRS} $ENV{PQXX_LIB_DIR} )


#########################################################
#
#  Set include directories.
#
#########################################################

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR}
					 ${CADKIT_INC_DIR} 
                     "${CADKIT_INC_DIR}/Experimental"
                     "${PROJECT_SOURCE_DIR}/../"
                     ${CMAKE_CURRENT_BINARY_DIR} 
                     ${QT_INCLUDE_DIR} 
                     ${OSG_INC_DIR}
                     )

set (SOURCES
./AddPostGISLayerWidget.cpp
./ConnectToDatabase.cpp
./DatabasePage.cpp
./GradientColorWidget.cpp
./LineWidget.cpp
./PointWidget.cpp
./PolygonWidget.cpp
./PostGISLayerQtComponent.cpp
./PropertyPage.cpp
./SingleColorWidget.cpp
./UniqueColorWidget.cpp
)

SET(MOC_HDRS
./AddPostGISLayerWidget.h
./ConnectToDatabase.h
./DatabasePage.h
./GradientColorWidget.h
./LineWidget.h
./PointWidget.h
./PolygonWidget.h
./PropertyPage.h
./SingleColorWidget.h
./UniqueColorWidget.h
)

# Make .h from .ui
SET ( UIS 
./LayerProperties.ui
./LineWidget.ui
./PointWidget.ui
./PolygonWidget.ui
)

QT4_WRAP_UI( UIS_HDRS ${UIS} )

# Make the moc files.
QT4_WRAP_CPP( MOC_SRCS ${MOC_HDRS} )

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "PostGISLayerQt" )
SET ( SOURCES ${SOURCES} ${MOC_SRCS} ${UIS_HDRS} )
SET ( COMPILE_GUARD "_COMPILING_POSTGIS_LAYER_QT" )
SET ( CADKIT_LIBRARIES Usul Minerva QtTools MinervaPostGIS MinervaPostgres MinervaGDAL )
SET ( OTHER_LIBRARIES ${QT_LIBRARIES} ${Boost_LIBRARIES} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
