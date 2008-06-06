project (Triangulate)

# This plugin depends on the Quake triangulation Code from:
#
#  http://www.cs.cmu.edu/~quake/triangle.html
#  http://www.cs.cmu.edu/~quake/showme.html

set(COMPILE_GUARD "_COMPILING_TRIANGULATE")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" )

set (SOURCES
	TriangulateFactory.cpp
	TriangulateComponent.cpp
)

# ---- Find the CMU Quake Triangulation Library
FIND_LIBRARY(QUAKE_LIB Quake
 ${QUAKE_LIB_DIR}
)

# ---------- Create a Shared Library
ADD_LIBRARY(Triangulate SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (Triangulate
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (Triangulate
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(Triangulate 
  Usul
	#${OPENGL_LIBRARIES}
)

