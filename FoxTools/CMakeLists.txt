
PROJECT(FoxTools)

SET (CMAKE_BUILD_TYPE Debug)
SET (CMAKE_SKIP_RPATH OFF)

#setup output Directories
SET (LIBRARY_OUTPUT_PATH
${PROJECT_SOURCE_DIR}/../Bin/${CMAKE_BUILD_TYPE}
CACHE PATH
"Single Directory for all Libraries"
)

#setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
${PROJECT_SOURCE_DIR}/../Bin/${CMAKE_BUILD_TYPE}
CACHE PATH
"SINGLE Directory for all Executables."
)


# ------------ Find FoxToolkit Library and include Dir ----------------------
FIND_LIBRARY(FOXTOOLKIT_LIB FOX-1.4
	/usr/local/lib
	/usr/lib
)

FIND_PATH(FOX_INCLUDE fx.h
    /usr/local/include/fox-1.4
    /usr/include/fox-1.4
)

# ------------ Find Usul Library ----------------------
FIND_LIBRARY (USUL_LIBRARY Usul
	${PROJECT_SOURCE_DIR}/../Bin/${CMAKE_BUILD_TYPE}
	/usr/local/lib
)

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../" )
INCLUDE_DIRECTORIES( "${FOX_INCLUDE}" )

#List the Sources
SET (SOURCES
	Adaptors/Wait.cpp
	App/Application.cpp
	Builder/Reader.cpp
	Cursors/Factory.cpp
	Dialogs/FileSelection.cpp
	Dialogs/Message.cpp
	Functions/App.cpp
	Functions/Check.cpp
	Functions/Delete.cpp
	Functions/Enable.cpp
	Functions/GLVisual.cpp
	Functions/MainWindow.cpp
	Functions/Mouse.cpp
	Icons/Factory.cpp
	Items/Command.cpp
	Items/Item.cpp
	Menu/Bar.cpp
	Menu/Button.cpp
	Menu/CheckButton.cpp
	Menu/Group.cpp
	Menu/RadioButton.cpp
	Menu/RecentFiles.cpp
	Menu/Separator.cpp
	Predicates/HasText.cpp
	Predicates/IsChildNumber.cpp
	Predicates/IsMemberOf.cpp
	Registry/Registry.cpp
	ToolBar/Bar.cpp
	ToolBar/Button.cpp
	ToolBar/ComboBox.cpp
	ToolBar/DockSite.cpp
	ToolBar/Factory.cpp
	ToolBar/Separator.cpp
	ToolBar/Slider.cpp
	ToolBar/ToggleButton.cpp
	Widgets/SliderTextField.cpp
	Windows/MdiChildWindow.cpp
)

# Create a Shared Library
ADD_LIBRARY(FoxTools SHARED ${SOURCES} )

# Link the Library	
TARGET_LINK_LIBRARIES(FoxTools ${USUL_LIBRARY} ${FOXTOOLKIT_LIB})

    
   