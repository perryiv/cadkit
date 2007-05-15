project (TriangleModel)

SET(PLUGIN_NAME "TriangleModel")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

# Set the compile guard.
set(COMPILE_GUARD "_COMPILING_TRIANGLE_MODEL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" )

set (SOURCES
	TriangleWriterTDF.cpp
	TriangleWriterSTL.cpp
	TriangleReaderTDF.cpp
	TriangleReaderSTL.cpp
	TriangleReaderR3D.cpp
	TriangleFactory.cpp
	TriangleDocument.cpp
	TriangleComponent.cpp
	ParadisReader.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(TriangleModel SHARED ${SOURCES} )

# Create a plugin.
CADKIT_PLUGIN()

# Link the Library with Cadkit libraries.
LINK_CADKIT( ${PLUGIN_NAME} Usul OsgTools )

# Link the Library	
TARGET_LINK_LIBRARIES( ${PLUGIN_NAME}
	${OSG_LIB}
)

