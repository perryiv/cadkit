project (PostGISLayerQt)

SET(PLUGIN_NAME "PostGISLayerQt")

SET(CMakeModules "${CADKIT_INC_DIR}/CMakeModules" )
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

set(COMPILE_GUARD "_COMPILING_POSTGIS_LAYER_QT")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${CADKIT_INC_DIR}/Experimental"
                     "${PROJECT_SOURCE_DIR}/../"
                     ${CMAKE_CURRENT_BINARY_DIR} 
                     ${QT_INCLUDE_DIR} 
                     ${OSG_INC_DIR}
                     $ENV{PQXX_INC_DIR}
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
QT4_WRAP_CPP( MOC_SRCS ${MOC_HDRS} ${UIS_HDRS} )


# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES}  ${MOC_SRCS} ${UIS_HDRS} )

CADKIT_PLUGIN()

# Link the Library
LINK_CADKIT( ${PLUGIN_NAME} Usul Minerva MinervaLayersGDAL )	

