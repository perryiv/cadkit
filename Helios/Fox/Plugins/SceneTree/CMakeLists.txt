project (FoxSceneTree)

ADD_DEFINITIONS ("-D_COMPILING_SCENE_TREE")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" "${FOX_INCLUDE}" "${GENERIC_NURBS_INCLUDE_DIR}" )

set (SOURCES
	SceneTreeFactory.cpp
	SceneTreeComponent.cpp
	SceneTree.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(FoxSceneTree SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxSceneTree
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxSceneTree
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(FoxSceneTree 
  Usul 
  FoxTools
  OsgTools
  ${FOXTOOLKIT_LIB}
)

