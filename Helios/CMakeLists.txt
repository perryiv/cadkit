# This file should be at CadKit/Helios/CMakeLists.txt

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake )

# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)

INCLUDE(${CMakeModules}/FindOSG.cmake)


# Options for different versions of Helios.
OPTION(BUILD_HELIOS_QT "Build QT version of Helios" ON)
OPTION(BUILD_HELIOS_FOX "Build Fox Toolkit version of Helios" OFF )


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
)


# include base plugins
SUBDIRS(
${PROJECT_SOURCE_DIR}/Animate
${PROJECT_SOURCE_DIR}/Plugins/Manager
${PROJECT_SOURCE_DIR}/Plugins/About
${PROJECT_SOURCE_DIR}/Plugins/SceneModel
${PROJECT_SOURCE_DIR}/Plugins/SceneStatsOSG
${PROJECT_SOURCE_DIR}/Plugins/TriangleModel
${PROJECT_SOURCE_DIR}/Plugins/VolumeModel
)

#Check to see if we are building QT version of Helios
IF( BUILD_HELIOS_QT )
	SUBDIRS (	${PROJECT_SOURCE_DIR}/Qt )
ENDIF ( BUILD_HELIOS_QT )

#Check to see if we are building the Fox Toolkit version of Helios
IF( BUILD_HELIOS_FOX )
 SUBDIRS ( ${PROJECT_SOURCE_DIR}/Fox )
ENDIF ( BUILD_HELIOS_FOX )




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


