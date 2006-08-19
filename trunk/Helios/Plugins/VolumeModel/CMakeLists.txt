project (VolumeModel)

set(COMPILE_GUARD "_COMPILING_VOLUME_MODEL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" )

set (SOURCES
	VolumeFactory.cpp
	VolumeDocument.cpp
	VolumeComponent.cpp
	VolumeCallback.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(VolumeModel SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (VolumeModel
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (VolumeModel
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(VolumeModel 
  Usul
  OsgTools
  ${OPENTHREADS_LIB}
	${OSG_LIB}
)

