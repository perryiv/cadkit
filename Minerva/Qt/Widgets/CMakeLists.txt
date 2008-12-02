
PROJECT(MinervaQtWidgets)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

set(QT_MAX_VERSION "4.9.9")
set(QT_USE_OPENGL TRUE)

FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})

# the next line sets up include and link directories and defines some variables that we will use.
# you can modify the behavior by setting some variables, e.g.
INCLUDE(${QT_USE_FILE})

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${OSG_INC_DIR} 
                     ${CADKIT_INC_DIR} 
                     ${QT_INCLUDE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR} )


# Add definitions
ADD_DEFINITIONS ("-D_COMPILING_MINERVA_QT_WIDGETS")

# List the Sources.
SET (SOURCES
./WmsLayerItem.cpp
./OptionsDialog.cpp
./AddWmsLayerWidget.cpp
./AlphasDialog.cpp
)

# These need to be moc'd.
SET(MOC_HDRS
  AddWmsLayerWidget.h
  AlphasDialog.h
  OptionsDialog.h
)

SET(UIS
  ./WmsWidget.ui
  ./OptionsDialog.ui
  ./AlphasDialog.ui
)

QT4_WRAP_UI(UIS_HDRS ${UIS})

# Create the moc files.
QT4_WRAP_CPP(MOC_SRCS ${MOC_HDRS} ${UIS_HDRS} )

# Create a Shared Library
ADD_LIBRARY( MinervaQtWidgets SHARED ${SOURCES} ${MOC_SRCS} )
CADKIT_ADD_LIBRARY ( MinervaQtWidgets )

# Link the Library	
LINK_CADKIT( MinervaQtWidgets Usul Minerva QtTools )

TARGET_LINK_LIBRARIES( MinervaQtWidgets
   ${QT_LIBRARIES}
)

CADKIT_COPY( MinervaQtWidgets "${CMAKE_CURRENT_BINARY_DIR}/ui_WmsWidget.h" "${PROJECT_SOURCE_DIR}/ui_WmsWidget.h" )
CADKIT_COPY( MinervaQtWidgets "${CMAKE_CURRENT_BINARY_DIR}/ui_OptionsDialog.h" "${PROJECT_SOURCE_DIR}/ui_OptionsDialog.h" )
CADKIT_COPY( MinervaQtWidgets "${CMAKE_CURRENT_BINARY_DIR}/ui_AlphasDialog.h" "${PROJECT_SOURCE_DIR}/ui_AlphasDialog.h" )