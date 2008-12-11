
SUBDIRS( ${CADKIT_INC_DIR}/Minerva/Plugins/Document )

# Build these projects if we found GDAL.
IF(GDAL_FOUND)
	SUBDIRS( ${CADKIT_INC_DIR}/Minerva/Plugins/GDALReadImage )
ENDIF(GDAL_FOUND)

# Add these projects if Qt was found.
IF(QT_FOUND AND BUILD_HELIOS_QT)
	SUBDIRS( ${CADKIT_INC_DIR}/Minerva/Plugins/LayerManager
			 ${CADKIT_INC_DIR}/Minerva/Plugins/QtArcIMS
			 ${CADKIT_INC_DIR}/Minerva/Plugins/WmsLayerQt
			 ${CADKIT_INC_DIR}/Minerva/Plugins/OssimLayerQt
			 ${CADKIT_INC_DIR}/Minerva/Plugins/QtCustomGeometry )
			 
	# Build this project if pqxx is found.
	IF ( PQXX_FOUND )
		SUBDIRS ( ${CADKIT_INC_DIR}/Minerva/Plugins/PostGISLayerQt )
	ENDIF ( PQXX_FOUND )
ENDIF(QT_FOUND AND BUILD_HELIOS_QT)
