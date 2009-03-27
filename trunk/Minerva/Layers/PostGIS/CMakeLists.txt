
PROJECT(MinervaPostGIS)

SET(CMakeModules ${CADKIT_INC_DIR}/CMakeModules )
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE(${CMakeModules}/FindOSG.cmake)


INCLUDE(${QT_USE_FILE})

# Add needed preprocessor definitions.
ADD_DEFINITIONS("-D_COMPILING_MINERVA_POSTGIS")

IF(MSVC)
	ADD_DEFINITIONS("-DNOMINMAX -D_CRT_SECURE_NO_DEPRECATE")
ENDIF(MSVC)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} ${Boost_LIBRARY_DIRS} )


# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( 
			 ${Boost_INCLUDE_DIR}
		     ${CADKIT_INC_DIR}
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
         ${GDAL_INCLUDE_DIR}
      )


#List the headers
SET (HEADERS
./BinaryParser.h
./LabelData.h
./Layer.h
./LineLayer.h
./PointLayer.h
./PolygonLayer.h
)

#List the Sources
SET (SOURCES
./BinaryParser.cpp
./LabelData.cpp
./Layer.cpp
./LineLayer.cpp
./PointLayer.cpp
./PolygonLayer.cpp
)

# Create a Shared Library
ADD_LIBRARY(MinervaPostGIS SHARED ${SOURCES} ${HEADERS} )

CADKIT_ADD_LIBRARY ( MinervaPostGIS )

# Link the Library	
LINK_CADKIT( MinervaPostGIS Usul OsgTools SerializeXML XmlTree Minerva MinervaPostgres )
TARGET_LINK_LIBRARIES(MinervaPostGIS
	${Boost_LIBRARIES}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
  ${GDAL_LIBRARY}
)
