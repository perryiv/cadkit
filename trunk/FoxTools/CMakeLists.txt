
PROJECT(FoxTools)

INCLUDE(${PROJECT_SOURCE_DIR}/../Helios/FindFoxToolkit.cmake)

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../" "${FOXTOOLKIT_INCLUDE}" )

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
TARGET_LINK_LIBRARIES(FoxTools 
  Usul 
  ${FOXTOOLKIT_LIB}
)

    
   
