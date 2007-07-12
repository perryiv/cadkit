
PROJECT(HeliosQtTools)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_COMPILING_HELIOS_QT_TOOLS")

FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})

# the next line sets up include and link directories and defines some variables that we will use.
# you can modify the behavior by setting some variables, e.g.
INCLUDE(${QT_USE_FILE})

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../../../" ${QT_INCLUDE_DIR} )

#List the Sources
SET (SOURCES
    Image.cpp
    Move.cpp
    SettingsGroupScope.cpp
    Show.cpp
)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# Create a Shared Library
ADD_LIBRARY(HeliosQtTools SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( HeliosQtTools Usul )

TARGET_LINK_LIBRARIES( HeliosQtTools ${QT_LIBRARIES} )

