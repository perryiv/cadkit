
#########################################################
#
#  CMakeLists for CenterGeometry.
#
#########################################################

project (CenterGeometry)

INCLUDE ( Cadkit.plugin )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" ${OSG_INC_DIR} )

# Set the sources to compile.
set (SOURCES
	CenterGeometryComponent.cpp
	CenterGeometryComponent.h
)

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "CenterGeometry" )
SET ( COMPILE_GUARD "_COMPILING_CENTER_GEOMETRY" )
SET ( CADKIT_LIBRARIES Usul OsgTools MenuKit )
SET ( OTHER_LIBRARIES ${OPENTHREADS_LIB} ${OSG_LIB}	${OSG_UTIL_LIB} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
