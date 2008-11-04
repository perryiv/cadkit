project (TriangleModel)

SET(PLUGIN_NAME "TriangleModel")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

# Set the compile guard.
set(COMPILE_GUARD "_COMPILING_TRIANGLE_MODEL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
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

# ---------- Create a Shared Library
ADD_LIBRARY(TriangleModel SHARED ${SOURCES} )

# Create a plugin.
CADKIT_PLUGIN( ${PLUGIN_NAME} )

# Link the Library with Cadkit libraries.
LINK_CADKIT( ${PLUGIN_NAME} Usul OsgTools XmlTree )

# Link the Library	
TARGET_LINK_LIBRARIES( ${PLUGIN_NAME}
	${OSG_LIB}
)

