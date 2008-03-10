
PROJECT(HeliosQtCore)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_COMPILING_OSG_QT_CORE")


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${CADKIT_INC_DIR}" "${CADKIT_INC_DIR}/Experimental" ${QT_INCLUDE_DIR} )

#List the Sources
SET (SOURCES
    Application.cpp
    DocumentProxy.cpp
    MainWindow.cpp
    Program.cpp
    ProgressBarDock.cpp
    SplashScreen.cpp
)

SET(MOC_HDRS
  MainWindow.h
  ProgressBarDock.h
)

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(MOC_SRCS ${MOC_HDRS})

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# Create a Shared Library
ADD_LIBRARY(HeliosQtCore SHARED ${SOURCES} ${MOC_SRCS} )

# Link the Library	
LINK_CADKIT( HeliosQtCore HeliosQtCommands HeliosQtTools XmlTree MenuKit QtTools
  Usul
)

TARGET_LINK_LIBRARIES( HeliosQtCore
   ${QT_LIBRARIES}
)

SET( IconDir ${EXECUTABLE_OUTPUT_PATH}/icons )
FILE(GLOB PngFiles "${CMAKE_CURRENT_SOURCE_DIR}/../../Images/*.png" )
FILE(GLOB JpgFiles "${CMAKE_CURRENT_SOURCE_DIR}/../../Images/*.jpg" )

SET ( IconFiles ${PngFiles} ${JpgFiles} )

#WRITE_FILE(Helios.log "${CMAKE_CURRENT_SOURCE_DIR}")

MAKE_DIRECTORY( ${IconDir} )
FOREACH(FILE ${IconFiles})
#WRITE_FILE(Helios.log "${FILE}" APPEND)
ADD_CUSTOM_COMMAND(
        SOURCE HeliosQtCore
        COMMAND ${CCOMMAND_COMMAND}
        ARGS cp ${FILE}  ${IconDir}
        TARGET HeliosQtCore )
ENDFOREACH(FILE ${IconFiles})