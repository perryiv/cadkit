
#########################################################
#
#  CMakeLists for Minerva GeoNames.
#
#########################################################

PROJECT(MinervaGeoNames)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSG.cmake)

# ------------ Find curl Library ----------------------
INCLUDE ( FindCURL )

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
			 ${CURL_INCLUDE_DIR}
		     )


# List the headers
SET ( HEADERS
	./City.h
	./CityLayer.h
	./Export.h
)
			 
# List the Sources
SET (SOURCES
	./City.cpp
	./CityLayer.cpp
)

ADD_DEFINITIONS ("-D_COMPILING_MINERVA_GEO_NAMES")

IF(MSVC)
	ADD_DEFINITIONS("-DCURL_STATICLIB")
ENDIF(MSVC)

SET ( TARGET_NAME MinervaGeoNames )

# Create a Shared Library
ADD_LIBRARY( ${TARGET_NAME} SHARED ${HEADERS} ${SOURCES} )
CADKIT_ADD_LIBRARY ( ${TARGET_NAME} )

# Link the Library	
LINK_CADKIT( ${TARGET_NAME} Usul OsgTools SerializeXML XmlTree Minerva )
TARGET_LINK_LIBRARIES( ${TARGET_NAME}
	${Boost_LIBRARIES}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_TEXT_LIB}
	${CURL_LIBRARY}
)

# If we are building on Windows, link to these libraries.
IF(WIN32)
	TARGET_LINK_LIBRARIES( ${TARGET_NAME} Ws2_32 wldap32 )
ENDIF(WIN32)
