
project (VRVGUI)

SET(PLUGIN_NAME "VRVGUI")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)
INCLUDE ( ${CMakeModules}/FindVRJuggler.cmake)

set(COMPILE_GUARD "_COMPILING_VRV_GUI")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" )

set (SOURCES
./Menu.cpp
./Factory.cpp
./Parser.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN()

# Link the Library
LINK_CADKIT( ${PLUGIN_NAME} Usul MenuKit )	
TARGET_LINK_LIBRARIES( ${PLUGIN_NAME} ${CPPDOM_LIB} )
