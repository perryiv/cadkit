
PROJECT(Minerva)


# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE($ENV{USUL_INC_DIR}/Usul/Make/FindOSG.cmake)


# ------------ Find Ossim Libraries ----------------------
INCLUDE($ENV{USUL_INC_DIR}/Usul/Make/FindOSSIM.cmake)

# ------------ Find boost ----------------------
SET ( Boost_DATE_TIME_LIBRARY TRUE )
SET ( Boost_USE_SERIALIZATION TRUE )
INCLUDE( $ENV{USUL_INC_DIR}/Usul/Make/FindBoost.cmake)

# ------------ Find PQXX Library ----------------------
FIND_LIBRARY(PQXX_LIB pqxx $ENV{PQXX_LIB_DIR} )

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
		     $ENV{USUL_INC_DIR}
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     ${WMS_INC_DIR}
		     ${OSSIM_INC_DIR}
		     ${OSG_EPHEMERIS_INC_DIR}
		     ${OSSIM_PLANET_INC_DIR}
		     $ENV{PQXX_INC_DIR}
		     )


#List the Sources
SET (SOURCES
  ./GUI/Controller.cpp
  ./Scene/SceneManager.cpp
  ./Functors/GradientColorFunctor.cpp
  ./Functors/SingleColorFunctor.cpp
  ./Layers/RLayer.cpp
  ./Layers/PointTimeLayer.cpp
  ./Layers/LineLayer.cpp
  ./Layers/PolygonLayer.cpp
  ./Layers/PolygonTimeLayer.cpp
  ./Layers/PointLayer.cpp
  ./Layers/PatentLayer.cpp
  ./Layers/Layer.cpp
  ./DB/Connection.cpp
  ./DB/Info.cpp
  ./DataObjects/Polygon.cpp
  ./DataObjects/PointTime.cpp
  ./DataObjects/DataObject.cpp
  ./DataObjects/Line.cpp
  ./DataObjects/Point.cpp
  ./postGIS/Polygon.cpp
  ./postGIS/Geometry.cpp
  ./postGIS/BinaryParser.cpp
  ./postGIS/Line.cpp
  ./postGIS/Point.cpp
  ./Viz/Progress.cpp
  ./Viz/Controller.cpp
)

# Create a Shared Library
ADD_LIBRARY(Minerva SHARED ${SOURCES} )

# Link the Library	
TARGET_LINK_LIBRARIES(Minerva
	Usul
	OsgTools
	Magrathea
	${Boost_LIBRARIES}
	${PQXX_LIB}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSSIM_LIB}
	${OSSIM_PLANET_LIB}
)

    
    
