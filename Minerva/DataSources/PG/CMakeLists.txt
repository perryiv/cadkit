
PROJECT(MinervaPostgres)

SET(CMakeModules ${CADKIT_INC_DIR}/CMakeModules )
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# ------------ Find boost ----------------------
INCLUDE( ${CMakeModules}/FindBoost.cmake)

# ------------ Find PQXX Library ----------------------
FIND_LIBRARY(PQXX_LIB pqxx $ENV{PQXX_LIB_DIR} )


LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
		     $ENV{PQXX_INC_DIR}
		     )


#List the Sources
SET (SOURCES
./Connection.cpp
./ConnectionPool.cpp
./Info.cpp
)

# Create a Shared Library
ADD_LIBRARY(MinervaPostgres SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( MinervaPostgres Usul SerializeXML XmlTree )
TARGET_LINK_LIBRARIES(MinervaPostgres
	${Boost_LIBRARIES}
	${PQXX_LIB}
)
