

PROJECT(HeliosQtCommands)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

INCLUDE ( ${CMakeModules}/FindBoost.cmake)
# Note that the Boost Include path will automatically get added

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_COMPILING_OSG_QT_CORE")


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../../../" ${QT_INCLUDE_DIR} )

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
)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# Create a Shared Library
ADD_LIBRARY(HeliosQtCommands SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( HeliosQtCommands Usul)

TARGET_LINK_LIBRARIES( HeliosQtCommands ${QT_LIBRARIES} )

INSTALL(TARGETS HeliosQtCommands
  LIBRARY DESTINATION bin
)
