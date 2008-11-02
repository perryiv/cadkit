project (QtPasswordPrompt)

# Set the name.  Needed for the macros to set up plugin properties.
SET(PLUGIN_NAME "QtPasswordPrompt")

SET(CMakeModules "${CADKIT_INC_DIR}/CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

ADD_DEFINITIONS ("-D_COMPILING_PASSWORD_PROMPT")

# Adds includes and libraries for QT.
INCLUDE(${QT_USE_FILE})

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
  ${CADKIT_INC_DIR} 
  "${PROJECT_SOURCE_DIR}/../" 
  ${QT_INCLUDE_DIR} )


set (SOURCES
  PasswordPromptComponent.cpp
)

# These need to be moc'd.
SET(MOC_HDRS
  PasswordPromptComponent.h
)

# Create the moc files.
QT4_WRAP_CPP( MOC_SRCS ${MOC_HDRS} )

# ---------- Create a Shared Library
ADD_LIBRARY(QtPasswordPrompt SHARED ${SOURCES} ${MOC_SRCS} )


CADKIT_PLUGIN()

# Link the Library with Cadkit Libraries.
LINK_CADKIT( ${PLUGIN_NAME} Usul )


# Link the Library	
TARGET_LINK_LIBRARIES( ${PLUGIN_NAME}
  ${QT_LIBRARIES}
)
