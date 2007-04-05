project (TriangulateCGAL)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

INCLUDE(${CMakeModules}/FindCGAL.cmake)

set(COMPILE_GUARD "_COMPILING_TRIANGULATE_CGAL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR} 
		     ${CGAL_INC_DIR} 
		     ${CGAL_CONFIG_INC_DIR}
		     "${PROJECT_SOURCE_DIR}/../" )

set (SOURCES
    ./TriangulateComponent.cpp
    ./TriangulateFactory.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(TriangulateCGAL SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated library --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (TriangulateCGAL
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (TriangulateCGAL
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library
LINK_CADKIT( TriangulateCGAL Usul )

TARGET_LINK_LIBRARIES( TriangulateCGAL ${CGAL_LIB} )