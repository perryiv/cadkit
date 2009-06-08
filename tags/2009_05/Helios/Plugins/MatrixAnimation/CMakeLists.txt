
#########################################################
#
#  CMakeLists for Matrix Animation.
#
#########################################################

PROJECT (MatrixAnimation)

INCLUDE ( Cadkit.plugin )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( 
  "${PROJECT_SOURCE_DIR}/../"
	"${CADKIT_INC_DIR}/" 
  "${CADKIT_INC_DIR}/GenericNurbs"
  "${CADKIT_INC_DIR}/Experimental"
  ${OSG_INC_DIR}
)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

SET (SOURCES
./MatrixAnimationComponent.cpp
)

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "MatrixAnimation" )
SET ( COMPILE_GUARD "_COMPILING_MATRIX_ANIMATION" )
SET ( CADKIT_LIBRARIES Usul )
SET ( OTHER_LIBRARIES ${OSG_LIB} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
