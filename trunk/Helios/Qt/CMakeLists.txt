# ccmake should be run from the Helios directory.


SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")


SUBDIRS (
${PROJECT_SOURCE_DIR}/Qt/Views/OSG
${PROJECT_SOURCE_DIR}/Qt/Commands
${PROJECT_SOURCE_DIR}/Qt/Tools
${PROJECT_SOURCE_DIR}/Qt/Core
${PROJECT_SOURCE_DIR}/Qt/Application
${PROJECT_SOURCE_DIR}/Qt/Plugins/SceneModel
)
