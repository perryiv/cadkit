
# Cmake file for all cadkit libaries to include.

# Option for tracing.
OPTION ( USUL_TRACING "Should tracing be enabled?" OFF )

# We don't want RPath.
SET ( CMAKE_SKIP_RPATH ON )

# Turn on tracing if we are supposed to.
IF ( USUL_TRACING )
ADD_DEFINITIONS("-D_USUL_TRACE")
ENDIF( USUL_TRACING )

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


 # Set up common variables	 
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
