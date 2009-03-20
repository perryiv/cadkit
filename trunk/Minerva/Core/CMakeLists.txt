
#########################################################
#
#  CMakeLists for Minerva library.
#
#########################################################

PROJECT ( Minerva )

INCLUDE ( Cadkit )

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE( FindOSG )

# ------------ Find curl Library ----------------------
INCLUDE( FindCURL )

ADD_DEFINITIONS("-D_COMPILING_MINERVA")

IF(MSVC)
	ADD_DEFINITIONS("-DCURL_STATICLIB -D_CRT_SECURE_NO_WARNINGS")
ENDIF(MSVC)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} ${Boost_LIBRARY_DIRS} )


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
			 ${Boost_INCLUDE_DIR}
		     ${CADKIT_INC_DIR}
		     ${CADKIT_INC_DIR}/GenericNurbs
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     ${CURL_INCLUDE_DIR}
		     )

#########################################################
#
#  Add Source groups.
#
#########################################################

SOURCE_GROUP ( Source                FILES Export.h Extents.h Macros.h Serialize.h Visitor.h Visitor.cpp )
SOURCE_GROUP ( Source\\Algorithms    REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Algorithms/* )
SOURCE_GROUP ( Source\\Animate       REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Animate/* )
SOURCE_GROUP ( Source\\Commands      REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Commands/* )
SOURCE_GROUP ( Source\\Data          REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Data/* )
SOURCE_GROUP ( Source\\Factory       REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Factory/* )
SOURCE_GROUP ( Source\\Functions     REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Functions/* )
SOURCE_GROUP ( Source\\Functors      REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Functors/* )
SOURCE_GROUP ( Source\\Jobs          REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Jobs/* )
SOURCE_GROUP ( Source\\Layers        REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Layers/* )
SOURCE_GROUP ( Source\\OSG           REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/OSG/* )
SOURCE_GROUP ( Source\\TileEngine    REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/TileEngine/* )
SOURCE_GROUP ( Source\\Utilities     REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Utilities/* )
SOURCE_GROUP ( Source\\Visitors      REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Visitors/* )


#########################################################
#
#  List all the headers
#
#########################################################

SET ( HEADERS
	./Algorithms/Composite.h
	./Algorithms/Resample.h
	./Algorithms/ResampleElevation.h
	./Algorithms/SubRegion.h
	./Animate/Date.h
	./Animate/Settings.h
	./Commands/AddLayer.h
	./Commands/AnimationSpeed.h
	./Commands/ChangeTimestepType.h
	./Commands/HideLayer.h
	./Commands/PauseAnimation.h
	./Commands/RemoveLayer.h
	./Commands/ShowLayer.h
	./Commands/ShowPastEvents.h
	./Commands/StartAnimation.h
	./Commands/StopAnimation.h
	./Commands/ToggleShown.h
	./Data/AbstractView.h
	./Data/Camera.h
	./Data/ColorStyle.h
	./Data/Container.h
	./Data/DataObject.h
	./Data/Feature.h
	./Data/Geometry.h
	./Data/IconStyle.h
	./Data/Line.h
	./Data/LineStyle.h
	./Data/LookAt.h
	./Data/Model.h
	./Data/ModelCache.h
	./Data/MultiPoint.h
	./Data/Object.h
	./Data/Point.h
	./Data/Polygon.h
	./Data/PolyStyle.h
	./Data/Style.h
	./Data/TimePrimitive.h
	./Data/TimeSpan.h
	./Data/TimeStamp.h
	./Data/Transform.h
	./Data/UserData.h
	./ElevationData.h
	./Export.h
	./Extents.h
	./Factory/Readers.h
	./Functions/CacheString.h
	./Functions/MakeBody.h
	./Functions/ReadFile.h
	./Functions/SearchDirectory.h
	./Functors/BaseColorFunctor.h
	./Functors/GradientColorFunctor.h
	./Functors/SingleColorFunctor.h
	./Jobs/BuildRaster.h
	./Jobs/BuildTiles.h
	./Layers/ElevationGroup.h
	./Layers/ElevationLayerDirectory.h
	./Layers/LayerInfo.h
	./Layers/RasterGroup.h
	./Layers/RasterLayer.h
	./Layers/RasterLayerArcIMS.h
	./Layers/RasterLayerNetwork.h
	./Layers/RasterLayerWms.h
	./Macros.h
	./Navigator.h
	./Serialize.h
	./TileEngine/Body.h
	./TileEngine/LandModel.h
	./TileEngine/LandModelEllipsoid.h
	./TileEngine/Mesh.h
	./TileEngine/SplitCallbacks.h
	./TileEngine/Tile.h
	./Utilities/Atmosphere.h
	./Utilities/ClampNearFar.h
	./Utilities/Compass.h
	./Utilities/Download.h
	./Utilities/GeoCode.h
	./Utilities/Hud.h
	./Utilities/SkyDome.h
	./Visitor.h
	./Visitors/BuildLegend.h
	./Visitors/FindMinMaxDates.h
	./Visitors/FindObject.h
	./Visitors/FindUnknowns.h
	./Visitors/ResizePoints.h
	./Visitors/Scale.h
	./Visitors/StackPoints.h
	./Visitors/TemporalAnimation.h
)


#########################################################
#
#  List the sources.
#
#########################################################

SET (SOURCES
./Algorithms/ResampleElevation.cpp
./Animate/Date.cpp
./Animate/Settings.cpp
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
./Data/AbstractView.cpp
./Data/Camera.cpp
./Data/ColorStyle.cpp
./Data/Container.cpp
./Data/DataObject.cpp
./Data/Feature.cpp
./Data/IconStyle.cpp
./Data/Geometry.cpp
./Data/Line.cpp
./Data/LineStyle.cpp
./Data/LookAt.cpp
./Data/Model.cpp
./Data/ModelCache.cpp
./Data/MultiPoint.cpp
./Data/Object.cpp
./Data/Point.cpp
./Data/Polygon.cpp
./Data/PolyStyle.cpp
./Data/Style.cpp
./Data/TimePrimitive.cpp
./Data/TimeSpan.cpp
./Data/TimeStamp.cpp
./ElevationData.cpp
./Factory/Readers.cpp
./Functions/CacheString.cpp
./Functions/MakeBody.cpp
./Functions/ReadFile.cpp
./Functions/SearchDirectory.cpp
./Functors/GradientColorFunctor.cpp
./Functors/SingleColorFunctor.cpp
./Jobs/BuildRaster.cpp
./Jobs/BuildTiles.cpp
./Layers/ElevationGroup.cpp
./Layers/ElevationLayerDirectory.cpp
./Layers/RasterGroup.cpp
./Layers/RasterLayer.cpp
./Layers/RasterLayerArcIMS.cpp
./Layers/RasterLayerNetwork.cpp
./Layers/RasterLayerWms.cpp
./Navigator.cpp
./TileEngine/Body.cpp
./TileEngine/LandModelEllipsoid.cpp
./TileEngine/Mesh.cpp
./TileEngine/SplitCallbacks.cpp
./TileEngine/Tile.cpp
./Utilities/Atmosphere.cpp
./Utilities/Download.cpp
./Utilities/Compass.cpp
./Utilities/GeoCode.cpp
./Utilities/Hud.cpp
./Utilities/SkyDome.cpp
./Visitor.cpp
./Visitors/BuildLegend.cpp
./Visitors/FindMinMaxDates.cpp
./Visitors/FindObject.cpp
./Visitors/ResizePoints.cpp
./Visitors/Scale.cpp
./Visitors/StackPoints.cpp
./Visitors/TemporalAnimation.cpp
)

# Create a Shared Library
ADD_LIBRARY(Minerva SHARED ${SOURCES} ${HEADERS} )

# Add a library
CADKIT_ADD_LIBRARY ( Minerva )

# Link the Library	
LINK_CADKIT( Minerva Usul OsgTools SerializeXML XmlTree )
TARGET_LINK_LIBRARIES(Minerva
	${Boost_DATE_TIME_LIBRARY}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSG_TEXT_LIB}
	${OSG_UTIL_LIB}
    ${CURL_LIBRARY}
)

IF(WIN32)
	TARGET_LINK_LIBRARIES( Minerva Ws2_32 wldap32 )
ENDIF(WIN32)
