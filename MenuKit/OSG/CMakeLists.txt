
PROJECT(MenuKit_OSG)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

INCLUDE ( ${CMakeModules}/FindOSG.cmake)

# setup output Directories
SET (LIBRARY_OUTPUT_PATH
	${CADKIT_BIN_DIR}
	CACHE PATH
	"Single Directory for all Libraries"
)

# setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
	${CADKIT_BIN_DIR}
	CACHE PATH
	"Single Directory for all Executables."
)


# Find needed libraries.
INCLUDE(${CMakeModules}/FindOSG.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

ADD_DEFINITIONS("-D_COMPILING_MENUKIT_OSG")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     )


#List the Sources
SET (SOURCES
./BaseMenu.cpp
./Menu.cpp
./Skin.cpp
./Font.cpp
./GFX.cpp
./BackgroundBox.cpp

)

# Create a Shared Library
ADD_LIBRARY( MenuKit_OSG SHARED ${SOURCES} )
CADKIT_ADD_LIBRARY ( MenuKit_OSG )

# Link the Library	
LINK_CADKIT( MenuKit_OSG MenuKit Usul OsgTools )

TARGET_LINK_LIBRARIES(  MenuKit_OSG
	${OSG_LIB}
	${OSG_TEXT_LIB}
)
