# ccmake should be run from the Helios directory.


SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")

# the next line sets up include and link directories and defines some variables that we will use.
# you can modify the behavior by setting some variables, e.g.
INCLUDE(${QT_USE_FILE})

SUBDIRS (
${CADKIT_INC_DIR}/QtTools
${CADKIT_INC_DIR}/Helios/Qt/Views/OSG
${CADKIT_INC_DIR}/Helios/Qt/Commands
${CADKIT_INC_DIR}/Helios/Qt/Core
${CADKIT_INC_DIR}/Helios/Qt/Application
${CADKIT_INC_DIR}/Helios/Qt/Plugins/AnimationControl
${CADKIT_INC_DIR}/Helios/Qt/Plugins/ClipPlanes
${CADKIT_INC_DIR}/Helios/Qt/Plugins/SceneDelegate
${CADKIT_INC_DIR}/Helios/Qt/Plugins/SnapShot
)


