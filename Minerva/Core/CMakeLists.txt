
PROJECT(Minerva)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSG.cmake)


# ------------ Find Ossim Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSSIM.cmake)

# ------------ Find boost ----------------------
SET ( Boost_USE_DATE_TIME TRUE )
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
		     ${OSSIM_PLANET_INC_DIR}
		     $ENV{PQXX_INC_DIR}
		     )


#List the Sources
SET (SOURCES
./Layers/PointTimeLayer.cpp
./Layers/Layer.cpp
./Layers/PolygonLayer.cpp
./Layers/LineLayer.cpp
./Layers/PointLayer.cpp
./Layers/Vector.cpp
./Layers/VectorGroup.cpp
./DataObjects/DataObject.cpp
./DataObjects/Line.cpp
./DataObjects/Model.cpp
./DataObjects/Polygon.cpp
./DataObjects/Point.cpp
./Functors/SingleColorFunctor.cpp
./Functors/GradientColorFunctor.cpp
./DB/ConnectionPool.cpp
./DB/Connection.cpp
./DB/Info.cpp
./postGIS/BinaryParser.cpp
./postGIS/TextParser.cpp
./postGIS/Factory.cpp
./Scene/SceneManager.cpp
./Commands/AnimationSpeed.cpp
./Commands/PauseAnimation.cpp
./Commands/HideLayer.cpp
./Commands/ShowLayer.cpp
./Commands/RemoveLayer.cpp
./Commands/AddLayer.cpp
./Commands/StopAnimation.cpp
./Commands/StartAnimation.cpp
./Commands/ToggleShown.cpp
./Commands/ShowPastEvents.cpp
./Commands/ChangeTimestepType.cpp
./Visitors/TemporalAnimation.cpp
./Visitors/FindMinMaxDates.cpp
./Visitors/ResizePoints.cpp
./Visitors/StackPoints.cpp
./Animate/Settings.cpp
./Animate/Date.cpp
./Animate/TimeSpan.cpp
./Geometry/Polygon.cpp
./Geometry/Line.cpp
./Geometry/Point.cpp
./Geometry/Geometry.cpp
./Factory/Readers.cpp
)

# Create a Shared Library
ADD_LIBRARY(Minerva SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( Minerva Usul OsgTools SerializeXML XmlTree StarSystem )
TARGET_LINK_LIBRARIES(Minerva
	${Boost_LIBRARIES}
	${PQXX_LIB}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSSIM_LIB}
)
