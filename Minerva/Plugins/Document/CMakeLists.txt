project (MinervaModel)

SET(PLUGIN_NAME "MinervaModel")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

# Find dependencies
INCLUDE(${CMakeModules}/FindOSG.cmake)
INCLUDE(${CMakeModules}/FindOSSIM.cmake)

set(COMPILE_GUARD "_COMPILING_MINERVA_MODEL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")


# ------------ Set Include Folders ----------------------

INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${CADKIT_INC_DIR}/Experimental"
                     "${PROJECT_SOURCE_DIR}/../"
                     ${OSG_INC_DIR}
                     ${OSSIM_INC_DIR}
                     ${OSSIM_PLANET_INC_DIR} )

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

set (SOURCES
./MinervaComponent.cpp
./MinervaFactory.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN()

# Link the Library
LINK_CADKIT( ${PLUGIN_NAME} Usul MinervaDocument )

