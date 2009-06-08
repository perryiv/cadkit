
#########################################################
#
#  CMakeLists for QtPasswordPrompt.
#
#########################################################

project (QtPasswordPrompt)

INCLUDE ( Cadkit.plugin )

# Adds includes and libraries for QT.
INCLUDE(${QT_USE_FILE})

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( 
  ${CADKIT_INC_DIR} 
  "${PROJECT_SOURCE_DIR}/../" 
  ${QT_INCLUDE_DIR} )


set (SOURCES
  PasswordPromptComponent.cpp
  PasswordPromptComponent.h
)

# These need to be moc'd.
SET(MOC_HDRS
  PasswordPromptComponent.h
)

# Create the moc files.
QT4_WRAP_CPP( MOC_SRCS ${MOC_HDRS} )

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "QtPasswordPrompt" )
SET ( SOURCES ${SOURCES} ${MOC_SRCS} )
SET ( COMPILE_GUARD "_COMPILING_PASSWORD_PROMPT" )
SET ( CADKIT_LIBRARIES Usul )
SET ( OTHER_LIBRARIES ${QT_LIBRARIES} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
