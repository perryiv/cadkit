
#########################################################
#
#  CMakeLists for WRF Model.
#
#########################################################

PROJECT (WrfModel)

INCLUDE ( Cadkit.plugin )

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE ( FindOSG )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${CADKIT_INC_DIR}/Experimental" 
                     "${PROJECT_SOURCE_DIR}/../" 
                      ${OSG_INC_DIR} 
		      ${Boost_INCLUDE_DIR} )

# Set the sources to compile.
SET (SOURCES
./WRFComponent.cpp
./WRFFactory.cpp
./Parser.cpp
./WRFDocument.cpp
./Channel.cpp
./LoadDataJob.cpp
)

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "WrfModel" )
SET ( COMPILE_GUARD "_COMPILING_WRF_MODEL" )
SET ( CADKIT_LIBRARIES Usul OsgTools XmlTree SerializeXML MenuKit )
SET ( OTHER_LIBRARIES ${OPENTHREADS_LIB}
					  ${OSG_LIB}
					  ${OSG_TEXT_LIB} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
