project (FoxConvolve)

ADD_DEFINITIONS ("-D_COMPILING_CONVOLVE")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" "${FOXTOOLKIT_INCLUDE}" "${GENERIC_NURBS_INCLUDE_DIR}" )

set (SOURCES
  ConvolveComponent.cpp
  ConvolveFactory.cpp

)

# ---------- Create a Shared Library
ADD_LIBRARY(FoxConvolve SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxConvolve
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxConvolve
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(FoxConvolve 
  Usul 
  FoxTools
  OsgTools
  ${FOXTOOLKIT_LIB}
)

