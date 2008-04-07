
# Cmake file for all cadkit libaries to include.

# Option for tracing.
OPTION ( USUL_TRACING "Should tracing be enabled?" OFF )

# We don't want RPath.
SET ( CMAKE_SKIP_RPATH ON )

# Turn on tracing if we are supposed to.
IF ( USUL_TRACING )
ADD_DEFINITIONS("-D_USUL_TRACE")
ENDIF( USUL_TRACING )

#######################################################
#
#  No need to define _DEBUG and NDEBUG.  Cmake defines these.
#
#######################################################

# Add _DEBUG definition is it's debug.
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
#ADD_DEFINITIONS("-D_DEBUG")
SET(CMAKE_DEBUG_POSTFIX d)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")


# Add NDEBUG definition is it's release.
#IF (CMAKE_BUILD_TYPE STREQUAL "Release")
#ADD_DEFINITIONS("-DNDEBUG")
#ENDIF (CMAKE_BUILD_TYPE STREQUAL "Release")


# A macro for linking to other cadkit libraries.
# This will link to the proper library depending on build type.
MACRO(LINK_CADKIT TRGTNAME)
	FOREACH(LINKLIB ${ARGN})
		TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${LINKLIB}" debug "${LINKLIB}${CMAKE_DEBUG_POSTFIX}")
	ENDFOREACH(LINKLIB)
ENDMACRO(LINK_CADKIT TRGTNAME)


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
SET (CMAKE_VERBOSE_MAKEFILE "ON")
