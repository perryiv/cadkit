
project (OSG_SAL)

SET(PLUGIN_NAME "OSG_SAL")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)
INCLUDE ( ${CMakeModules}/FindOSG.cmake)

set(COMPILE_GUARD "_COMPILING_OSG_SAL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" )

set (SOURCES
./FileIO.cpp
./Factory.cpp
./Transform.cpp
./Node.cpp
./Grid.cpp
./Sphere.cpp
./Group.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN()

# Link the Library
LINK_CADKIT( ${PLUGIN_NAME} Usul OsgTools )	
TARGET_LINK_LIBRARIES( ${PLUGIN_NAME} ${OSG_LIB} )

