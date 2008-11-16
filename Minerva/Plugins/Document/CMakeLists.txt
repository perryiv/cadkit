
#########################################################
#
#  CMakeLists for MinervaModel.
#
#########################################################

PROJECT (MinervaModel)

INCLUDE ( Cadkit.plugin )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR}
					 ${CADKIT_INC_DIR} 
                     "${CADKIT_INC_DIR}/Experimental"
                     "${PROJECT_SOURCE_DIR}/../"
                     ${OSG_INC_DIR} )

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} ${Boost_LIBRARY_DIRS} )

# Set the sources to compile.
SET ( SOURCES
	./MinervaComponent.h
	./MinervaComponent.cpp
	./MinervaFactory.cpp
)

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "MinervaModel" )
SET ( COMPILE_GUARD "_COMPILING_MINERVA_MODEL" )
SET ( CADKIT_LIBRARIES Usul MinervaDocument )
SET ( OTHER_LIBRARIES ${Boost_LIBRARIES}  )

# Add the plugin.
CADKIT_ADD_PLUGIN( ${PLUGIN_NAME} )
