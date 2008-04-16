
# Set variables needed below...
SET ( CADKIT_INC_DIR ${PROJECT_SOURCE_DIR}/../ )
SET ( CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules" )

# Path to look for .cmake files.
SET(CMAKE_MODULE_PATH ${CMakeModules} )

INCLUDE ( ${CMakeModules}/Cadkit.cmake )

# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)

INCLUDE( FindOSG )

INCLUDE ( ${CMakeModules}/FindBoost.cmake)
# Note that the Boost Include path will automatically get added


SET (GENERIC_NURBS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../GenericNurbs)


IF (APPLE)
option(OSX_DWARF_DEBUG "Use Dwarf Debug Style" ON)
IF (OSX_DWARF_DEBUG )
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-2")
ENDIF(OSX_DWARF_DEBUG )
ENDIF (APPLE)


# Build these projects.
SUBDIRS(
${CADKIT_INC_DIR}/Usul 
${CADKIT_INC_DIR}/Threads/OpenThreads
${CADKIT_INC_DIR}/OsgTools 
${CADKIT_INC_DIR}/XmlTree 
${CADKIT_INC_DIR}/Serialize/XML
${CADKIT_INC_DIR}/MenuKit
${CADKIT_INC_DIR}/Helios/Plugins/PathAnimation
${CADKIT_INC_DIR}/Experimental/MeasureTool
${CADKIT_INC_DIR}/Helios/Qt
${CADKIT_INC_DIR}/Helios/Qt/Plugins/PasswordPrompt
${CADKIT_INC_DIR}/Minerva/Application
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
${CADKIT_INC_DIR}/Minerva/Plugins/PostGISLayerQt
)
