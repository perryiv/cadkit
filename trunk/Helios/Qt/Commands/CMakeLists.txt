

PROJECT(HeliosQtCommands)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

INCLUDE ( ${CMakeModules}/FindBoost.cmake)
# Note that the Boost Include path will automatically get added

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../../../" ${QT_INCLUDE_DIR} )

# Add definitions
ADD_DEFINITIONS ("-D_COMPILING_HELIOS_QT_COMMANDS")

#List the Sources
SET (SOURCES
	About.cpp
    Command.cpp
    Group.cpp
    ExitApplication.cpp
    ExportImage.cpp
    ExportModel.cpp
    ExportScene.cpp
    InsertDocument.cpp
    NewDocument.cpp
    OpenDocument.cpp
    OpenManual.cpp
    SaveAsDocument.cpp
    SaveDocument.cpp
    ToggleView.cpp
	WorkOffline.cpp
)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# Create a Shared Library
ADD_LIBRARY(HeliosQtCommands SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( HeliosQtCommands Usul MenuKit )

TARGET_LINK_LIBRARIES( HeliosQtCommands ${QT_LIBRARIES} )

INSTALL(TARGETS HeliosQtCommands
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin
  ARCHIVE DESTINATION bin
)
