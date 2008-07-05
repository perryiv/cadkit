
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

# ------------ Find GDAL Library ----------------------
INCLUDE( ${CMakeModules}/FindGDAL.cmake)

# ------------ Find curl Library ----------------------
INCLUDE( ${CMakeModules}/FindCURL.cmake)

ADD_DEFINITIONS("-D_COMPILING_MINERVA")
LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     ${OSSIM_INC_DIR}
		     ${CURL_INCLUDE_DIR}
		     ${GDAL_INCLUDE_DIR}
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
./Data/DataObject.cpp
./Data/Transform.cpp
./Data/Geometry.cpp
./Data/Line.cpp
./Dagta/Model.cpp
./Data/Object.cpp
./Data/Point.cpp
./Data/Polygon.cpp
./Factory/Readers.cpp
./Functors/GradientColorFunctor.cpp
./Functors/SingleColorFunctor.cpp
./Jobs/BuildRaster.cpp
./Jobs/BuildTiles.cpp
./Layers/Container.cpp
./Layers/ElevationGroup.cpp
./Layers/RasterGroup.cpp
./Layers/RasterLayer.cpp
./Layers/RasterLayerArcIMS.cpp
./Layers/RasterLayerGDAL.cpp
./Layers/RasterLayerNetwork.cpp
./Layers/RasterLayerOssim.cpp
./Layers/RasterLayerWms.cpp
./TileEngine/Body.cpp
./TileEngine/Group.cpp
./TileEngine/LandModelEllipsoid.cpp
./TileEngine/LandModelFlat.cpp
./TileEngine/Node.cpp
./TileEngine/SplitCallbacks.cpp
./TileEngine/System.cpp
./TileEngine/Tile.cpp
./Utilities/Atmosphere.cpp
./Utilities/Compass.cpp
./Utilities/DirtyTiles.cpp
./Utilities/Hud.cpp
./Utilities/SkyDome.cpp
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
	${GDAL_LIBRARY}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSSIM_LIB}
    ${CURL_LIBRARY}
)
