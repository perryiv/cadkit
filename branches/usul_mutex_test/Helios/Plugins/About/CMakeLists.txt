project (About)

SET(PLUGIN_NAME "About")

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.plugin.cmake)


set(COMPILE_GUARD "_COMPILING_ABOUT")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} "${PROJECT_SOURCE_DIR}/../" )


set (SOURCES
	AboutFactory.cpp
	AboutComponent.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(${PLUGIN_NAME} SHARED ${SOURCES} )

CADKIT_PLUGIN()

# Link the Library	
LINK_CADKIT( ${PLUGIN_NAME} Usul )

TARGET_LINK_LIBRARIES( ${PLUGIN_NAME} 	${OPENGL_LIBRARIES} )

