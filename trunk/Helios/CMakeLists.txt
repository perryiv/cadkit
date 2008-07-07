# This file should be at CadKit/Helios/CMakeLists.txt

cmake_minimum_required(VERSION "2.4")

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

# Set variables needed below...
SET ( CADKIT_INC_DIR ${PROJECT_SOURCE_DIR}/../ )
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


SUBDIRS(

# include base libraries.
${PROJECT_SOURCE_DIR}/../Usul 
${PROJECT_SOURCE_DIR}/../Threads/OpenThreads
${PROJECT_SOURCE_DIR}/../OsgTools 
${PROJECT_SOURCE_DIR}/../XmlTree 
${PROJECT_SOURCE_DIR}/../Serialize/XML
${PROJECT_SOURCE_DIR}/../MenuKit

# include plugins.
${PROJECT_SOURCE_DIR}/Plugins/SceneModel
${PROJECT_SOURCE_DIR}/Plugins/TriangleModel
${PROJECT_SOURCE_DIR}/Plugins/VolumeModel
${PROJECT_SOURCE_DIR}/Plugins/PathAnimation
${PROJECT_SOURCE_DIR}/Plugins/ModelPresentation/ModelPresentationLib
${PROJECT_SOURCE_DIR}/Plugins/ModelPresentation/ModelPresentation
${PROJECT_SOURCE_DIR}/../Experimental/WRF/WrfModel
${PROJECT_SOURCE_DIR}/../Experimental/Flash/FlashModel
${CADKIT_INC_DIR}/Helios/Plugins/MeasureTool
${CADKIT_INC_DIR}/Helios/Plugins/CenterGeometry
${CADKIT_INC_DIR}/Minerva/Plugins/EsriShapeFile
${CADKIT_INC_DIR}/Nimue/Plugins/ModflowModel
)

OPTION ( BUILD_MINERVA "Build Minerva libraries."  ON)

IF(BUILD_MINERVA)
  SUBDIRS (
    ${CADKIT_INC_DIR}/Minerva/Core
    ${CADKIT_INC_DIR}/Minerva/DataSources/PG
    ${CADKIT_INC_DIR}/Minerva/Layers/PostGIS
    ${CADKIT_INC_DIR}/Minerva/Layers/GDAL
    ${CADKIT_INC_DIR}/Minerva/Layers/Kml
    ${CADKIT_INC_DIR}/Minerva/Document
    ${CADKIT_INC_DIR}/Minerva/Plugins/Document
    ${CADKIT_INC_DIR}/Minerva/Plugins/ProjectionManager
    ${CADKIT_INC_DIR}/Minerva/Plugins/TriangulateCGAL
    ${CADKIT_INC_DIR}/Minerva/Plugins/LayerManager
    ${CADKIT_INC_DIR}/Minerva/Plugins/WmsLayerQt
    ${CADKIT_INC_DIR}/Minerva/Plugins/OssimLayerQt
    ${CADKIT_INC_DIR}/Minerva/Plugins/PostGISLayerQt
    ${CADKIT_INC_DIR}/Minerva/Plugins/GDALReadImage
    ${CADKIT_INC_DIR}/Minerva/Application
    )
ENDIF(BUILD_MINERVA)


# Include the QT Cmake files.
SUBDIRS (
  ${PROJECT_SOURCE_DIR}/Qt 
  ${CADKIT_INC_DIR}/Helios/Qt/Plugins/PasswordPrompt
)


# Install 3rd party dependencies.
INSTALL ( FILES ${OSG_LIB} 
                ${OSG_DB_LIB} 
                ${OSG_GA_LIB} 
                ${OSG_TEXT_LIB}
                ${OSG_UTIL_LIB}
                ${OSG_MANIPULATOR}
                ${OPENTHREADS_LIB}
  DESTINATION bin
)
