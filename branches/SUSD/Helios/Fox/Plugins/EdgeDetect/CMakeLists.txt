project (FoxEdgeDetect)

ADD_DEFINITIONS ("-D_COMPILING_EDGE_DETECT")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" "${FOXTOOLKIT_INCLUDE}" "${GENERIC_NURBS_INCLUDE_DIR}" )

set (SOURCES
  EdgeDetectComponent.cpp
  EdgeDetectFactory.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(FoxEdgeDetect SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxEdgeDetect
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxEdgeDetect
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(FoxEdgeDetect 
  Usul 
  FoxTools
  ${FOXTOOLKIT_LIB}
  OsgTools
  ${OPENTHREADS_LIB}
	${OSG_LIB}
)

