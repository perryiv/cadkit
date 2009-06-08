
#########################################################
#
#  CMakeLists for Flash Model.
#
#########################################################

project (FlashModel)

INCLUDE ( Cadkit.plugin )

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE ( FindOSG )

IF(WIN32)
	ADD_DEFINITIONS ("-D_HDF5USEDLL_")
ENDIF(WIN32)


#########################################################
#
#  Set Include Folders.
#
#########################################################

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} 
					 ${CADKIT_INC_DIR} 
					 "${CADKIT_INC_DIR}/Experimental" 
					 "${PROJECT_SOURCE_DIR}/../" 
					 ${OSG_INC_DIR} 
					 ${HDF_INCLUDE_DIR} )

# Set the sources to compile.
set (SOURCES
./Dataset.cpp
./FlashComponent.cpp
./FlashDocument.cpp
./H5File.cpp
./Timestep.cpp
)

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "FlashModel" )
SET ( COMPILE_GUARD "_COMPILING_FLASH_MODEL" )
SET ( CADKIT_LIBRARIES Usul OsgTools XmlTree SerializeXML MenuKit )
SET ( OTHER_LIBRARIES ${OPENTHREADS_LIB}
					  ${OSG_LIB}
					  ${OSG_UTIL_LIB}
					  ${OSG_TEXT_LIB}
					  ${HDF_LIBRARY} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
