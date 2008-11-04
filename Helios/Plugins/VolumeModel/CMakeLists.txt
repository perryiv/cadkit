project (VolumeModel)

SET(PLUGIN_NAME "VolumeModel")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE ( ${CMakeModules}/FindOSG.cmake)

set(COMPILE_GUARD "_COMPILING_VOLUME_MODEL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" ${OSG_INC_DIR} )

set (SOURCES
./VolumeFactory.cpp
./VolumeComponent.cpp
./ImageReaderWriter.cpp
./RawReaderWriter.cpp
./VolumeDocument.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(VolumeModel SHARED ${SOURCES} )

CADKIT_PLUGIN( ${PLUGIN_NAME} )

# Link the Library with Cadkit libraries.
LINK_CADKIT( ${PLUGIN_NAME} Usul OsgTools XmlTree )

# Link the Library	
TARGET_LINK_LIBRARIES( ${PLUGIN_NAME} 
	${OSG_LIB}
	${OSG_DB_LIB}
)

