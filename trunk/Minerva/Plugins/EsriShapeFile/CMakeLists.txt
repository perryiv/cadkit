project (EsriShapeFile)

SET(PLUGIN_NAME "EsriShapeFile")

SET(CMakeModules "$ENV{CADKIT_INC_DIR}/CMakeModules" )
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

set(COMPILE_GUARD "_COMPILING_ESRI_SHAPE_FILE")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${PROJECT_SOURCE_DIR}/../"
                     ${OSG_INC_DIR}
                     ${GDAL_INCLUDE_DIR} )

set (SOURCES
./EsriShapeFileComponent.cpp
./EsriShapeFileDocument.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY( ${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN()

# Link the Library
LINK_CADKIT( ${PLUGIN_NAME} Usul )	


TARGET_LINK_LIBRARIES( ${PLUGIN_NAME}
  ${GDAL_LIBRARY}
	${OSG_LIB}
)
