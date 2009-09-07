
PROJECT(MinervaGDAL)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSG.cmake)

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
./RasterLayerGDAL.cpp
./OGRVectorLayer.cpp
./WarpOptions.cpp
)

IF(POSTGRESQL_FOUND)
	SET ( SOURCES ${SOURCES} ./RasterPolygonLayer.cpp )
ENDIF(POSTGRESQL_FOUND)

SET ( TARGET_NAME MinervaGDAL )

# Create a Shared Library
IF (BUILD_GDAL_PLUGIN_AS_BUNDLE )
  ADD_LIBRARY( ${TARGET_NAME} MODULE ${SOURCES} )
ELSE(BUILD_GDAL_PLUGIN_AS_BUNDLE )
  ADD_LIBRARY( ${TARGET_NAME} SHARED ${SOURCES} )
ENDIF (BUILD_GDAL_PLUGIN_AS_BUNDLE )

CADKIT_ADD_LIBRARY ( ${TARGET_NAME} )

IF (BUILD_GDAL_PLUGIN_AS_BUNDLE )
  ADD_DEFINITIONS ("-D_DARWIN")
  SET_TARGET_PROPERTIES (${TARGET_NAME} PROPERTIES SUFFIX ".bundle" )
  SET ( CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "-WI -bind_at_load -bundle" )
ENDIF (BUILD_GDAL_PLUGIN_AS_BUNDLE )

# Link the Library	
LINK_CADKIT( ${TARGET_NAME} Usul OsgTools SerializeXML XmlTree Minerva )
TARGET_LINK_LIBRARIES( ${TARGET_NAME}
	${Boost_LIBRARIES}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${GDAL_LIBRARY}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
)

IF(POSTGRESQL_FOUND)
	LINK_CADKIT( ${TARGET_NAME} MinervaPostgres MinervaPostGIS  )
	TARGET_LINK_LIBRARIES( ${TARGET_NAME} ${POSTGRES_LIBRARIES} )
	
	IF(WIN32)
		TARGET_LINK_LIBRARIES( ${TARGET_NAME} wsock32)
	ENDIF(WIN32)

ENDIF(POSTGRESQL_FOUND)
