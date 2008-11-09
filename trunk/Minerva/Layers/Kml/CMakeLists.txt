
#########################################################
#
#  CMakeLists for Minerva Kml Layer.
#
#########################################################

PROJECT(MinervaKml)

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


# List the headers
SET ( HEADERS
	./Factory.h
	./KmlLayer.h
	./Link.h
	./LoadModel.h
	./NetworkLink.h
	./ModelPostProcess.h
)
			 
# List the Sources
SET (SOURCES
	./Factory.cpp
	./KmlLayer.cpp
	./Link.cpp
	./LoadModel.cpp
	./NetworkLink.cpp
	./ModelPostProcess.cpp
)

SET ( TARGET_NAME MinervaKml )

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
	${COLLADA_LIBRARY}
)
