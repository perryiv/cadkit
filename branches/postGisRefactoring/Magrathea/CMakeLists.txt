
PROJECT(Magrathea)


# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)


# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE($ENV{USUL_INC_DIR}/Usul/Make/FindOSG.cmake)


# ------------ Find Ossim Libraries ----------------------
INCLUDE($ENV{USUL_INC_DIR}/Usul/Make/FindOSSIM.cmake)


#setup output Directories
SET (LIBRARY_OUTPUT_PATH
	$ENV{USUL_BIN_DIR}
	CACHE PATH
	"Single Directory for all Libraries"
)

#setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
	$ENV{USUL_BIN_DIR}
	CACHE PATH
	"Single Directory for all Executables."
)

# ------------ Find the Usul Library -----------------------------
if (NOT USUL_BIN_DIR)
  set(USUL_BIN_DIR "$ENV{USUL_BIN_DIR}")
  if (NOT USUL_BIN_DIR)  
    SET (USUL_BIN_DIR ${LIBRARY_OUTPUT_PATH} )
    endif (NOT USUL_BIN_DIR)
endif( NOT USUL_BIN_DIR )


LINK_DIRECTORIES( ${USUL_BIN_DIR} )


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     "$ENV{USUL_INC_DIR}" 
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
)

# Create a Shared Library
ADD_LIBRARY(Magrathea SHARED ${SOURCES} )

# Link the Library	
TARGET_LINK_LIBRARIES(Magrathea
	Usul
	OsgTools
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

    
    
