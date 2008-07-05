project (QtSceneDelegate)

# Set the name.  Needed for the macros to set up plugin properties.
SET(PLUGIN_NAME "QtSceneDelegate")

SET(CMakeModules "${CADKIT_INC_DIR}/CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)

ADD_DEFINITIONS ("-D_COMPILING_SCENE_DELEGATE")



# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     "${OSG_INC_DIR}" 
                     "${PROJECT_SOURCE_DIR}/../" 
                      ${QT_INCLUDE_DIR} )


set (SOURCES
  SceneDelegateComponent.cpp
)


# ---------- Create a Shared Library
ADD_LIBRARY(QtSceneDelegate SHARED ${SOURCES} )

CADKIT_PLUGIN()

# Link the Library with Cadkit Libraries.
LINK_CADKIT( ${PLUGIN_NAME} Usul OsgTools HeliosQtViewsOSG )


# Link the Library	
TARGET_LINK_LIBRARIES( ${PLUGIN_NAME}
  ${OPENTHREADS_LIB}
	${OSG_LIB}
)

INSTALL(TARGETS ${PLUGIN_NAME}
  LIBRARY DESTINATION bin
)
