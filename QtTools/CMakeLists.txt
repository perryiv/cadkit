
PROJECT(QtTools)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${OSG_INC_DIR} 
                     ${CADKIT_INC_DIR} 
                     ${QT_INCLUDE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR} )


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
QT4_WRAP_CPP(MOC_SRCS ${MOC_HDRS} ${UIS_HDRS} )

# Create a Shared Library
ADD_LIBRARY( QtTools SHARED ${SOURCES} ${MOC_SRCS} )

# Link the Library	
LINK_CADKIT( QtTools Usul )

TARGET_LINK_LIBRARIES( QtTools
   ${QT_LIBRARIES}
)

ADD_CUSTOM_COMMAND(
        SOURCE QtTools
        COMMAND ${CCOMMAND_COMMAND}
        ARGS cp "${CMAKE_CURRENT_BINARY_DIR}/ui_AnimationControl.h" "${PROJECT_SOURCE_DIR}/ui_AnimationControl.h"
        TARGET QtTools )

SET( IconDir ${EXECUTABLE_OUTPUT_PATH}/icons )
FILE(GLOB PngFiles "${PROJECT_SOURCE_DIR}/Icons/*.png" )

SET ( IconFiles ${PngFiles} )

MAKE_DIRECTORY( ${IconDir} )
FOREACH(FILE ${IconFiles})
ADD_CUSTOM_COMMAND(
        SOURCE QtTools
        COMMAND ${CCOMMAND_COMMAND}
        ARGS cp ${FILE}  ${IconDir}
        TARGET QtTools )
ENDFOREACH(FILE ${IconFiles})

INSTALL(TARGETS QtTools
  LIBRARY DESTINATION bin
)
