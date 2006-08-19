project (SceneStatsOSG)

set(COMPILE_GUARD "_COMPILING_OSG_SCENE_STATISTICS")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" )

set (SOURCES
	SceneStatsOSGFactory.cpp
	SceneStatsOSGComponent.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(SceneStatsOSG SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (SceneStatsOSG
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (SceneStatsOSG
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(SceneStatsOSG 
  Usul
	${OPENTHREADS_LIB}
	${OSG_LIB}
)

