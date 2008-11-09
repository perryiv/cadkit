
# Set variables in Cmake.  Use the user set environment variables
# If these are not set, then they will simply be ignored.


# try to find the header
FIND_PATH(XERCESC_INCLUDE_DIR xercesc/parsers/SAXParser.hpp
  ${XERCESC_ROOT_DIR}/include
  $ENV{XERCES_INC_DIR}
  /usr/include
  /usr/local/include
)

# Find the release library
FIND_LIBRARY(XERCESC_LIBRARY_RELEASE
	NAMES xerces-c xerces-c_2
	PATHS
	  ${XERCESC_ROOT_DIR}/lib
	  $ENV{XERCES_LIB_DIR}
	  /usr/lib
	  /usr/local/lib
	DOC "The name of the xerces-c library"
)

# Find the debug library
FIND_LIBRARY(XERCESC_LIBRARY_DEBUG
	NAMES xerces-cd xerces-c_2D
	PATHS
	  ${XERCESC_ROOT_DIR}/lib
	  $ENV{XERCES_LIB_DIR}
	  /usr/lib
	  /usr/local/lib
	DOC "The name of the xerces-c library"
)


# See if anything was found.
IF (XERCESC_INCLUDE_DIR)
	
	# If only the release version was found, set the debug variable also to the release version
     IF (XERCESC_LIBRARY_RELEASE AND NOT XERCESC_LIBRARY_DEBUG)
       SET(XERCESC_LIBRARY_DEBUG ${XERCESC_LIBRARY_RELEASE})
       SET(XERCESC_LIBRARY       ${XERCESC_LIBRARY_RELEASE})
     ENDIF (XERCESC_LIBRARY_RELEASE AND NOT XERCESC_LIBRARY_DEBUG)

     # If only the debug version was found, set the release variable also to the debug version
     IF (XERCESC_LIBRARY_DEBUG AND NOT XERCESC_LIBRARY_RELEASE)
       SET(XERCESC_LIBRARY_RELEASE ${XERCESC_LIBRARY_DEBUG})
       SET(XERCESC_LIBRARY         ${XERCESC_LIBRARY_DEBUG})
     ENDIF (XERCESC_LIBRARY_DEBUG AND NOT XERCESC_LIBRARY_RELEASE)

	IF ( XERCESC_LIBRARY_RELEASE AND XERCESC_LIBRARY_DEBUG )
		SET( XERCESC_LIBRARY optimized ${XERCESC_LIBRARY_RELEASE} debug ${XERCESC_LIBRARY_DEBUG} )
	ENDIF ( XERCESC_LIBRARY_RELEASE AND XERCESC_LIBRARY_DEBUG )
	
ENDIF (XERCESC_INCLUDE_DIR)

IF(XERCESC_LIBRARY)
	SET (XERCESC_FOUND TRUE)
	SET (XERCESC_LIBRARIES "${XERCESC_LIBRARY}")
ENDIF(XERCESC_LIBRARY)
 	
IF (XERCESC_FOUND)
  IF (NOT XERCESC_FIND_QUIETLY)
    MESSAGE (STATUS "Found Xerces-C: ${XERCESC_LIBRARY}")
  ENDIF (NOT XERCESC_FIND_QUIETLY)
ELSE (XERCESC_FOUND)
  IF (XERCESC_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find Xerces-C")
  ENDIF (XERCESC_FIND_REQUIRED)
ENDIF (XERCESC_FOUND)
 
MARK_AS_ADVANCED(
  XERCESC_INCLUDE_DIR
  XERCESC_LIBRARY
)
