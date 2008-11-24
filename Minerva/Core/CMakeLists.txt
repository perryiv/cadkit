
#########################################################
#
#  CMakeLists for Minerva library.
#
#########################################################

PROJECT(Minerva)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSG.cmake)

# ------------ Find curl Library ----------------------
INCLUDE( ${CMakeModules}/FindCURL.cmake)

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


#List the Sources
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
./Data/Object.cpp
./Data/Point.cpp
./Data/Polygon.cpp
./Data/PolyStyle.cpp
./Data/Style.cpp
./Data/TimePrimitive.cpp
./Data/TimeSpan.cpp
./Data/TimeStamp.cpp
./Factory/Readers.cpp
./Functions/CacheString.cpp
./Functions/MakeBody.cpp
./Functions/ParameterizeExtents.cpp
./Functors/GradientColorFunctor.cpp
./Functors/SingleColorFunctor.cpp
./Jobs/BuildRaster.cpp
./Jobs/BuildTiles.cpp
./Layers/ElevationGroup.cpp
./Layers/RasterGroup.cpp
./Layers/RasterLayer.cpp
./Layers/RasterLayerArcIMS.cpp
./Layers/RasterLayerNetwork.cpp
./Layers/RasterLayerWms.cpp
./OSG/MatrixTransform.cpp
./TileEngine/Body.cpp
./TileEngine/Group.cpp
./TileEngine/LandModelEllipsoid.cpp
./TileEngine/Mesh.cpp
./TileEngine/Node.cpp
./TileEngine/SplitCallbacks.cpp
./TileEngine/System.cpp
./TileEngine/Tile.cpp
./Utilities/Atmosphere.cpp
./Utilities/Download.cpp
./Utilities/Compass.cpp
./Utilities/DirtyTiles.cpp
./Utilities/GeoCode.cpp
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
