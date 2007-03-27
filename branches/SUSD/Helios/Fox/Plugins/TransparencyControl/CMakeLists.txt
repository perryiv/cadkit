project (FoxTransparancyControl)

set(COMPILE_GUARD "_COMPILING_TRANSPARENCY_CONTROL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" "${FOXTOOLKIT_INCLUDE}" "${GENERIC_NURBS_INCLUDE_DIR}" )

set (SOURCES
	TransparencyFactory.cpp
	TransparencyComponent.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(FoxTransparancyControl SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxTransparancyControl
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxTransparancyControl
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(FoxTransparancyControl 
  Usul 
  FoxTools
  ${OPENTHREADS_LIB}
 	${OSG_LIB}
  ${FOXTOOLKIT_LIB}
)

