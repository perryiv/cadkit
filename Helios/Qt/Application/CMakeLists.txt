
PROJECT(Helios)

# Set the target name.
SET ( TARGET_NAME HeliosQt )

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

INCLUDE ( ${CMakeModules}/FindOSG.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )


# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES(
        ${CADKIT_INC_DIR}
       "${CADKIT_INC_DIR}/Experimental"
        )

# List the Sources
SET ( SOURCES main.cpp )

# If we are building on Windows, add the resource file for the icon.
IF(WIN32)
    SET( SOURCES ${SOURCES} Resources.rc )
	
	# Add Unicode preprocessor definition.
	ADD_DEFINITIONS("-DUNICODE")
ENDIF(WIN32)

# Create an executable.
ADD_EXECUTABLE( ${TARGET_NAME} ${SOURCES} )

# Note: Set targe properties have to be called after ADD_EXECUTABLE.

# Settings so the console does not appear when running (See: http://www.cmake.org/Wiki/VSConfigSpecificSettings).
IF(WIN32)	
	SET_TARGET_PROPERTIES( ${TARGET_NAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS" )
	SET_TARGET_PROPERTIES( ${TARGET_NAME} PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS" )
	SET_TARGET_PROPERTIES( ${TARGET_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS" )
	SET_TARGET_PROPERTIES( ${TARGET_NAME} PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS" )
ENDIF(WIN32)

# Add the target label.
SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES PROJECT_LABEL "App ${TARGET_NAME}")

# Add the debug postfix.
SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

# Link the Library	
LINK_CADKIT( ${TARGET_NAME} Usul OpenThreadsEx HeliosQtCore )

TARGET_LINK_LIBRARIES( ${TARGET_NAME}
   ${QT_LIBRARIES}
   ${QT_QTMAIN_LIBRARY}
   ${OPENTHREADS_LIB}
)

INSTALL(TARGETS ${TARGET_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin
  ARCHIVE DESTINATION bin
)
