project (PointModel)

# Set the name.  Needed for the macros to set up plugin properties.
SET(PLUGIN_NAME "PointModel")

SET(CMakeModules "${CADKIT_INC_DIR}/CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE ( ${CMakeModules}/FindOSG.cmake)

# Set the compile guard.
set(COMPILE_GUARD "_COMPILING_POINT_MODEL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${CADKIT_INC_DIR}/Experimental" "${PROJECT_SOURCE_DIR}/../" ${OSG_INC_DIR} )

# Set the sources to compile.
set (SOURCES
PointComponent.cpp
PointDocument.cpp
PointFactory.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(PointModel SHARED ${SOURCES} )

# Create a plugin.
CADKIT_PLUGIN()

# Link the Library with Cadkit libraries.
LINK_CADKIT( ${PLUGIN_NAME} Usul OsgTools )

# Link the Library	
TARGET_LINK_LIBRARIES( 
  ${PLUGIN_NAME}
  ${OPENTHREADS_LIB}
  ${OSG_LIB}
)

