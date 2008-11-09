project (ModflowModel)

# Set the name.  Needed for the macros to set up plugin properties.
SET(PLUGIN_NAME "ModflowModel")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE ( ${CMakeModules}/FindOSG.cmake)

# ------------ Find GDAL Library ----------------------
INCLUDE( ${CMakeModules}/FindGDAL.cmake)

# Set the compile guard.
set(COMPILE_GUARD "_COMPILING_MODFLOW_MODEL")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" ${Boost_INCLUDE_DIR} ${OSG_INC_DIR} ${GDAL_INC_DIR} )

# Set the sources to compile.
set (SOURCES
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

# ---------- Create a Shared Library
ADD_LIBRARY(${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN( ${PLUGIN_NAME} )

# Link the Library with Cadkit Libraries.
LINK_CADKIT( ${PLUGIN_NAME} Usul OsgTools XmlTree SerializeXML )

# Link the Library	
TARGET_LINK_LIBRARIES( ${PLUGIN_NAME} 
  ${OPENTHREADS_LIB}
  ${OSG_LIB}
  ${GDAL_LIB}
)
