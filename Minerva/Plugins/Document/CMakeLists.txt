project (MinervaModel)

SET(PLUGIN_NAME "MinervaModel")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

# Find dependencies
INCLUDE(${CMakeModules}/FindOSG.cmake)

set(COMPILE_GUARD "_COMPILING_MINERVA_MODEL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")


# ------------ Set Include Folders ----------------------

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR}
					 ${CADKIT_INC_DIR} 
                     "${CADKIT_INC_DIR}/Experimental"
                     "${PROJECT_SOURCE_DIR}/../"
                     ${OSG_INC_DIR} )

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} ${Boost_LIBRARY_DIRS} )

set (SOURCES
./MinervaComponent.cpp
./MinervaFactory.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN( ${PLUGIN_NAME} )

# Link the Library
LINK_CADKIT( ${PLUGIN_NAME} Usul MinervaDocument )

TARGET_LINK_LIBRARIES ( ${PLUGIN_NAME} ${Boost_LIBRARIES} )
