
INCLUDE ( Cadkit )

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE( FindOSG )

# Add needed preprocessor definitions.
ADD_DEFINITIONS("-D_COMPILING_MINERVA_DOCUMENT")

IF(MSVC)
	ADD_DEFINITIONS("-DNOMINMAX -D_CRT_SECURE_NO_DEPRECATE")
ENDIF(MSVC)

# Add directories to search for libraries to link to.
LINK_DIRECTORIES( ${CADKIT_BIN_DIR} ${Boost_LIBRARY_DIRS} )


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
			 ${Boost_INCLUDE_DIR}
		     ${CADKIT_INC_DIR}
			 ${CADKIT_INC_DIR}/GenericNurbs
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     )


#List the Sources
SET (SOURCES
#./CommandSender.cpp
./MinervaDocument.cpp
./Manipulator.cpp
./KmlWriter.cpp
#./CommandReceiver.cpp
)

# Create a Shared Library
ADD_LIBRARY( MinervaDocument SHARED ${SOURCES} )

CADKIT_ADD_LIBRARY ( MinervaDocument )
			
# Link the Library	
LINK_CADKIT( MinervaDocument Usul OsgTools SerializeXML XmlTree MenuKit Minerva )
TARGET_LINK_LIBRARIES( MinervaDocument
	${Boost_LIBRARIES}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSG_UTIL_LIB}
	${OSG_GA_LIB}
)
