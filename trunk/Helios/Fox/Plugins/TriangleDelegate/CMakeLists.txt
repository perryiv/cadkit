project (FoxTriangleDelegate)

set(COMPILE_GUARD "_COMPILING_TRIANGLE_DELEGATE")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" "${FOXTOOLKIT_INCLUDE}" "${GENERIC_NURBS_INCLUDE_DIR}" )

set (SOURCES
	TriangleDelegateFactory.cpp
	TriangleDelegateComponent.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(FoxTriangleDelegate SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxTriangleDelegate
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxTriangleDelegate
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(FoxTriangleDelegate 
  Usul
  FoxViews
  FoxTools
  OsgTools
  ${FOXTOOLKIT_LIB}
  ${OSG_LIB}
  ${OSG_TEXT_LIB}
)

