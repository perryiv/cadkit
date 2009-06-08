
#########################################################
#
#  CMakeLists for Modflow Model.
#
#########################################################

project (ModflowModel)

INCLUDE ( Cadkit.plugin )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" ${Boost_INCLUDE_DIR} ${OSG_INC_DIR} ${GDAL_INCLUDE_DIR} )

# Set the sources to compile.
SET (SOURCES
./Builders/Geometry.cpp
./ModflowDocument.cpp
./Model/Cell.cpp
./Model/Layer.cpp
./Readers/BasicPackage.cpp
./Readers/Discretization.cpp
./Readers/Pumping.cpp
./Readers/BlockCenteredFlow.cpp
./Readers/Recharge.cpp
./Readers/BaseReader.cpp
./Readers/AuxiliaryData.cpp
./Readers/HeadLevelOutput.cpp
./ModflowComponent.cpp
./Base/BaseObject.cpp
./Attributes/AuxiliaryData.cpp
./Attributes/HeadLevels.cpp
./Attributes/HeadSurface.cpp
./Attributes/Quads.cpp
./Attributes/Attribute.cpp
./Attributes/CellBoundary.cpp
./Attributes/Cylinders.cpp
)

# Set variables that the CADKIT_ADD_PLUGIN macro uses.
SET ( PLUGIN_NAME "ModflowModel" )
SET ( COMPILE_GUARD "_COMPILING_MODFLOW_MODEL" )
SET ( CADKIT_LIBRARIES Usul OsgTools XmlTree SerializeXML )
SET ( OTHER_LIBRARIES ${OPENTHREADS_LIB} ${OSG_LIB} ${OSG_DB_LIB} ${GDAL_LIBRARY} )

# Add the plugin.
CADKIT_ADD_PLUGIN ( ${PLUGIN_NAME} )
