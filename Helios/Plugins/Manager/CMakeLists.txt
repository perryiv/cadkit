
PROJECT(HeliosPluginManager)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

ADD_DEFINITIONS("-DD_COMPILING_HELIOS_PLUGINS_MANAGER")

INCLUDE(${CMakeModules}/FindXerces.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} ${XERCESC_INCLUDE_DIR} )


#List the Sources
SET (SOURCES
  ./Manager.cpp
)

# Create a Shared Library
ADD_LIBRARY(HeliosPluginManager SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( HeliosPluginManager Usul XmlTree )
TARGET_LINK_LIBRARIES(HeliosPluginManager
	${XERCESC_LIBRARY}
)

    
    
