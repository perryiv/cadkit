
#########################################################
#
#  CMakeLists for Minerva Kml Layer.
#
#########################################################

PROJECT(MinervaLayersKml)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSG.cmake)

# try to find the header
FIND_PATH(COLLADA_INCLUDE_DIR dae.h
  $ENV{COLLADA_INC_DIR}
)

# Find the library
FIND_LIBRARY(COLLADA_LIBRARY
	NAMES collada14dom
	PATHS
    $ENV{COLLADA_LIB_DIR}
	DOC "The name of the COLLADA library"
)


#########################################################
#
#  Set link directories.
#
#########################################################

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} ${Boost_LIBRARY_DIRS} )


#########################################################
#
#  Set include directories.
#
#########################################################

INCLUDE_DIRECTORIES( 
			 ${Boost_INCLUDE_DIR}
		     ${CADKIT_INC_DIR}
		     ${OSG_INC_DIR} 
			 ${COLLADA_INCLUDE_DIR} 
			 ${COLLADA_INCLUDE_DIR}/1.4
		     )


#List the Sources
SET (SOURCES
./Factory.cpp
./KmlLayer.cpp
./Link.cpp
./LoadModel.cpp
./NetworkLink.cpp
./ModelPostProcess.cpp
)

# Create a Shared Library
ADD_LIBRARY(MinervaLayersKml SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( MinervaLayersKml Usul OsgTools SerializeXML XmlTree Minerva )
TARGET_LINK_LIBRARIES( MinervaLayersKml
	${Boost_LIBRARIES}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${COLLADA_LIBRARY}
)
