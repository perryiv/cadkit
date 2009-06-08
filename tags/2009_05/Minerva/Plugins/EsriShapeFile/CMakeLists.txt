
#########################################################
#
#  CMakeLists for EsriShapeFile.
#
#########################################################

PROJECT (EsriShapeFile)

INCLUDE ( Cadkit.plugin )

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${PROJECT_SOURCE_DIR}/../"
                     ${OSG_INC_DIR}
                     ${GDAL_INCLUDE_DIR} )

SET (SOURCES
	./EsriShapeFileComponent.cpp
	./EsriShapeFileComponent.h
	./EsriShapeFileDocument.cpp
	./EsriShapeFileDocument.h
)

# Set vairables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "EsriShapeFile" )
SET ( COMPILE_GUARD "_COMPILING_ESRI_SHAPE_FILE" )
SET ( CADKIT_LIBRARIES Usul )
SET ( OTHER_LIBRARIES ${GDAL_LIBRARY} ${OSG_LIB} )

# Add the plugin.
CADKIT_ADD_PLUGIN( ${PLUGIN_NAME} )
