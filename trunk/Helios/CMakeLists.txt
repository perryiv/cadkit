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
${PROJECT_SOURCE_DIR}/../Experimental/PathAnimation
${PROJECT_SOURCE_DIR}/../Experimental/ModelPresentation/ModelPresentation
)

#SET(BUILD_MINERVA ON)

#IF(BUILD_MINERVA)
SUBDIRS (
${CADKIT_INC_DIR}/Minerva/Core
${CADKIT_INC_DIR}/Minerva/DataSources/PG
${CADKIT_INC_DIR}/Minerva/Layers/PostGIS
${CADKIT_INC_DIR}/Minerva/Document
${CADKIT_INC_DIR}/Minerva/Plugins/Document
${CADKIT_INC_DIR}/Minerva/Plugins/ProjectionManager
${CADKIT_INC_DIR}/Minerva/Plugins/ResampleLine
${CADKIT_INC_DIR}/Minerva/Plugins/TriangulateCGAL
)
#ENDIF(BUILD_MINERVA)


# Include the QT Cmake files.
SUBDIRS (	${PROJECT_SOURCE_DIR}/Qt )



# ----------------------------------------------------
# Optional plugins.  TODO: Make options for these.
# ----------------------------------------------------
#
#  ${PROJECT_SOURCE_DIR}/Plugins/AVIWriter
#  ${PROJECT_SOURCE_DIR}/Plugins/CalcPerVertexNormals
#  ${PROJECT_SOURCE_DIR}/Plugins/CalculateEnergy
#  ${PROJECT_SOURCE_DIR}/Plugins/DirectXJoystick
#  ${PROJECT_SOURCE_DIR}/Plugins/Grayscale
#  ${PROJECT_SOURCE_DIR}/Plugins/GroupPrimitives
#  ${PROJECT_SOURCE_DIR}/Plugins/HideShowNormals
#  ${PROJECT_SOURCE_DIR}/Plugins/ImageMorphology
#  ${PROJECT_SOURCE_DIR}/Plugins/InvertImage
#  ${PROJECT_SOURCE_DIR}/Plugins/MoleculeModel
#  ${PROJECT_SOURCE_DIR}/Plugins/MOVWriter
#  ${PROJECT_SOURCE_DIR}/Plugins/NewPrimitives
#  ${PROJECT_SOURCE_DIR}/Plugins/ParadisModel
#  ${PROJECT_SOURCE_DIR}/Plugins/PhaseFieldModel
#  ${PROJECT_SOURCE_DIR}/Plugins/ScaleImage
#  ${PROJECT_SOURCE_DIR}/Plugins/Triangulate
#  ${PROJECT_SOURCE_DIR}/Plugins/VoxelModel
# ---------- Dislocation Plugins---------------
#  ${PROJECT_SOURCE_DIR}/../DislocationModel
#  ${PROJECT_SOURCE_DIR}/../DislocationDelegate


