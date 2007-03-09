PROJECT (Animated)

set(COMPILE_GUARD "_COMPILING_Animated")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" "${HELIOS_INCLUDE_DIR}/GenericNurbs")

SET (SOURCES
	Movie.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(Animated SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (Animated
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (Animated
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(Animated
	Usul
	OsgTools
  ${OPENTHREADS_LIB}
  ${OSG_LIB}
)
