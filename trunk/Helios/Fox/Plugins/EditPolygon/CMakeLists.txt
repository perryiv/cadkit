project (FoxEditPolygon)

ADD_DEFINITIONS ("-D_COMPILING_EDIT_POLYGON")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" "${FOXTOOLKIT_INCLUDE}" "${GENERIC_NURBS_INCLUDE_DIR}" )

set (SOURCES
	Primitive.cpp
	Islands.cpp
	Geometry.cpp
	FlipNormal.cpp
	EditPolygonFactory.cpp
	EditPolygonComponent.cpp
	CapPolygons.cpp
	AddTriangle.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(FoxEditPolygon SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxEditPolygon
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxEditPolygon
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(FoxEditPolygon 
  Usul 
  FoxTools
  OsgTools
  ${FOXTOOLKIT_LIB}
)

