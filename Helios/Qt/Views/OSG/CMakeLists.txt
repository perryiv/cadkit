

PROJECT(HeliosQtViewsOSG)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)


FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})

# the next line sets up include and link directories and defines some variables that we will use.
# you can modify the behavior by setting some variables, e.g.
INCLUDE(${QT_USE_FILE})

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../../../" ${QT_INCLUDE_DIR} )


# List the Sources.
SET (SOURCES
    Viewer.cpp
    Format.cpp
)


# Create a Shared Library
ADD_LIBRARY(HeliosQtViewsOSG SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( HeliosQtViewsOSG Usul OsgTools )

TARGET_LINK_LIBRARIES( HeliosQViewsOSG
   ${QT_LIBRARIES}
)
