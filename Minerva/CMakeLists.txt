
PROJECT(MinervaAll)

SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/Core
		  ${CADKIT_INC_DIR}/Minerva/Layers/GeoNames		  
		  ${CADKIT_INC_DIR}/Minerva/Document )


# Try to find Collada
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

IF (COLLADA_INCLUDE_DIR AND COLLADA_LIBRARY)
  SET(COLLADA_FOUND TRUE)
ENDIF (COLLADA_INCLUDE_DIR AND COLLADA_LIBRARY)

IF(COLLADA_FOUND)
  SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/Layers/Kml )
ENDIF(COLLADA_FOUND)


IF ( SPATIALITE_FOUND )
	SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/Layers/OSM )
ENDIF ( SPATIALITE_FOUND )	


# Add these projects if Qt was found.
IF(QT_FOUND AND BUILD_HELIOS_QT)
	SUBDIRS( ${CADKIT_INC_DIR}/Minerva/Qt/Widgets
	         ${CADKIT_INC_DIR}/Minerva/Application )
ENDIF(QT_FOUND AND BUILD_HELIOS_QT)

# Build this project only if Boost regex library is found.
#IF ( Boost_REGEX_LIBRARY_FOUND )
	SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/Layers/GeoRSS )
#ENDIF ( Boost_REGEX_LIBRARY_FOUND )

SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/DataSources )

# Build this project if pqxx is found.
IF ( POSTGRESQL_FOUND )
	SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/DataSources/PG )
ENDIF ( POSTGRESQL_FOUND )

# Build these projects if we found GDAL and libpq.
IF(GDAL_FOUND AND POSTGRESQL_FOUND)
	SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/Layers/PostGIS )
ENDIF(GDAL_FOUND AND POSTGRESQL_FOUND)

IF(GDAL_FOUND)
	SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/Layers/GDAL )
ENDIF(GDAL_FOUND)


# Build these projects if we found Ossim
IF(OSSIM_FOUND)
	SUBDIRS( ${CADKIT_INC_DIR}/Minerva/Layers/OSSIM )
ENDIF(OSSIM_FOUND)

# Add the plugins
SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/Plugins )
