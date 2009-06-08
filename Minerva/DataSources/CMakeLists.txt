
PROJECT(MinervaDataSource)

INCLUDE ( Cadkit )

ADD_DEFINITIONS("-D_COMPILING_MINERVA_DATA_SOURCE")

IF(MSVC)
	ADD_DEFINITIONS("-D_CRT_SECURE_NO_DEPRECATE")
ENDIF(MSVC)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
			 ${Boost_INCLUDE_DIR}
		     ${CADKIT_INC_DIR}
		     )


#List the Sources
SET (SOURCES
./BinaryString.h
./BinaryString.cpp
./Connection.h
./Connection.cpp
./Result.h
./Result.cpp
)

# Create a Shared Library
ADD_LIBRARY(MinervaDataSource SHARED ${SOURCES} )
CADKIT_ADD_LIBRARY ( MinervaDataSource )

# Link the Library	
LINK_CADKIT( MinervaDataSource Usul SerializeXML XmlTree )
TARGET_LINK_LIBRARIES(MinervaDataSource
	${Boost_LIBRARIES}
)
