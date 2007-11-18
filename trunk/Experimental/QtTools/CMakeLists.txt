
PROJECT(QtTools)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")
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
  ColorButton.cpp
  PropertyBrowser.cpp
  TreeControl.cpp
)

# These need to be moc'd.
SET(MOC_HDRS
  ColorButton.h
  PropertyBrowser.h
  TreeControl.h
)

# Create the moc files.
QT4_WRAP_CPP(MOC_SRCS ${MOC_HDRS})

# Create a Shared Library
ADD_LIBRARY( QtTools SHARED ${SOURCES} ${MOC_SRCS} )

# Link the Library	
LINK_CADKIT( QtTools Usul )

TARGET_LINK_LIBRARIES( QtTools
   ${QT_LIBRARIES}
)
