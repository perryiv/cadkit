
PROJECT(Magrathea)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)


# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)


# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSG.cmake)


# ------------ Find Ossim Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSSIM.cmake)


LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR} 
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     ${WMS_INC_DIR}
		     ${OSSIM_INC_DIR}
		     ${OSG_EPHEMERIS_INC_DIR}
		     ${OSSIM_PLANET_INC_DIR}
		     )


#List the Sources
SET (SOURCES
  LatLongGrid.cpp
  LayerManipulator.cpp
  Manipulator.cpp
  Planet.cpp
  Navigator.cpp
)

# Create a Shared Library
ADD_LIBRARY(Magrathea SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT(Magrathea Usul OsgTools)
TARGET_LINK_LIBRARIES(Magrathea
	${OPENGL_LIBRARIES}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSG_GA_LIB}
	${OSG_TEXT_LIB}
	${OSG_UTIL_LIB}
	${WMS_LIB}
	${OSSIM_LIB}
	${OSSIM_PLANET_LIB}
)

    
    
