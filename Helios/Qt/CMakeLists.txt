# ccmake should be run from the Helios directory.


SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")

set(QT_MAX_VERSION "4.9.9")
set(QT_USE_OPENGL TRUE)

FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})

# the next line sets up include and link directories and defines some variables that we will use.
# you can modify the behavior by setting some variables, e.g.
INCLUDE(${QT_USE_FILE})

SUBDIRS (
${CADKIT_INC_DIR}/Experimental/QtTools
${CADKIT_INC_DIR}/Helios/Qt/Views/OSG
${CADKIT_INC_DIR}/Helios/Qt/Commands
${CADKIT_INC_DIR}/Helios/Qt/Tools
${CADKIT_INC_DIR}/Helios/Qt/Core
${CADKIT_INC_DIR}/Helios/Qt/Application
${CADKIT_INC_DIR}/Helios/Qt/Plugins/SceneDelegate
)
