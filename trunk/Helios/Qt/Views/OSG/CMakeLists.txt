
PROJECT(HeliosQtViewsOSG)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${OSG_INC_DIR} ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../../../" ${QT_INCLUDE_DIR} )


# List the Sources.
SET (SOURCES
    Viewer.cpp
    Format.cpp
)


# Create a Shared Library
ADD_LIBRARY(HeliosQtViewsOSG SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( HeliosQtViewsOSG Usul OsgTools )

TARGET_LINK_LIBRARIES( HeliosQtViewsOSG
   ${QT_LIBRARIES}
   ${QT_QTOPENGL_LIBRARY}
   ${OSG_LIBRARY}
)
