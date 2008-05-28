
PROJECT(HeliosQtViewsOSG)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${OSG_INC_DIR} 
                     ${CADKIT_INC_DIR} 
                     "${CADKIT_INC_DIR}/Experimental"
                     "${PROJECT_SOURCE_DIR}/../../../" 
                     ${QT_INCLUDE_DIR} )


# List the Sources.
SET (SOURCES
    Viewer.cpp
    Format.cpp
    Delegate.cpp
    EditBackground.cpp
)

# These need to be moc'd.
SET(MOC_HDRS
  Viewer.h
  EditBackground.h
)

# Create the moc files.
QT4_WRAP_CPP(MOC_SRCS ${MOC_HDRS})

# Create a Shared Library
ADD_LIBRARY(HeliosQtViewsOSG SHARED ${SOURCES} ${MOC_SRCS} )

# Link the Library	
LINK_CADKIT( HeliosQtViewsOSG Usul OsgTools QtTools )

TARGET_LINK_LIBRARIES( HeliosQtViewsOSG
   ${QT_LIBRARIES}
   ${QT_QTOPENGL_LIBRARY}
   ${OSG_LIBRARY}
)

INSTALL(TARGETS HeliosQtViewsOSG
  LIBRARY DESTINATION bin
)
