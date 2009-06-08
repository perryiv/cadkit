
# Set variables in Cmake.  Use the user set environment variables
# If these are not set, then they will simply be ignored.

# try to find the header
FIND_PATH(PQXX_INCLUDE_DIR pqxx/pqxx
  $ENV{PQXX_INC_DIR}
  /usr/include
  /usr/local/include
)

# Find the library
FIND_LIBRARY(PQXX_LIBRARY_DEBUG
	NAMES pqxxd
	PATHS
	$ENV{PQXX_LIB_DIR}
    /usr/lib64
    /usr/local/lib64
	/usr/lib
	/usr/local/lib
	DOC "The name of the PQXX library"
)

# Find the library
FIND_LIBRARY(PQXX_LIBRARY_RELEASE
	NAMES pqxx
	PATHS
	$ENV{PQXX_LIB_DIR}
    /usr/lib64
    /usr/local/lib64
	/usr/lib
	/usr/local/lib
	DOC "The name of the PQXX library"
)

CADKIT_SET_DEBUG_RELEASE_LIBRARY( PQXX_LIBRARY PQXX_LIBRARY_DEBUG PQXX_LIBRARY_RELEASE)

IF (PQXX_INCLUDE_DIR AND PQXX_LIBRARY)
  SET (PQXX_FOUND TRUE)
  SET (PQXX_LIBRARIES "${PQXX_LIBRARY}")
ENDIF (PQXX_INCLUDE_DIR AND PQXX_LIBRARY)
 
IF (PQXX_FOUND)
  IF (NOT PQXX_FIND_QUIETLY)
    MESSAGE (STATUS "Found PQXX: ${PQXX_LIBRARY}")
  ENDIF (NOT PQXX_FIND_QUIETLY)
ELSE (PQXX_FOUND)
  IF (PQXX_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find PQXX")
  ENDIF (PQXX_FIND_REQUIRED)
ENDIF (PQXX_FOUND)
 	
MARK_AS_ADVANCED(
  PQXX_INCLUDE_DIR
  PQXX_LIBRARY
)
