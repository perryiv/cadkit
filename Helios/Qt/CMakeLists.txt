# ccmake should be run from the Helios directory.


SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")


SUBDIRS (
${PROJECT_SOURCE_DIR}/Qt/Core
${PROJECT_SOURCE_DIR}/Qt/Application
)
