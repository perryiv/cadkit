
PROJECT(MenuKit)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

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

ADD_DEFINITIONS("-D_COMPILING_VRJ_GA")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     )


#List the Sources
SET (SOURCES
./Behavior.cpp
./Item.cpp
./Referenced.cpp
./Visitor.cpp
./Button.cpp
./MessageVisitor.cpp
./SampleMenu.cpp
./Errors.cpp
./StreamVisitor.cpp
./CloseVisitor.cpp
./Menu.cpp
./FlagsVisitor.cpp
./Callback.cpp

)

# Create a Shared Library
ADD_LIBRARY( MenuKit SHARED ${SOURCES} )

# Link the Library	
LINK_CADKIT( MenuKit Usul OsgTools )


