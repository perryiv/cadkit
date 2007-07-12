
PROJECT(HeliosQtCommands)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

INCLUDE ( ${CMakeModules}/FindBoost.cmake)
# Note that the Boost Include path will automatically get added

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_COMPILING_OSG_QT_CORE")

FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})

# the next line sets up include and link directories and defines some variables that we will use.
# you can modify the behavior by setting some variables, e.g.
INCLUDE(${QT_USE_FILE})

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../../../" ${QT_INCLUDE_DIR} )

#List the Sources
SET (SOURCES
    BaseAction.cpp
    Command.cpp
    ExitApplication.cpp
    ExportImage.cpp
    OpenDocument.cpp
)

SET(MOC_HDRS
  BaseAction.h
)

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(MOC_SRCS ${MOC_HDRS})

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# Create a Shared Library
ADD_LIBRARY(HeliosQtCommands SHARED ${SOURCES} ${MOC_SRCS} )

# Link the Library	
LINK_CADKIT( HeliosQtCommands Usul)

TARGET_LINK_LIBRARIES( HeliosQtCommands ${QT_LIBRARIES} )

