
PROJECT(MinervaPostGIS)

SET(CMakeModules ${CADKIT_INC_DIR}/CMakeModules )
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSG.cmake)

# ------------ Find boost ----------------------
SET ( Boost_USE_DATE_TIME TRUE )
INCLUDE( ${CMakeModules}/FindBoost.cmake)

# ------------ Find PQXX Library ----------------------
FIND_LIBRARY(PQXX_LIB pqxx $ENV{PQXX_LIB_DIR} )

# ------------ Find GDAL Library ----------------------
INCLUDE( ${CMakeModules}/FindGDAL.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     $ENV{PQXX_INC_DIR}
         ${GDAL_INCLUDE_DIR}
		     )


#List the Sources
SET (SOURCES
./BinaryParser.cpp
./Layer.cpp
./LineLayer.cpp
./PointLayer.cpp
./PolygonLayer.cpp
)

# Create a Shared Library
ADD_LIBRARY(MinervaPostGIS SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( MinervaPostGIS Usul OsgTools SerializeXML XmlTree Minerva MinervaPostgres )
TARGET_LINK_LIBRARIES(MinervaPostGIS
	${Boost_LIBRARIES}
	${PQXX_LIB}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
  ${GDAL_LIBRARY}
)
