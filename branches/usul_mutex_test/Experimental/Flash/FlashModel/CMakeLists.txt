project (FlashModel)

# Set the name.  Needed for the macros to set up plugin properties.
SET(PLUGIN_NAME "FlashModel")

SET(CMakeModules "$ENV{CADKIT_INC_DIR}/CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE ( ${CMakeModules}/FindOSG.cmake)

# Set the compile guard.
set(COMPILE_GUARD "_COMPILING_FLASH_MODEL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

FIND_PATH(HDF_INCLUDE_DIR hdf5.h )

# Look for the library.
FIND_LIBRARY(HDF_LIBRARY NAMES hdf5 )

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${CADKIT_INC_DIR}/Experimental" "${PROJECT_SOURCE_DIR}/../" ${OSG_INC_DIR} ${HDF_INCLUDE_DIR} )

# Set the sources to compile.
set (SOURCES
./Dataset.cpp
./FlashComponent.cpp
./FlashDocument.cpp
./H5File.cpp
./Timestep.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN()

# Link the Library with Cadkit Libraries.
LINK_CADKIT( ${PLUGIN_NAME} Usul OsgTools XmlTree SerializeXML  )

# Link the Library	
TARGET_LINK_LIBRARIES( ${PLUGIN_NAME} 
  ${OPENTHREADS_LIB}
	${OSG_LIB}
  ${HDF_LIBRARY}
)
