
PROJECT(MinervaAll)

SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/Core
		  ${CADKIT_INC_DIR}/Minerva/Layers/Kml
		  ${CADKIT_INC_DIR}/Minerva/Layers/GeoNames
		  
		  ${CADKIT_INC_DIR}/Minerva/Document )
		

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
	SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/Layers/PostGIS
			  ${CADKIT_INC_DIR}/Minerva/Layers/GDAL )
ENDIF(GDAL_FOUND AND POSTGRESQL_FOUND)

# Build these projects if we found Ossim
IF(OSSIM_FOUND)
	SUBDIRS( ${CADKIT_INC_DIR}/Minerva/Layers/OSSIM )
ENDIF(OSSIM_FOUND)

# Add the plugins
SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/Plugins )
