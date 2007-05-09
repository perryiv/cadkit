
PROJECT(Minerva)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSG.cmake)


# ------------ Find Ossim Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSSIM.cmake)

# ------------ Find boost ----------------------
SET ( Boost_USE_DATE_TIME TRUE )
SET ( Boost_USE_SERIALIZATION TRUE )
INCLUDE( ${CMakeModules}/FindBoost.cmake)

# ------------ Find PQXX Library ----------------------
FIND_LIBRARY(PQXX_LIB pqxx $ENV{PQXX_LIB_DIR} )


LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
		     "${CADKIT_INC_DIR}/Experimental"
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
  ./Layers/PointTimeLayer.cpp
  ./Layers/LineLayer.cpp
  ./Layers/PolygonLayer.cpp
  ./Layers/PolygonTimeLayer.cpp
  ./Layers/PointLayer.cpp
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
LINK_CADKIT( Minerva Usul OsgTools Magrathea SerializeXML XmlTree )
TARGET_LINK_LIBRARIES(Minerva
	${Boost_LIBRARIES}
	${PQXX_LIB}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSSIM_LIB}
	${OSSIM_PLANET_LIB}
)

    
    
