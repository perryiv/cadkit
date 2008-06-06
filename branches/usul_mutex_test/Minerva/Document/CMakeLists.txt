
PROJECT(MinervaDocument)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSG.cmake)


# ------------ Find Ossim Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSSIM.cmake)

# ------------ Find boost ----------------------
SET ( Boost_USE_DATE_TIME TRUE )
INCLUDE( ${CMakeModules}/FindBoost.cmake)

# ------------ Find PQXX Library ----------------------
FIND_LIBRARY(PQXX_LIB pqxx $ENV{PQXX_LIB_DIR} )


LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
         "${CADKIT_INC_DIR}/Experimental"
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     ${WMS_INC_DIR}
		     ${OSSIM_INC_DIR}
		     ${OSG_EPHEMERIS_INC_DIR}
		     $ENV{PQXX_INC_DIR}
		     )


#List the Sources
SET (SOURCES
./CommandSender.cpp
./MinervaDocument.cpp
./KmlWriter.cpp
./MinervaWriter.cpp
./MinervaReader.cpp
./CommandReceiver.cpp
)

# Create a Shared Library
ADD_LIBRARY( MinervaDocument SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( MinervaDocument Usul OsgTools SerializeXML XmlTree Minerva MinervaPostgres )
TARGET_LINK_LIBRARIES( MinervaDocument
	${Boost_LIBRARIES}
	${PQXX_LIB}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSSIM_LIB}
)
