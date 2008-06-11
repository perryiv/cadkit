project (PointModel)

SET(PLUGIN_NAME "PointModel")

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
	PointComponent.cpp
	PointDocument.cpp
	PointFactory.cpp
	PointReaderArcAsciiGrid.cpp
	PointReaderFieldViewAscii.cpp
	PointReaderGrassRaster.cpp
	PointReaderOFF.cpp
	PointReaderR3D.cpp
	PointReaderSTL.cpp
	PointReaderTDF.cpp
	PointWriterSTL.cpp
	PointWriterTDF.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(PointModel SHARED ${SOURCES} )

# Create a plugin.
CADKIT_PLUGIN()

# Link the Library with Cadkit libraries.
LINK_CADKIT( ${PLUGIN_NAME} Usul OsgTools )

# Link the Library	
TARGET_LINK_LIBRARIES( ${PLUGIN_NAME}
	${OSG_LIB}
)

