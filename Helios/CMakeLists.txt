# This file should be at CadKit/Helios/CMakeLists.txt

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")

# Path to look for .cmake files.
SET(CMAKE_MODULE_PATH ${CMakeModules} )

INCLUDE ( ${CMakeModules}/Cadkit.cmake )

# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)

INCLUDE( FindOSG )

INCLUDE ( ${CMakeModules}/FindBoost.cmake)
# Note that the Boost Include path will automatically get added


SET (HELIOS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../ )
SET (GENERIC_NURBS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../GenericNurbs)


IF (APPLE)
option(OSX_DWARF_DEBUG "Use Dwarf Debug Style" ON)
IF (OSX_DWARF_DEBUG )
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-2")
ENDIF(OSX_DWARF_DEBUG )
ENDIF (APPLE)


# include base libraries.
SUBDIRS(
${PROJECT_SOURCE_DIR}/../Usul 
${PROJECT_SOURCE_DIR}/../Threads/OpenThreads
${PROJECT_SOURCE_DIR}/../OsgTools 
${PROJECT_SOURCE_DIR}/../XmlTree 
${PROJECT_SOURCE_DIR}/../Serialize/XML
${PROJECT_SOURCE_DIR}/../MenuKit
${PROJECT_SOURCE_DIR}/Plugins/SceneModel
${PROJECT_SOURCE_DIR}/Plugins/TriangleModel
${PROJECT_SOURCE_DIR}/Plugins/VolumeModel
${PROJECT_SOURCE_DIR}/Plugins/PathAnimation
${PROJECT_SOURCE_DIR}/../Experimental/ModelPresentation/ModelPresentation
${PROJECT_SOURCE_DIR}/../Experimental/WRF/WrfModel
${PROJECT_SOURCE_DIR}/../Experimental/WRF/WrfDelegate
${CADKIT_INC_DIR}/Experimental/MeasureTool
)

#SET(BUILD_MINERVA ON)

#IF(BUILD_MINERVA)
SUBDIRS (
${CADKIT_INC_DIR}/Minerva/Core
${CADKIT_INC_DIR}/Minerva/DataSources/PG
${CADKIT_INC_DIR}/Minerva/Layers/PostGIS
${CADKIT_INC_DIR}/Minerva/Layers/GDAL
${CADKIT_INC_DIR}/Minerva/Layers/Kml
${CADKIT_INC_DIR}/Minerva/Document
${CADKIT_INC_DIR}/Minerva/Plugins/Document
${CADKIT_INC_DIR}/Minerva/Plugins/ProjectionManager
${CADKIT_INC_DIR}/Minerva/Plugins/ResampleLine
${CADKIT_INC_DIR}/Minerva/Plugins/TriangulateCGAL
${CADKIT_INC_DIR}/Minerva/Plugins/LayerManager
${CADKIT_INC_DIR}/Minerva/Plugins/WmsLayerQt
${CADKIT_INC_DIR}/Minerva/Plugins/OssimLayerQt
)
#ENDIF(BUILD_MINERVA)


# Include the QT Cmake files.
SUBDIRS (	${PROJECT_SOURCE_DIR}/Qt )


