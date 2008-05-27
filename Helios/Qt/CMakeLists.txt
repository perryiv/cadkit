# ccmake should be run from the Helios directory.


SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")

set(QT_MAX_VERSION "4.9.9")
set(QT_USE_OPENGL TRUE)

FIND_PACKAGE(Qt4 REQUIRED)

# Install 3rd party dependencies.
INSTALL ( FILES ${QT_QTCORE_LIBRARY_RELEASE}
                ${QT_QTGUI_LIBRARY_RELEASE}
                ${QT_QTOPENGL_LIBRARY_RELEASE}
  DESTINATION bin
)

ADD_DEFINITIONS(${QT_DEFINITIONS})

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


