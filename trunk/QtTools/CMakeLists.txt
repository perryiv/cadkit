
#########################################################
#
#  CMakeLists for QtTools.
#
#########################################################

PROJECT(QtTools)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} 
					 ${OSG_INC_DIR} 
                     ${CADKIT_INC_DIR} 
                     ${QT_INCLUDE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR} )


LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# Add definitions
ADD_DEFINITIONS ("-D_COMPILING_QT_TOOLS")
					 
# List the Sources.
SET (SOURCES
  Action.cpp
  AnimationControl.cpp
  ColorButton.cpp
  FileDialog.cpp
  Image.cpp
  Menu.cpp
  Move.cpp
  PropertyBrowser.cpp
  Question.cpp
  SettingsGroupScope.cpp
  Show.cpp
  StringsView.cpp
  TreeControl.cpp
  TreeModel.cpp
  TreeNode.cpp
)

# These need to be moc'd.
SET(MOC_HDRS
  Action.h
  AnimationControl.h
  ColorButton.h
  Menu.h
  PropertyBrowser.h
  TreeControl.h
  TreeNode.h
)

SET(UIS
    AnimationControl.ui
)

QT4_WRAP_UI(UIS_HDRS ${UIS})

# Create the moc files.
QT4_WRAP_CPP(MOC_SRCS ${MOC_HDRS} )

# Create a Shared Library
ADD_LIBRARY( QtTools SHARED ${SOURCES} ${MOC_SRCS} ${UIS} )

# Add a library
CADKIT_ADD_LIBRARY ( QtTools )

# Link the Library	
LINK_CADKIT( QtTools Usul MenuKit )

TARGET_LINK_LIBRARIES( QtTools
   ${QT_LIBRARIES}
)

# Copy the generated header file to location client libraries can see.
CADKIT_COPY(QtTools "${CMAKE_CURRENT_BINARY_DIR}/ui_AnimationControl.h" "${PROJECT_SOURCE_DIR}/ui_AnimationControl.h" )

SET( IconDir ${EXECUTABLE_OUTPUT_PATH}/icons/ )
FILE(GLOB PngFiles "${PROJECT_SOURCE_DIR}/Icons/*.png" )

SET ( IconFiles ${PngFiles} )

MAKE_DIRECTORY( ${IconDir} )
FOREACH(FILE ${IconFiles})
	CADKIT_COPY(QtTools ${FILE}  ${IconDir} )
ENDFOREACH(FILE ${IconFiles})

INSTALL(TARGETS QtTools
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin
  ARCHIVE DESTINATION bin
)
