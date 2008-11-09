

PROJECT(HeliosQtCommands)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} "${PROJECT_SOURCE_DIR}/../../../" ${QT_INCLUDE_DIR} )

# Add definitions
ADD_DEFINITIONS ("-D_COMPILING_HELIOS_QT_COMMANDS")

# List all the headers.
SET ( HEADERS
	./About.h
	./Command.h
	./ExitApplication.h
	./Export.h
	./ExportImage.h
	./ExportModel.h
	./ExportScene.h
	./Group.h
	./InsertDocument.h
	./NewDocument.h
	./OpenDocument.h
	./OpenManual.h
	./SaveAsDocument.h
	./SaveDocument.h
	./ToggleView.h
	./WorkOffline.h
)

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
ADD_LIBRARY(HeliosQtCommands SHARED ${HEADERS} ${SOURCES} )

CADKIT_ADD_LIBRARY ( HeliosQtCommands )

# Link the Library	
LINK_CADKIT( HeliosQtCommands Usul MenuKit )

TARGET_LINK_LIBRARIES( HeliosQtCommands ${QT_LIBRARIES} )
