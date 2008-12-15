
#########################################################
#
#  CMakeLists for Minerva Qt Application.
#
#########################################################

# Set the target name.
SET ( TARGET_NAME MinervaQt )

SET(CMakeModules "${CADKIT_INC_DIR}/CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

INCLUDE ( ${CMakeModules}/FindOSG.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )


# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES(
        ${CADKIT_INC_DIR}
       "${CADKIT_INC_DIR}/Experimental"
        )

#List the Sources
SET (SOURCES
    main.cpp
)

# If we are building on Windows...
IF(WIN32)
	# Add the resource file for the icon.
    SET( SOURCES ${SOURCES} Resources.rc )
	
	# Add Unicode preprocessor definition.
	ADD_DEFINITIONS("-DUNICODE")
ENDIF(WIN32)

# Adds includes and libraries for QT.
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS( ${QT_DEFINITIONS} )

# Create the executabe.
ADD_EXECUTABLE( ${TARGET_NAME} ${SOURCES} )

# Note: Set targe properties have to be called after ADD_EXECUTABLE.

# Settings so the console does not appear when running (See: http://www.cmake.org/Wiki/VSConfigSpecificSettings).
IF(WIN32)	
	SET_TARGET_PROPERTIES( ${TARGET_NAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS" )
	SET_TARGET_PROPERTIES( ${TARGET_NAME} PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS" )
	SET_TARGET_PROPERTIES( ${TARGET_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS" )
	SET_TARGET_PROPERTIES( ${TARGET_NAME} PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS" )
ENDIF(WIN32)

SET ( TARGET_OUTPUT_NAME "Minerva" )
SET_TARGET_PROPERTIES( ${TARGET_NAME} PROPERTIES OUTPUT_NAME ${TARGET_OUTPUT_NAME} )

# Add the target label.
SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES PROJECT_LABEL "App ${TARGET_NAME}")

# Add the debug postfix.
SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

# Make an application bundle if we are on OS X
IF(APPLE)
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES MACOSX_BUNDLE TRUE )
ENDIF(APPLE)

# Link the Library	
LINK_CADKIT( ${TARGET_NAME} Usul OpenThreadsEx HeliosQtCore )

TARGET_LINK_LIBRARIES( ${TARGET_NAME}
   ${QT_LIBRARIES}
   ${QT_QTMAIN_LIBRARY}
   ${OPENTHREADS_LIB}
)

# Find icons to copy.
SET( IconDir ${EXECUTABLE_OUTPUT_PATH}/icons )
FILE(GLOB PngFiles "${CMAKE_CURRENT_SOURCE_DIR}/../Images/*.png" )
FILE(GLOB JpgFiles "${CMAKE_CURRENT_SOURCE_DIR}/../Images/*.jpg" )

SET ( IconFiles ${PngFiles} ${JpgFiles} )

# Install the icons.
INSTALL(
    FILES ${IconFiles}
    DESTINATION bin/icons
)

MAKE_DIRECTORY( ${IconDir} )
FOREACH(FILE ${IconFiles})
	CADKIT_COPY( ${TARGET_NAME} ${FILE}  ${IconDir} )
ENDFOREACH(FILE ${IconFiles})

# Copy the plugin file.
FIND_FILE ( PLUGIN_FILE_NAME "Minerva.plugins" PATHS ${CMAKE_CURRENT_SOURCE_DIR} )

CONFIGURE_FILE ( ${CMAKE_CURRENT_SOURCE_DIR}/Plugins.xml ${EXECUTABLE_OUTPUT_PATH}/configs/Minerva.plugins )
#CADKIT_COPY( ${TARGET_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/Minerva.plugins"  "${EXECUTABLE_OUTPUT_PATH}/configs/Minerva.plugins" )

# --- If we are on OS X copy all the embedded libraries to the app bundle
IF (APPLE)

	# This is a hack.  When CMake runs, copy the plugin file to both Debug and Release app packages.
	CONFIGURE_FILE ( ${CMAKE_CURRENT_SOURCE_DIR}/Plugins.xml ${EXECUTABLE_OUTPUT_PATH}/Release/${TARGET_OUTPUT_NAME}.app/Contents/Plugins/Minerva.plugins )
	CONFIGURE_FILE ( ${CMAKE_CURRENT_SOURCE_DIR}/Plugins.xml ${EXECUTABLE_OUTPUT_PATH}/Debug/${TARGET_OUTPUT_NAME}.app/Contents/Plugins/Minerva.plugins )

  IF(CMAKE_BUILD_TYPE MATCHES "Debug")
      MakeOSXBundleApp( ${TARGET_OUTPUT_NAME} ${PROJECT_BINARY_DIR})
  ELSE (CMAKE_BUILD_TYPE MATCHES "Debug")
      MakeOSXBundleApp( ${TARGET_OUTPUT_NAME} ${PROJECT_BINARY_DIR})
  ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")

ELSE(APPLE)
  
INSTALL(TARGETS ${TARGET_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin
  ARCHIVE DESTINATION bin
)

ENDIF (APPLE)
