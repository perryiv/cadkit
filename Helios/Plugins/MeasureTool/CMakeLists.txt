project ( MeasureTool )

SET(PLUGIN_NAME "MeasureTool")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)


set(COMPILE_GUARD "_COMPILING_MEASURE_TOOL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} ${OSG_LIB_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" ${OSG_INC_DIR} )

set (SOURCES
./MeasureToolComponent.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN()

# Link the Library
LINK_CADKIT( ${PLUGIN_NAME} Usul )

TARGET_LINK_LIBRARIES( ${PLUGIN_NAME}
  ${OSG_LIB}
  ${OSG_UTIL_LIB}
)

INSTALL(TARGETS ${PLUGIN_NAME}
  LIBRARY DESTINATION bin
)
