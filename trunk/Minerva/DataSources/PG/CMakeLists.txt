
PROJECT(MinervaPostgres)

SET(CMakeModules ${CADKIT_INC_DIR}/CMakeModules )
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

ADD_DEFINITIONS("-D_COMPILING_MINERVA_POSTGRES")

IF(MSVC)
	ADD_DEFINITIONS("-D_CRT_SECURE_NO_DEPRECATE")
ENDIF(MSVC)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
			 ${Boost_INCLUDE_DIR}
		     ${CADKIT_INC_DIR}
		     ${POSTGRESQL_INCLUDE_DIR}
		     )


#List the Sources
SET (SOURCES
./Connection.h
./Connection.cpp
./ConnectionPool.h
./ConnectionPool.cpp
./Info.h
./Info.cpp
./Result.h
./Result.cpp
)

# Create a Shared Library
ADD_LIBRARY(MinervaPostgres SHARED ${SOURCES} )
CADKIT_ADD_LIBRARY ( MinervaPostgres )

# Link the Library	
LINK_CADKIT( MinervaPostgres Usul SerializeXML XmlTree MinervaDataSource )
TARGET_LINK_LIBRARIES(MinervaPostgres
	${Boost_LIBRARIES}
	${POSTGRESQL_LIBRARIES}
)

IF(WIN32)
	TARGET_LINK_LIBRARIES(MinervaPostgres wsock32)
ENDIF(WIN32)
