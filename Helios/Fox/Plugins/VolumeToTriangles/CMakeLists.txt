project (FoxVolumeToTriangles)

# This project Depends on the VTKTools Project in the Experimental Branch

set(COMPILE_GUARD "_COMPILING__COMPILING_VOLUMETOTRIANGLES")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" "${FOX_INCLUDE}" "${GENERIC_NURBS_INCLUDE_DIR}" )

set (SOURCES
	VolumeToTrianglesFactory.cpp
	VolumeToTrianglesComponent.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(FoxVolumeToTriangles SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxVolumeToTriangles
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxVolumeToTriangles
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(FoxVolumeToTriangles 
  Usul
  FoxViews
  FoxTools
  ${FOXTOOLKIT_LIB}
)

