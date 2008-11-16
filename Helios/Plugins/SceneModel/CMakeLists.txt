
#########################################################
#
#  CMakeLists for Scene Model.
#
#########################################################

project (SceneModel)

INCLUDE ( Cadkit.plugin )

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE ( FindOSG )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" ${OSG_INC_DIR} )

# Set the sources to compile.
SET ( SOURCES
	SceneComponent.cpp
	SceneDocument.cpp
)

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "SceneModel" )
SET ( COMPILE_GUARD "_COMPILING_SCENE_MODEL" )
SET ( CADKIT_LIBRARIES Usul OsgTools )
SET ( OTHER_LIBRARIES ${OPENTHREADS_LIB} ${OSG_LIB}	${OSG_DB_LIB} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
