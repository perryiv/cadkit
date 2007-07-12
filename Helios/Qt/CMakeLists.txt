# ccmake should be run from the Helios directory.


SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")

set(QT_USE_OPENGL TRUE)

FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})

# the next line sets up include and link directories and defines some variables that we will use.
# you can modify the behavior by setting some variables, e.g.
INCLUDE(${QT_USE_FILE})

SUBDIRS (
${PROJECT_SOURCE_DIR}/Qt/Views/OSG
${PROJECT_SOURCE_DIR}/Qt/Commands
${PROJECT_SOURCE_DIR}/Qt/Tools
${PROJECT_SOURCE_DIR}/Qt/Core
${PROJECT_SOURCE_DIR}/Qt/Application
${PROJECT_SOURCE_DIR}/Qt/Plugins/SceneDelegate
)
