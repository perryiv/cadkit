project ( Planet )

SET(PLUGIN_NAME "Planet")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

INCLUDE(${CMakeModules}/FindOSSIM.cmake)

set(COMPILE_GUARD "_COMPILING_PLANET")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( 
                     ${CADKIT_INC_DIR} 
                     "${PROJECT_SOURCE_DIR}/../"
                     ${OSSIM_INC_DIR}
                     ${OSSIM_PLANET_INC_DIR} )

set (SOURCES
./PlanetComponent.cpp
./PlanetFactory.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN()

# Link the Library
LINK_CADKIT( ${PLUGIN_NAME} Usul )

TARGET_LINK_LIBRARIES( ${PLUGIN_NAME}
  ${OSSIM_LIB}
  ${OSSIM_PLANET_LIB}
)

