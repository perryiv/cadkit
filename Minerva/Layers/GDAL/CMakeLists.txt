
PROJECT(MinervaGDAL)

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
             ${GDAL_INCLUDE_DIR}
		     )


#List the Sources
SET (SOURCES
./Dataset.cpp
./Init.cpp
./RasterPolygonLayer.cpp
./RasterLayerGDAL.cpp
./OGRVectorLayer.cpp
)

SET ( TARGET_NAME MinervaGDAL )

# Create a Shared Library
ADD_LIBRARY( ${TARGET_NAME} SHARED ${SOURCES} )

CADKIT_ADD_LIBRARY ( ${TARGET_NAME} )

# Link the Library	
LINK_CADKIT( ${TARGET_NAME} Usul OsgTools SerializeXML XmlTree Minerva MinervaPostgres MinervaPostGIS  )
TARGET_LINK_LIBRARIES( ${TARGET_NAME}
	${Boost_LIBRARIES}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${GDAL_LIBRARY}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
)
