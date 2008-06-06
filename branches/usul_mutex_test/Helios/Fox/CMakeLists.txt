# ccmake should be run from the Helios directory.

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")
INCLUDE(${CMakeModules}/FindFoxToolkit.cmake)

SUBDIRS (
${PROJECT_SOURCE_DIR}/../../FoxTools
${PROJECT_SOURCE_DIR}/Core
${PROJECT_SOURCE_DIR}/Views
${PROJECT_SOURCE_DIR}/Application
# ---- Helios Fox PlugIns -------
${PROJECT_SOURCE_DIR}/Plugins/Animate
${PROJECT_SOURCE_DIR}/Plugins/ClipPlanes
${PROJECT_SOURCE_DIR}/Plugins/Convolve
${PROJECT_SOURCE_DIR}/Plugins/EdgeDetect
${PROJECT_SOURCE_DIR}/Plugins/EditPolygon
${PROJECT_SOURCE_DIR}/Plugins/FoxDevice
${PROJECT_SOURCE_DIR}/Plugins/Histogram
${PROJECT_SOURCE_DIR}/Plugins/ImageDelegate
${PROJECT_SOURCE_DIR}/Plugins/ImageModel
${PROJECT_SOURCE_DIR}/Plugins/MaterialEditor
${PROJECT_SOURCE_DIR}/Plugins/PhaseFieldDelegate
${PROJECT_SOURCE_DIR}/Plugins/SceneDelegate
${PROJECT_SOURCE_DIR}/Plugins/SceneTree
${PROJECT_SOURCE_DIR}/Plugins/Seek
#  ${PROJECT_SOURCE_DIR}/Plugins/SelectionSTL
${PROJECT_SOURCE_DIR}/Plugins/Threshold
${PROJECT_SOURCE_DIR}/Plugins/TransparencyControl
${PROJECT_SOURCE_DIR}/Plugins/TriangleDelegate
${PROJECT_SOURCE_DIR}/Plugins/VolumeDelegate
#  ${PROJECT_SOURCE_DIR}/Plugins/VolumeToTriangles
)
