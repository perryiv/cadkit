project (ComputeNormals)

SET(PLUGIN_NAME "ProjectionManager")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

set(COMPILE_GUARD "_COMPILING_PROJECTION_MANAGER")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Find Ossim Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSSIM.cmake)
INCLUDE(${CMakeModules}/FindOSG.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" ${OSSIM_INC_DIR} ${OT_INC_DIR} )

set (SOURCES
    ProjectionManagerComponent.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN()

# Link the Library
LINK_CADKIT( ${PLUGIN_NAME} Usul )	

TARGET_LINK_LIBRARIES( ${PLUGIN_NAME}
	${OSSIM_LIB}
)
