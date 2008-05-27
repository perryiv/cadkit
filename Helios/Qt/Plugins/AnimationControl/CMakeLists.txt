project (QtAnimationControl)

# Set the name.  Needed for the macros to set up plugin properties.
SET(PLUGIN_NAME "QtAnimationControl")

SET(CMakeModules "${CADKIT_INC_DIR}/CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

ADD_DEFINITIONS ("-D_COMPILING_ANIMATION_CONTROL")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${CADKIT_INC_DIR}/Experimental"
                     "${PROJECT_SOURCE_DIR}/../" 
                      ${QT_INCLUDE_DIR} )


SET ( SOURCES
  ./AnimationControlComponent.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN()

# Link the Library with Cadkit Libraries.
LINK_CADKIT( ${PLUGIN_NAME} Usul QtTools )

INSTALL(TARGETS ${PLUGIN_NAME}
  LIBRARY DESTINATION bin
)
