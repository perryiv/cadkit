
#########################################################
#
#  CMakeLists for GDALReadImage.
#
#########################################################

PROJECT (GDALReadImage)

INCLUDE ( Cadkit.plugin )

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${PROJECT_SOURCE_DIR}/../"
                     ${OSG_INC_DIR}
                     ${GDAL_INCLUDE_DIR} )

SET (SOURCES
	./GDALReadImageComponent.cpp
	./GDALReadImageComponent.h
)

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "GDALReadImage" )
SET ( COMPILE_GUARD "_COMPILING_GDAL_READ_IMAGE" )
SET ( CADKIT_LIBRARIES Usul )
SET ( OTHER_LIBRARIES ${GDAL_LIBRARY} ${OSG_LIB} ${OPENTHREADS_LIB} )

# Add the plugin.
CADKIT_ADD_PLUGIN( ${PLUGIN_NAME} )
