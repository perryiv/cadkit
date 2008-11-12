
PROJECT(MinervaLayersGDAL)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSG.cmake)

# ------------ Find PQXX Library ----------------------
FIND_LIBRARY(PQXX_LIB pqxx $ENV{PQXX_LIB_DIR} )


LINK_DIRECTORIES( ${CADKIT_BIN_DIR} ${Boost_LIBRARY_DIRS} )

# Add definitions
ADD_DEFINITIONS ("-D_COMPILING_MINERVA_GDAL")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
			 ${Boost_INCLUDE_DIR}
		     ${CADKIT_INC_DIR}
		     "${CADKIT_INC_DIR}/Experimental"
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     $ENV{PQXX_INC_DIR}
             ${GDAL_INCLUDE_DIR}
		     )


#List the Sources
SET (SOURCES
./Init.cpp
./RasterPolygonLayer.cpp
./RasterLayerGDAL.cpp
)

# Create a Shared Library
ADD_LIBRARY(MinervaLayersGDAL SHARED ${SOURCES} )

CADKIT_ADD_LIBRARY ( MinervaLayersGDAL )

# Link the Library	
LINK_CADKIT( MinervaLayersGDAL Usul OsgTools SerializeXML XmlTree Minerva MinervaPostGIS  )
TARGET_LINK_LIBRARIES( MinervaLayersGDAL
	${Boost_LIBRARIES}
	${GDAL_LIBRARY}
	${PQXX_LIB}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
)
