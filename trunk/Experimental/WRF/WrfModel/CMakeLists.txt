project (WrfModel)

# Set the name.  Needed for the macros to set up plugin properties.
SET(PLUGIN_NAME "WrfModel")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE ( ${CMakeModules}/FindOSG.cmake)

# Set the compile guard.
set(COMPILE_GUARD "_COMPILING_WRF_MODEL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" ${OSG_INC_DIR} )

# Set the sources to compile.
set (SOURCES
./ChannelCommand.cpp
./NextTimestep.cpp
./PreviousTimestep.cpp
./ChangeNumPlanes.cpp
./WRFComponent.cpp
./WRFFactory.cpp
./Parser.cpp
./WRFDocument.cpp

)

# ---------- Create a Shared Library
ADD_LIBRARY(${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN()

# Link the Library with Cadkit Libraries.
LINK_CADKIT( ${PLUGIN_NAME} Usul OsgTools XmlTree )

# Link the Library	
TARGET_LINK_LIBRARIES( ${PLUGIN_NAME} 
  ${OPENTHREADS_LIB}
	${OSG_LIB}
)
