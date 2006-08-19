project (FoxSceneDelegate)

ADD_DEFINITIONS ("-D_COMPILING_SCENE_DELEGATE")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" "${FOX_INCLUDE}" "${GENERIC_NURBS_INCLUDE_DIR}" )

set (SOURCES
	SceneDelegateFactory.cpp
	SceneDelegateComponent.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(FoxSceneDelegate SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxSceneDelegate
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxSceneDelegate
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(FoxSceneDelegate 
  Usul 
  FoxTools
  FoxViews
  OsgTools
  ${FOXTOOLKIT_LIB}
)

