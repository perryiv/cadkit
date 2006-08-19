project (FoxSelectionSTL)

ADD_DEFINITIONS ("-D_COMPILING_SELECTION_STL")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" "${FOXTOOLKIT_INCLUDE}" "${GENERIC_NURBS_INCLUDE_DIR}" )

set (SOURCES
	SelectionObject.cpp
	Factory.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(FoxSelectionSTL SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxSelectionSTL
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxSelectionSTL
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(FoxSelectionSTL 
  Usul 
  FoxTools
 # OsgTools
  ${FOXTOOLKIT_LIB}
)

