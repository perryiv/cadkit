project (VoxelModel)

# This project has lots of compiler errors of which I have no idea how to solve


set(COMPILE_GUARD "_COMPILING_VOXEL_MODEL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" )

set (SOURCES
	VoxelFactory.cpp
	VoxelDocument.cpp
	VoxelComponent.cpp
	ImageInfo.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(VoxelModel SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (VoxelModel
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (VoxelModel
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(VoxelModel 
  Usul
  OsgTools
  ${OPENTHREADS_LIB}
	${OSG_LIB}
)

