
#########################################################
#
#  CMakeLists for Volume Model.
#
#########################################################

project (VolumeModel)

INCLUDE ( Cadkit.plugin )

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE ( FindOSG )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" ${OSG_INC_DIR} )

set (SOURCES
./VolumeFactory.cpp
./VolumeComponent.cpp
./ImageReaderWriter.cpp
./RawReaderWriter.cpp
./VolumeDocument.cpp
)

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "VolumeModel" )
SET ( COMPILE_GUARD "_COMPILING_VOLUME_MODEL" )
SET ( CADKIT_LIBRARIES Usul OsgTools XmlTree )
SET ( OTHER_LIBRARIES ${OSG_LIB} ${OSG_DB_LIB} ${OPENTHREADS_LIB} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
