
# Set variables in Cmake.  Use the user set environment variables
# If these are not set, then they will simply be ignored.

# try to find the header
FIND_PATH(GDAL_INCLUDE_DIR gdal.h
  $ENV{GDAL_INC_DIR}
  /usr/include
  /usr/local/include
  /usr/include/gdal
)

# Find the library
FIND_LIBRARY(GDAL_LIBRARY
	NAMES gdal gdal_i
	PATHS
	$ENV{GDAL_LIB_DIR}
    /usr/lib64
    /usr/local/lib64
	/usr/lib
	/usr/local/lib
	DOC "The name of the gdal library"
)



IF (GDAL_INCLUDE_DIR AND GDAL_LIBRARY)
  SET (GDAL_FOUND TRUE)
  SET (GDAL_LIBRARIES "${GDAL_LIBRARY}")
  LINK_DIRECTORIES(${GDAL_ROOT_DIR}/lib)
ENDIF (GDAL_INCLUDE_DIR AND GDAL_LIBRARY)
 
IF (GDAL_FOUND)
  IF (NOT GDAL_FIND_QUIETLY)
    MESSAGE (STATUS "Found GDAL: ${GDAL_LIBRARY}")
  ENDIF (NOT GDAL_FIND_QUIETLY)
ELSE (GDAL_FOUND)
  IF (GDAL_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find GDAL")
  ENDIF (GDAL_FIND_REQUIRED)
ENDIF (GDAL_FOUND)
 	
MARK_AS_ADVANCED(
  GDAL_INCLUDE_DIR
  GDAL_LIBRARY
)
