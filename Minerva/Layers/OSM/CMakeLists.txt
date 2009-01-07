
#########################################################
#
#  CMakeLists for Minerva OpenStreetMap.
#
#########################################################

PROJECT(MinervaOSM)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSG.cmake)


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
		     )


# List the headers
SET ( HEADERS
	./Common.h
	./Export.h
	./Functions.h
	./Node.h
	./Object.h
	./OpenStreetMapFile.h
	./OpenStreetMapXAPI.h
	./TileVectorJob.h
	./Way.h
)
			 
# List the Sources
SET (SOURCES
	./Functions.cpp
	./Node.cpp
	./Object.cpp
	./OpenStreetMapFile.cpp
	./OpenStreetMapXAPI.cpp
	./TileVectorJob.cpp
	./Way.cpp
)

ADD_DEFINITIONS ("-D_COMPILING_MINERVA_OSM")

SET ( TARGET_NAME MinervaOSM )

# Create a Shared Library
ADD_LIBRARY( ${TARGET_NAME} SHARED ${HEADERS} ${SOURCES} )
CADKIT_ADD_LIBRARY ( ${TARGET_NAME} )

# Link the Library	
LINK_CADKIT( ${TARGET_NAME} Usul OsgTools SerializeXML XmlTree Minerva )
TARGET_LINK_LIBRARIES( ${TARGET_NAME}
	${Boost_LIBRARIES}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
)
