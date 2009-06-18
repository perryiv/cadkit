
#########################################################
#
#  CMakeLists for Minerva Kml Layer.
#
#########################################################

PROJECT(MinervaKml)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

ADD_DEFINITIONS("-D_COMPILING_MINERVA_KML")

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSG.cmake)

# Currenly zlib is only used on Windows.
IF(WIN32)
	FIND_PACKAGE( ZLib )
ENDIF(WIN32)

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
	./LoadModel.h
	./NetworkLink.h
	./ModelPostProcess.h
)
			 
# List the Sources
SET (SOURCES
	./Factory.cpp
	./KmlLayer.cpp
	./LoadModel.cpp
	./NetworkLink.cpp
	./ModelPostProcess.cpp
)

# Add header, cpp and include directory.
IF(ZLIB_FOUND)
	SET ( HEADERS ${HEADERS} ZipFile.h )
	SET ( SOURCES ${SOURCES} ZipFile.cpp )
	
	INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR}/contrib )
	
	IF(MSVC)
		ADD_DEFINITIONS ( "-DZLIB_WINAPI" )
	ENDIF(MSVC)
	
ENDIF(ZLIB_FOUND)

SET ( TARGET_NAME MinervaKml )

# Create a Shared Library
ADD_LIBRARY( ${TARGET_NAME} SHARED ${HEADERS} ${SOURCES} )
CADKIT_ADD_LIBRARY ( ${TARGET_NAME} )

# Link the Library	
LINK_CADKIT( ${TARGET_NAME} Usul OsgTools SerializeXML XmlTree Minerva )
TARGET_LINK_LIBRARIES( ${TARGET_NAME}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${COLLADA_LIBRARY}
)

# Link to zlib.
IF(ZLIB_FOUND)
	TARGET_LINK_LIBRARIES( ${TARGET_NAME} ${ZLIB_LIBRARIES} )
ENDIF(ZLIB_FOUND)
