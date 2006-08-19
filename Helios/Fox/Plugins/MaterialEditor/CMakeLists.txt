project (FoxMaterialEditor)

ADD_DEFINITIONS ("-D_COMPILING_MATERIAL_EDITOR")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" "${FOX_INCLUDE}" "${GENERIC_NURBS_INCLUDE_DIR}" )

set (SOURCES
	 MaterialEditorFactory.cpp
	MaterialEditorComponent.cpp
	MaterialEditor.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(FoxMaterialEditor SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxMaterialEditor
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxMaterialEditor
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(FoxMaterialEditor 
  Usul 
  FoxTools
  OsgTools
  ${FOXTOOLKIT_LIB}
  ${OSG_LIB}
)

