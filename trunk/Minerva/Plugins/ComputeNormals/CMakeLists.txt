project (ComputeNormals)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

set(COMPILE_GUARD "_COMPILING_COMPUTE_NORMALS")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" )

set (SOURCES
    ComputeNormalsFactory.cpp
    ComputeNormalsComponent.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(ComputeNormals SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated library --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (ComputeNormals
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (ComputeNormals
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library
LINK_CADKIT( ComputeNormals Usul )	


