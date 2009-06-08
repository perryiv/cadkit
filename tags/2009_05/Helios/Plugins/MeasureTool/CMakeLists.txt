
#########################################################
#
#  CMakeLists for Measure Tool.
#
#########################################################

project ( MeasureTool )

INCLUDE ( Cadkit.plugin )

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} ${OSG_LIB_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" ${OSG_INC_DIR} )

SET (SOURCES
./MeasureToolComponent.cpp
)

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "MeasureTool" )
SET ( COMPILE_GUARD "_COMPILING_MEASURE_TOOL" )
SET ( CADKIT_LIBRARIES Usul OsgTools MenuKit )
SET ( OTHER_LIBRARIES ${OSG_LIB}
					  ${OSG_UTIL_LIB}
					  ${OSG_TEXT_LIB}
					  ${OSG_GA_LIB} 
					  ${OPENTHREADS_LIB} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
