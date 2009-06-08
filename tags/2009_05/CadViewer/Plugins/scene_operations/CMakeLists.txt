
PROJECT (SceneOperations)

INCLUDE ( Cadkit.plugin )

INCLUDE ( FindOSG )

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${PROJECT_SOURCE_DIR}/../" 
                     ${OSG_INC_DIR} )

SET (SOURCES
./UserDataState.cpp
./SceneOperationsComponent.cpp
./UserDataBase.cpp
./UserDataMap.cpp
./Factory.cpp
./UserDataSelected.cpp
)

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "SceneOperations" )
SET ( COMPILE_GUARD "_COMPILING_SCENE_OPERATIONS" )
SET ( CADKIT_LIBRARIES Usul )
SET ( OTHER_LIBRARIES ${OSG_LIB} ${OPENTHREADS_LIB} )

# Add the plugin.
CADKIT_ADD_PLUGIN( ${PLUGIN_NAME} )
