
PROJECT(MinervaAll)

SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/Core
		  ${CADKIT_INC_DIR}/Minerva/Layers/Kml
		  ${CADKIT_INC_DIR}/Minerva/Layers/GeoNames
		  ${CADKIT_INC_DIR}/Minerva/Layers/OSM
		  ${CADKIT_INC_DIR}/Minerva/Document )

# Add these projects if Qt was found.
IF(QT_FOUND AND BUILD_HELIOS_QT)
	SUBDIRS( ${CADKIT_INC_DIR}/Minerva/Qt/Widgets
	         ${CADKIT_INC_DIR}/Minerva/Application )
ENDIF(QT_FOUND AND BUILD_HELIOS_QT)

# Build this project only if Boost regex library is found.
IF ( Boost_REGEX_LIBRARY_FOUND )
	SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/Layers/GeoRSS )
ENDIF ( Boost_REGEX_LIBRARY_FOUND )

# Build this project if pqxx is found.
IF ( PQXX_FOUND )
	SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/DataSources/PG )
ENDIF ( PQXX_FOUND )

# Build these projects if we found GDAL and pqxx.
IF(GDAL_FOUND AND PQXX_FOUND )
	SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/Layers/PostGIS
			  ${CADKIT_INC_DIR}/Minerva/Layers/GDAL )
ENDIF(GDAL_FOUND AND PQXX_FOUND)

# Build these projects if we found Ossim
IF(OSSIM_FOUND)
	SUBDIRS( ${CADKIT_INC_DIR}/Minerva/Layers/OSSIM )
ENDIF(OSSIM_FOUND)

# Add the plugins
SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/Plugins )
