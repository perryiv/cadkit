
#######################################################
#
#  Cmake file for all cadkit libaries to include.
#
#######################################################

# Set compile flags for Visual Studio.
IF(MSVC)

	# Set flag to translate structured exceptions into c++ exceptions.
	STRING(REPLACE "EHsc" "EHa" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} )
	
	# Up the warning level.  This gives a ton of warnings and I'm not ready to fix them all.
	#STRING(REPLACE "W3" "W4" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} )
	
	# Disable these warnings.
	# 4127 is "conditional expression is constant".
	#SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4127" )
	
	ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")
	
	# Treat warnings as errors.
	SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX" )
	
ENDIF(MSVC)

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
		IF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} GREATER 4)
			TARGET_LINK_LIBRARIES(${TRGTNAME} ${LINKLIB})
		ELSEIF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} GREATER 4)
			TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${LINKLIB}" debug "${LINKLIB}${CMAKE_DEBUG_POSTFIX}")
		ENDIF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} GREATER 4)
	ENDFOREACH(LINKLIB)
ENDMACRO(LINK_CADKIT TRGTNAME)


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
      COMMAND ${CMAKE_COMMAND}
      ARGS -E copy ${SOURCE_} ${DESTINATION_}
      TARGET ${TARGET_NAME} )
ENDMACRO(CADKIT_COPY TARGET_NAME SOURCE DESTINATION)


#######################################################
#
#  Add a library.
#
#######################################################

FUNCTION(CADKIT_ADD_LIBRARY LIBRARY_NAME)

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

ENDFUNCTION(CADKIT_ADD_LIBRARY)


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


# -------- Function to build OS X Stand Alone Bundles -----------------
function(MakeOSXBundleApp target binary_dir)

	SET( SOURCE_BUNDLE_CMAKE_SCRIPT "${CMakeModules}/CompleteBundle.cmake.in" )

	IF(${CMAKE_GENERATOR} MATCHES "Xcode")
	
		SET (OSX_COPY_PLUGINS_SCRIPT "${binary_dir}/${target}_OSX_CopyPlugins.sh")
		CONFIGURE_FILE( "${CMakeModules}/CopyPlugins.sh.in.xcode" "${OSX_COPY_PLUGINS_SCRIPT}" @ONLY IMMEDIATE)
   
   		# Copy plugins when building, but not installing.
   		ADD_CUSTOM_COMMAND(
       		SOURCE ${TARGET_NAME}
      		COMMAND ${OSX_COPY_PLUGINS_SCRIPT}
       		TARGET ${TARGET_NAME} )
       
		SET( SOURCE_BUNDLE_BASH_SCRIPT "${CMakeModules}/CreateBundle.sh.in.xcode" )
	ELSE(${CMAKE_GENERATOR} MATCHES "Xcode")
		SET( SOURCE_BUNDLE_BASH_SCRIPT "${CMakeModules}/CreateBundle.sh.in" )
	ENDIF(${CMAKE_GENERATOR} MATCHES "Xcode")
    
    SET (OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT "${binary_dir}/${target}_OSX_MakeStandAloneBundle.cmake")
    SET (OSX_MAKE_STANDALONE_BUNDLE_BASH_SCRIPT "${binary_dir}/${target}_OSX_MakeStandAloneBundle.sh")

    CONFIGURE_FILE( "${SOURCE_BUNDLE_CMAKE_SCRIPT}" "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" @ONLY IMMEDIATE)
    CONFIGURE_FILE( "${SOURCE_BUNDLE_BASH_SCRIPT}" "${OSX_MAKE_STANDALONE_BUNDLE_BASH_SCRIPT}" @ONLY IMMEDIATE)
 
   install(SCRIPT "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}")
                    
endfunction(MakeOSXBundleApp)
