
# Set variables in Cmake.  Use the user set environment variables
# If these are not set, then they will simply be ignored.

# try to find the header
FIND_PATH(HDF_INCLUDE_DIR hdf5.h
  $ENV{HDF_INC_DIR}
  /usr/include
  /usr/local/include
  /usr/include/HDF
)

# Find the library
FIND_LIBRARY(HDF_LIBRARY
	NAMES hdf5
	PATHS
	$ENV{HDF_LIB_DIR}
    /usr/lib64
    /usr/local/lib64
	/usr/lib
	/usr/local/lib
	DOC "The name of the HDF library"
)



IF (HDF_INCLUDE_DIR AND HDF_LIBRARY)
  SET (HDF_FOUND TRUE)
  SET (HDF_LIBRARIES "${HDF_LIBRARY}")
  LINK_DIRECTORIES(${HDF_ROOT_DIR}/lib)
ENDIF (HDF_INCLUDE_DIR AND HDF_LIBRARY)
 
IF (HDF_FOUND)
  IF (NOT HDF_FIND_QUIETLY)
    MESSAGE (STATUS "Found HDF: ${HDF_LIBRARY}")
  ENDIF (NOT HDF_FIND_QUIETLY)
ELSE (HDF_FOUND)
  IF (HDF_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find HDF")
  ENDIF (HDF_FIND_REQUIRED)
ENDIF (HDF_FOUND)
 	
MARK_AS_ADVANCED(
  HDF_INCLUDE_DIR
  HDF_LIBRARY
)
