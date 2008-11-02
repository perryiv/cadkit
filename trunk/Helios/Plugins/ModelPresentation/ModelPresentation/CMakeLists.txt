project (ModelPresentation)

# Set the name.  Needed for the macros to set up plugin properties.
SET(PLUGIN_NAME "ModelPresentation")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE ( ${CMakeModules}/FindOSG.cmake)

# Set the compile guard.
set(COMPILE_GUARD "_COMPILING_DYNAMIC_LAND_MODEL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" ${OSG_INC_DIR} )

LINK_DIRECTORIES ( ${LINK_DIRECTORIES} ${CADKIT_BIN_DIR} )

# Add definitions
ADD_DEFINITIONS ("-D_COMPILING_DYNAMIC_LAND_MODEL")

# Set the sources to compile.
set (SOURCES
./MpdMenuCommand.cpp
./MpdNextCommand.cpp
./MpdLocation.cpp
./MpdTools.cpp
./MpdPrevTimestep.cpp
./MpdFirstTimestep.cpp
./MpdAnimation.cpp
./MpdDynamicModel.cpp
./MpdJob.cpp
./MpdPrevSequence.cpp
./MpdNextSequence.cpp
./MpdFirstSequence.cpp
./MpdSlowerAnimationSpeed.cpp
./MpdFasterAnimationSpeed.cpp
./MpdAnimationSpeed.cpp
./ModelPresentationComponent.cpp
./ModelPresentationDocument.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN()

# Link the Library with Cadkit Libraries.
LINK_CADKIT( ${PLUGIN_NAME} Usul OsgTools XmlTree ModelPresentationLib MenuKit )

# Link the Library	
TARGET_LINK_LIBRARIES( ${PLUGIN_NAME} 
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_UTIL_LIB}
)

IF( NOT WIN32 )
	TARGET_LINK_LIBRARIES ( ${PLUGIN_NAME} dl )
ENDIF( NOT WIN32 )
