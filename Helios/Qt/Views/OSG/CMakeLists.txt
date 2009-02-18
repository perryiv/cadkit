
PROJECT(HeliosQtViewsOSG)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR}
					 ${OSG_INC_DIR} 
                     ${CADKIT_INC_DIR} 
                     "${CADKIT_INC_DIR}/Experimental"
                     "${PROJECT_SOURCE_DIR}/../../../" 
                     ${QT_INCLUDE_DIR} )

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# Add definitions
ADD_DEFINITIONS ("-D_COMPILING_HELIOS_QT_VIEWS_OSG")

IF(MSVC)
	ADD_DEFINITIONS("-DNOMINMAX")
ENDIF(MSVC)

# List the Sources.
SET (SOURCES
    Viewer.cpp
    Format.cpp
    Delegate.cpp
    EditBackground.cpp
)

# These need to be moc'd.
SET(MOC_HDRS
  Viewer.h
  EditBackground.h
)

# Create the moc files.
QT4_WRAP_CPP(MOC_SRCS ${MOC_HDRS})

# Create a Shared Library
ADD_LIBRARY(HeliosQtViewsOSG SHARED ${SOURCES} ${MOC_SRCS} )
CADKIT_ADD_LIBRARY ( HeliosQtViewsOSG )

# Link the Library	
LINK_CADKIT( HeliosQtViewsOSG Usul OsgTools QtTools MenuKit )

TARGET_LINK_LIBRARIES( HeliosQtViewsOSG
	${OPENGL_LIBRARIES}
   ${QT_LIBRARIES}
   ${QT_QTOPENGL_LIBRARY}
   ${OSG_LIB}
   ${OPENTHREADS_LIB}
)
