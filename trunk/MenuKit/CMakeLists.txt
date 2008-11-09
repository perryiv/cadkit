
#########################################################
#
#  CMakeLists for MenuKit.
#
#########################################################

PROJECT(MenuKit)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

ADD_DEFINITIONS("-D_COMPILING_MENUKIT")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( 
			${Boost_INCLUDE_DIR}
		     ${CADKIT_INC_DIR}
		     ${OSG_INC_DIR} 
		     )


#List the Sources
SET (SOURCES
./Behavior.cpp
./Item.cpp
./Visitor.cpp
./Button.cpp
./Errors.cpp
./Menu.cpp
./FlagsVisitor.cpp
./UpdateVisitor.cpp
./RadioButton.cpp
./ToggleButton.cpp
./Separator.cpp
./CommandVisitor.cpp
./MenuCommands.cpp
)

# Create a Shared Library
ADD_LIBRARY( MenuKit SHARED ${SOURCES} )

# Add a library
CADKIT_ADD_LIBRARY ( MenuKit )

# Link the Library	
LINK_CADKIT( MenuKit Usul )


