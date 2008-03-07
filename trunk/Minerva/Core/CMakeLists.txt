
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


LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     ${OSSIM_INC_DIR}
		     )


#List the Sources
SET (SOURCES
./Animate/Date.cpp
./Animate/Settings.cpp
./Animate/TimeSpan.cpp
./Commands/AddLayer.cpp
./Commands/AnimationSpeed.cpp
./Commands/ChangeTimestepType.cpp
./Commands/HideLayer.cpp
./Commands/PauseAnimation.cpp
./Commands/RemoveLayer.cpp
./Commands/ShowLayer.cpp
./Commands/ShowPastEvents.cpp
./Commands/StartAnimation.cpp
./Commands/StopAnimation.cpp
./Commands/ToggleShown.cpp
./DataObjects/DataObject.cpp
./DataObjects/Line.cpp
./DataObjects/Model.cpp
./DataObjects/Point.cpp
./DataObjects/Polygon.cpp
./Factory/Readers.cpp
./Functors/GradientColorFunctor.cpp
./Functors/SingleColorFunctor.cpp
./Geometry/Geometry.cpp
./Geometry/Line.cpp
./Geometry/Point.cpp
./Geometry/Polygon.cpp
./Jobs/BuildRaster.cpp
./Jobs/BuildTiles.cpp
./Layers/ElevationGroup.cpp
./Layers/ElevationLayerArcAscii.cpp
./Layers/ElevationLayerDem.cpp
./Layers/RasterGroup.cpp
./Layers/RasterLayer.cpp
./Layers/RasterLayerOssim.cpp
./Layers/RasterLayerWms.cpp
./Layers/Vector.cpp
./Layers/VectorGroup.cpp
./TileEngine/Body.cpp
./TileEngine/Group.cpp
./TileEngine/LandModelEllipsoid.cpp
./TileEngine/LandModelFlat.cpp
./TileEngine/Node.cpp
./TileEngine/SplitCallbacks.cpp
./TileEngine/System.cpp
./TileEngine/Tile.cpp
./Utilities/Compass.cpp
./Utilities/DirtyTiles.cpp
./Utilities/Hud.cpp
./Visitor.cpp
./Visitors/BuildLegend.cpp
./Visitors/BuildScene.cpp
./Visitors/FindMinMaxDates.cpp
./Visitors/ResizePoints.cpp
./Visitors/Scale.cpp
./Visitors/SetJobManager.cpp
./Visitors/StackPoints.cpp
./Visitors/TemporalAnimation.cpp
)

# Create a Shared Library
ADD_LIBRARY(Minerva SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( Minerva Usul OsgTools SerializeXML XmlTree )
TARGET_LINK_LIBRARIES(Minerva
	${Boost_LIBRARIES}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSSIM_LIB}
        curl
)
