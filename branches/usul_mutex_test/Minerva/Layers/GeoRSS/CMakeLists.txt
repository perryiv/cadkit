
PROJECT(MinervaLayersKml)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSG.cmake)


# ------------ Find Ossim Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSSIM.cmake)

# ------------ Find boost ----------------------
SET ( Boost_USE_DATE_TIME TRUE )
SET ( Boost_USE_FILESYSTEM TRUE )
INCLUDE( ${CMakeModules}/FindBoost.cmake)


LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
		     "${CADKIT_INC_DIR}/Experimental"
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     ${OSSIM_INC_DIR}
		     )


#List the Sources
SET (SOURCES
./ColorStyle.cpp
./Feature.cpp
./KmlLayer.cpp
./Link.cpp
./LookAt.cpp
./NetworkLink.cpp
./Object.cpp
./PolyStyle.cpp
./Style.cpp
)

# Create a Shared Library
ADD_LIBRARY(MinervaLayersKml SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( MinervaLayersKml Usul OsgTools SerializeXML XmlTree Minerva )
TARGET_LINK_LIBRARIES( MinervaLayersKml
	${Boost_LIBRARIES}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSSIM_LIB}
)
