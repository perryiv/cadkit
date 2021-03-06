project (TriangulateCGAL)

SET( PLUGIN_NAME "TriangulateCGAL" )

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

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
ADD_LIBRARY( ${PLUGIN_NAME}  SHARED ${SOURCES} )

CADKIT_PLUGIN()

# Link the Library
LINK_CADKIT( ${PLUGIN_NAME}  Usul )

TARGET_LINK_LIBRARIES( ${PLUGIN_NAME}  ${CGAL_LIB} )