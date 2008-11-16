
#########################################################
#
#  CMakeLists for Model Presentation.
#
#########################################################

project (ModelPresentation)

INCLUDE ( Cadkit.plugin )

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE ( FindOSG )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" ${OSG_INC_DIR} )

LINK_DIRECTORIES ( ${LINK_DIRECTORIES} ${CADKIT_BIN_DIR} )

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

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "ModelPresentation" )
SET ( COMPILE_GUARD "_COMPILING_DYNAMIC_LAND_MODEL" )
SET ( CADKIT_LIBRARIES Usul OsgTools XmlTree ModelPresentationLib MenuKit )
SET ( OTHER_LIBRARIES ${OPENTHREADS_LIB} ${OSG_LIB}	${OSG_UTIL_LIB} )

IF( NOT WIN32 )
	SET ( OTHER_LIBRARIES ${OTHER_LIBRARIES} dl )
ENDIF( NOT WIN32 )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
