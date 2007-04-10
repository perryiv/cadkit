# This file should be at CadKit/Helios/CMakeLists.txt

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")

# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)

INCLUDE(${CMakeModules}/FindOSG.cmake)
INCLUDE(${CMakeModules}/FindFoxToolkit.cmake)

# Pick up IF the user has SET an environment variable to point to the 
# Helios Lib and includes. If these are not SET
# then they will simply be ignored.
SET(HELIOS_BIN_DIR "$ENV{HELIOS_BIN_DIR}")
IF ( NOT HELIOS_BIN_DIR )
SET(HELIOS_BIN_DIR ${PROJECT_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE} )
ENDIF ( NOT HELIOS_BIN_DIR )

SET (HELIOS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../ )
SET (GENERIC_NURBS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../GenericNurbs)


#Set the Usul Bin Directory to the same as Helios
SET (USUL_BIN_DIR ${HELIOS_BIN_DIR})
#Set the OsgTools Bin Directory to the same as Helios
SET (OSGTOOLS_BIN_DIR ${HELIOS_BIN_DIR})

#setup output Directories
SET (LIBRARY_OUTPUT_PATH
	${HELIOS_BIN_DIR}
	CACHE PATH
	"Single Directory for all Libraries"
)

#setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
	${HELIOS_BIN_DIR}
	CACHE PATH
	"SINGLE Directory for all Executables."
)


IF (APPLE)
option(OSX_DWARF_DEBUG "Use Dwarf Debug Style" ON)
IF (OSX_DWARF_DEBUG )
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-2")
ENDIF(OSX_DWARF_DEBUG )
ENDIF (APPLE)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
ADD_DEFINITIONS("-D_DEBUG")
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

SUBDIRS(
${PROJECT_SOURCE_DIR}/../Usul 
${PROJECT_SOURCE_DIR}/../Threads
#  ${PROJECT_SOURCE_DIR}/../Images
${PROJECT_SOURCE_DIR}/../FoxTools
${PROJECT_SOURCE_DIR}/../OsgTools 
${PROJECT_SOURCE_DIR}/Fox/Core
${PROJECT_SOURCE_DIR}/Fox/Views
${PROJECT_SOURCE_DIR}/Fox/Application
# ---- Helios Fox PlugIns -------
${PROJECT_SOURCE_DIR}/Fox/Plugins/Animate
${PROJECT_SOURCE_DIR}/Fox/Plugins/ClipPlanes
${PROJECT_SOURCE_DIR}/Fox/Plugins/Convolve
${PROJECT_SOURCE_DIR}/Fox/Plugins/EdgeDetect
${PROJECT_SOURCE_DIR}/Fox/Plugins/EditPolygon
${PROJECT_SOURCE_DIR}/Fox/Plugins/FoxDevice
${PROJECT_SOURCE_DIR}/Fox/Plugins/Histogram
${PROJECT_SOURCE_DIR}/Fox/Plugins/ImageDelegate
${PROJECT_SOURCE_DIR}/Fox/Plugins/ImageModel
${PROJECT_SOURCE_DIR}/Fox/Plugins/MaterialEditor
${PROJECT_SOURCE_DIR}/Fox/Plugins/PhaseFieldDelegate
${PROJECT_SOURCE_DIR}/Fox/Plugins/SceneDelegate
${PROJECT_SOURCE_DIR}/Fox/Plugins/SceneTree
${PROJECT_SOURCE_DIR}/Fox/Plugins/Seek
#  ${PROJECT_SOURCE_DIR}/Fox/Plugins/SelectionSTL
${PROJECT_SOURCE_DIR}/Fox/Plugins/Threshold
${PROJECT_SOURCE_DIR}/Fox/Plugins/TransparencyControl
${PROJECT_SOURCE_DIR}/Fox/Plugins/TriangleDelegate
${PROJECT_SOURCE_DIR}/Fox/Plugins/VolumeDelegate
#  ${PROJECT_SOURCE_DIR}/Fox/Plugins/VolumeToTriangles
# ------- Helios PlugIns ----------------
${PROJECT_SOURCE_DIR}/Animate
${PROJECT_SOURCE_DIR}/Plugins/About
#  ${PROJECT_SOURCE_DIR}/Plugins/AVIWriter
#  ${PROJECT_SOURCE_DIR}/Plugins/CalcPerVertexNormals
#  ${PROJECT_SOURCE_DIR}/Plugins/CalculateEnergy
#  ${PROJECT_SOURCE_DIR}/Plugins/DirectXJoystick
  ${PROJECT_SOURCE_DIR}/Plugins/Grayscale
#  ${PROJECT_SOURCE_DIR}/Plugins/GroupPrimitives
#  ${PROJECT_SOURCE_DIR}/Plugins/HideShowNormals
${PROJECT_SOURCE_DIR}/Plugins/ImageMorphology
${PROJECT_SOURCE_DIR}/Plugins/InvertImage
#  ${PROJECT_SOURCE_DIR}/Plugins/MoleculeModel
#  ${PROJECT_SOURCE_DIR}/Plugins/MOVWriter
#  ${PROJECT_SOURCE_DIR}/Plugins/NewPrimitives
#  ${PROJECT_SOURCE_DIR}/Plugins/ParadisModel
#  ${PROJECT_SOURCE_DIR}/Plugins/PhaseFieldModel
${PROJECT_SOURCE_DIR}/Plugins/ScaleImage
${PROJECT_SOURCE_DIR}/Plugins/SceneModel
${PROJECT_SOURCE_DIR}/Plugins/SceneStatsOSG
${PROJECT_SOURCE_DIR}/Plugins/TriangleModel
#  ${PROJECT_SOURCE_DIR}/Plugins/Triangulate
${PROJECT_SOURCE_DIR}/Plugins/VolumeModel
#  ${PROJECT_SOURCE_DIR}/Plugins/VoxelModel
# ---------- Dislocation Plugins---------------
#${PROJECT_SOURCE_DIR}/../DislocationModel
#${PROJECT_SOURCE_DIR}/../DislocationDelegate
)

