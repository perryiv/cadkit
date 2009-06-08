project (SceneStatsOSG)

SET(PLUGIN_NAME "SceneStatsOSG")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

# Set the compile guard.
set(COMPILE_GUARD "_COMPILING_OSG_SCENE_STATISTICS")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" )

set (SOURCES
	SceneStatsOSGFactory.cpp
	SceneStatsOSGComponent.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN()

# Link the Library with Cadkit libraries.
LINK_CADKIT( ${PLUGIN_NAME} Usul )

# Link the Library	
TARGET_LINK_LIBRARIES( ${PLUGIN_NAME}
	${OPENTHREADS_LIB}
	${OSG_LIB}
)

