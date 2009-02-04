
#########################################################
#
#  CMakeLists for Triangle Model.
#
#########################################################

project (TriangleModel)

INCLUDE ( Cadkit.plugin )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} ${OSG_INC_DIR} "${PROJECT_SOURCE_DIR}/../" )

set (SOURCES
	ParadisReader.cpp
	TriangleComponent.cpp
	TriangleDocument.cpp
	TriangleFactory.cpp
	TriangleReaderArcAsciiGrid.cpp
	TriangleReaderFieldViewAscii.cpp
	TriangleReaderGrassRaster.cpp
	TriangleReaderOFF.cpp
	TriangleReaderR3D.cpp
	TriangleReaderSTL.cpp
	TriangleReaderTDF.cpp
	TriangleWriterSTL.cpp
	TriangleWriterTDF.cpp
)

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "TriangleModel" )
SET ( COMPILE_GUARD "_COMPILING_TRIANGLE_MODEL" )
SET ( CADKIT_LIBRARIES Usul OsgTools XmlTree )
SET ( OTHER_LIBRARIES ${OSG_LIB} ${OPENTHREADS_LIB} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
