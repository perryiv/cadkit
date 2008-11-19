
#######################################################
#
#  Cmake file for all cadkit libaries to include.
#
#######################################################

# Option for tracing.
OPTION ( USUL_USE_TRACING "Should tracing be enabled?" OFF )

# We don't want RPath.
SET ( CMAKE_SKIP_RPATH ON )

# Turn on tracing if we are supposed to.
#IF ( USUL_TRACING )
#ADD_DEFINITIONS("-D_USUL_TRACE")
#ENDIF( USUL_TRACING )

# Append d to the library name if the build is debug.
SET(CMAKE_DEBUG_POSTFIX d)

#######################################################
#
#  No need to define NDEBUG.  Cmake defines these.
#
#######################################################

# Add _DEBUG definition is it's debug.
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
#SET(CMAKE_DEBUG_POSTFIX d)
ADD_DEFINITIONS("-D_DEBUG")
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# A macro for linking to other cadkit libraries.
# This will link to the proper library depending on build type.
MACRO(LINK_CADKIT TRGTNAME)
	FOREACH(LINKLIB ${ARGN})
		TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${LINKLIB}" debug "${LINKLIB}${CMAKE_DEBUG_POSTFIX}")
	ENDFOREACH(LINKLIB)
ENDMACRO(LINK_CADKIT TRGTNAME)


#######################################################
#
#  Set variables for copy
#
#######################################################

IF(WIN32)
	SET ( CADKIT_COPY_COMMAND xcopy )
	SET ( CADKIT_COPY_OPTIONS /f /i /k /y /c )
ELSE(WIN32)
	SET ( CADKIT_COPY_COMMAND cp )
	SET ( CADKIT_COPY_OPTIONS )
ENDIF(WIN32)


#######################################################
#
#  Macro to change path containing '/' to '\' on Windows.
#
#######################################################

MACRO(CORRECT_PATH VAR PATH)
    SET(${VAR} ${PATH})
	IF(WIN32)
		STRING(REGEX REPLACE "/" "\\\\" ${VAR} "${PATH}")
	ENDIF(WIN32)
ENDMACRO(CORRECT_PATH)


#######################################################
#
#  Macro copy the source file to destination.
#
#######################################################

MACRO(CADKIT_COPY TARGET_NAME SOURCE DESTINATION)
	CORRECT_PATH ( SOURCE_ ${SOURCE} )
	CORRECT_PATH ( DESTINATION_ ${DESTINATION} )
	ADD_CUSTOM_COMMAND(
       SOURCE ${TARGET_NAME}
      COMMAND ${CCOMMAND_COMMAND}
       ARGS ${CADKIT_COPY_COMMAND} ${SOURCE_} ${DESTINATION_} ${CADKIT_COPY_OPTIONS}
       TARGET ${TARGET_NAME} )
ENDMACRO(CADKIT_COPY TARGET_NAME SOURCE DESTINATION)


#######################################################
#
#  Add a library.
#
#######################################################

MACRO(CADKIT_ADD_LIBRARY LIBRARY_NAME)

	INSTALL(TARGETS ${LIBRARY_NAME}
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION bin
			ARCHIVE DESTINATION bin )
			
	# Make the label prefix.
	IF( MSVC )
		SET ( CADKIT_LIBRARY_LABEL_PREFIX "Lib:" )
	ELSE( MSVC )
		SET ( CADKIT_LIBRARY_LABEL_PREFIX "Lib" )
	ENDIF( MSVC )
	
	# Add the target label.
	SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES PROJECT_LABEL "${CADKIT_LIBRARY_LABEL_PREFIX} ${LIBRARY_NAME}")

ENDMACRO(CADKIT_ADD_LIBRARY LIBRARY_NAME)


#######################################################
#
#  Macro to set release and debug library variables.
#
#######################################################

MACRO(CADKIT_SET_DEBUG_RELEASE_LIBRARY _LIBRARY _LIBRARY_DEBUG _LIBRARY_RELEASE)
	
	SET ( ${_LIBRARY} ${_LIBRARY_RELEASE} )
	
	# If only the release version was found, set the debug variable also to the release version
     IF (${_LIBRARY_RELEASE} AND NOT ${_LIBRARY_DEBUG})
       SET(${_LIBRARY_DEBUG} ${${_LIBRARY_RELEASE}})
       SET(${_LIBRARY}       ${${_LIBRARY_RELEASE}})
     ENDIF (${_LIBRARY_RELEASE} AND NOT ${_LIBRARY_DEBUG})

     # If only the debug version was found, set the release variable also to the debug version
     IF (${_LIBRARY_DEBUG} AND NOT ${_LIBRARY_RELEASE})
       SET(${_LIBRARY_RELEASE} ${${_LIBRARY_DEBUG}})
       SET(${_LIBRARY}         ${${_LIBRARY_DEBUG}})
     ENDIF (${_LIBRARY_DEBUG} AND NOT ${_LIBRARY_RELEASE})

	IF ( ${_LIBRARY_RELEASE} AND ${_LIBRARY_DEBUG} )
		SET( ${_LIBRARY} optimized ${${_LIBRARY_RELEASE}} debug ${${_LIBRARY_DEBUG}} )
	ENDIF ( ${_LIBRARY_RELEASE} AND ${_LIBRARY_DEBUG} )
	
ENDMACRO(CADKIT_SET_DEBUG_RELEASE_LIBRARY _LIBRARY _LIBRARY_DEBUG _LIBRARY_RELEASE)

 
#######################################################
#
#  Set up common variables.
#
#######################################################

IF (NOT CADKIT_LIB_DIR)
   set(CADKIT_LIB_DIR "$ENV{CADKIT_LIB_DIR}")
endif( NOT CADKIT_LIB_DIR )

if (NOT CADKIT_BIN_DIR)
   set(CADKIT_BIN_DIR "$ENV{CADKIT_BIN_DIR}")
endif( NOT CADKIT_BIN_DIR )

if (NOT CADKIT_BIN_DIR) 	 
	SET (CADKIT_BIN_DIR ${LIBRARY_OUTPUT_PATH} ) 	 
endif (NOT CADKIT_BIN_DIR)


if (NOT CADKIT_INC_DIR)
   set(CADKIT_INC_DIR "$ENV{CADKIT_INC_DIR}")
endif( NOT CADKIT_INC_DIR )


# setup output Directories
SET (LIBRARY_OUTPUT_PATH
	${CADKIT_LIB_DIR}
	CACHE PATH
	"Single Directory for all Libraries"
)

# setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
	${CADKIT_BIN_DIR}
	CACHE PATH
	"Single Directory for all Executables."
)

# We want to see compiler arguments.
# Note: If we set this here, the user cannot override in the user interface.
SET (CMAKE_VERBOSE_MAKEFILE "ON")
