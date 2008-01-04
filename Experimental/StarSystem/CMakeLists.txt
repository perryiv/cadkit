
PROJECT(StarSystem)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)


# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSG.cmake)
INCLUDE(${CMakeModules}/FindOSSIM.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR} 
         "${CADKIT_INC_DIR}/Experimental/"
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
         ${OSSIM_INC_DIR}
		     )


#List the Sources
SET (SOURCES
./Visitor.cpp
./System.cpp
./Node.cpp
./Group.cpp
./Body.cpp
./BuildScene.cpp
./Tile.cpp
./RasterLayerOssim.cpp
./RasterGroup.cpp
./RasterLayerWms.cpp
./CutImageJob.cpp
./RasterLayer.cpp
./LandModelEllipsoid.cpp
./LandModelFlat.cpp
./ElevationGroup.cpp
./DirtyTiles.cpp
./ElevationLayerDem.cpp
./SplitCallbacks.cpp
./BuildTiles.cpp
./SetJobManager.cpp
./Scale.cpp
./Hud.cpp
)

# Create a Shared Library
ADD_LIBRARY(StarSystem SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT(StarSystem Usul OsgTools)
TARGET_LINK_LIBRARIES(StarSystem
	${OPENGL_LIBRARIES}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSG_GA_LIB}
	${OSG_TEXT_LIB}
	${OSG_UTIL_LIB}
  ${OSSIM_LIB}
)

    
    
