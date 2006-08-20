project (TriangleModel)

set(COMPILE_GUARD "_COMPILING_TRIANGLE_MODEL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" )

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

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (TriangleModel
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (TriangleModel
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(TriangleModel 
  Usul
  OsgTools
  ${OPENTHREADS_LIB}
	${OSG_LIB}
)

