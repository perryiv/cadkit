# - Find the Boost includes and libraries.
# The following variables are set if Boost is found.  If Boost is not
# found, Boost_FOUND is set to false.
#  Boost_FOUND        - True when the Boost include directory is found.
#  Boost_INCLUDE_DIRS - the path to where the boost include files are.
#  Boost_LIBRARY_DIRS - The path to where the boost library files are.
#  Boost_LIB_DIAGNOSTIC_DEFINITIONS - Only set if using Windows.

# ----------------------------------------------------------------------------
# If you have installed Boost in a non-standard location or you have
# just staged the boost files using bjam then you have three
# options. In the following comments, it is assumed that <Your Path>
# points to the root directory of the include directory of Boost. e.g
# If you have put boost in C:\development\Boost then <Your Path> is
# "C:/development/Boost" and in this directory there will be two
# directories called "include" and "lib".
# 1) After CMake runs, set Boost_INCLUDE_DIR to <Your Path>/include/boost<-version>
# 2) Use CMAKE_INCLUDE_PATH to set a path to <Your Path>/include. This will allow FIND_PATH()
#    to locate Boost_INCLUDE_DIR by utilizing the PATH_SUFFIXES option. e.g.
#    SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "<Your Path>/include")
# 3) Set an environment variable called ${Boost_ROOT} that points to the root of where you have
#    installed Boost, e.g. <Your Path>. It is assumed that there is at least a subdirectory called
#    include in this path.
#
# Note:
#  1) If you are just using the boost headers, then you do not need to use
#     Boost_LIBRARY_DIRS in your CMakeLists.txt file.
#  2) If Boost has not been installed, then when setting Boost_LIBRARY_DIRS
#     the script will look for /lib first and, if this fails, then for /stage/lib.
#
# Usage:
# In your CMakeLists.txt file do something like this:
# ...
# # Boost
# FIND_PACKAGE(Boost)
# ...
# INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
# LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
#
# In Windows, we make the assumption that, if the Boost files are installed, the default directory
# will be C:\boost.

#
# TODO:
#
# 1) Automatically find the Boost library files and eliminate the need
#    to use Link Directories.
#

IF(WIN32)
  # In windows, automatic linking is performed, so you do not have to specify the libraries.
  # If you are linking to a dynamic runtime, then you can choose to link to either a static or a
  # dynamic Boost library, the default is to do a static link.  You can alter this for a specific
  # library "whatever" by defining Boost_WHATEVER_DYN_LINK to force Boost library "whatever" to
  # be linked dynamically.  Alternatively you can force all Boost libraries to dynamic link by
  # defining Boost_ALL_DYN_LINK.

  # This feature can be disabled for Boost library "whatever" by defining Boost_WHATEVER_NO_LIB,
  # or for all of Boost by defining Boost_ALL_NO_LIB.

  # If you want to observe which libraries are being linked against then defining
  # Boost_LIB_DIAGNOSTIC will cause the auto-linking code to emit a #pragma message each time
  # a library is selected for linking.
  SET(Boost_LIB_DIAGNOSTIC_DEFINITIONS "-DBoost_LIB_DIAGNOSTIC")
ENDIF(WIN32)


SET(Boost_INCLUDE_PATH_DESCRIPTION "directory containing the boost include files. E.g /usr/local/include/boost-1_33_1 or c:/boost/include/boost-1_33_1")

SET(Boost_DIR_MESSAGE "Set the Boost_INCLUDE_DIR cmake cache entry to the ${Boost_INCLUDE_PATH_DESCRIPTION}")

SET(Boost_DIR_SEARCH $ENV{Boost_ROOT})
IF(Boost_DIR_SEARCH)
  FILE(TO_CMAKE_PATH ${Boost_DIR_SEARCH} Boost_DIR_SEARCH)
  SET(Boost_DIR_SEARCH ${Boost_DIR_SEARCH}/include)
ENDIF(Boost_DIR_SEARCH)

IF(WIN32)
  SET(Boost_DIR_SEARCH
    ${Boost_DIR_SEARCH}
    C:/boost/include
    D:/boost/include
  )
ENDIF(WIN32)

# Add in some path suffixes. These will have to be updated whenever a new Boost version comes out.
SET(SUFFIX_FOR_PATH
 boost-1_34_1
 boost-1_34_0
 boost-1_33_1
 boost-1_33_0
)

SET (Boost_LIBRARIES "")
SET (BOOST "boost")
SET (Boost_VERSION "-1_33_1")
SET (Boost_DEBUG "-d")
# - Boost has a naming scheme that we have to match..
IF (MINGW)
SET (Boost_COMPILER "-mgw")
ENDIF (MINGW)


#
# Look for an installation.
#
FIND_PATH(Boost_INCLUDE_DIR NAMES boost/config.hpp PATH_SUFFIXES ${SUFFIX_FOR_PATH} PATHS

  # Look in other places.
  ${Boost_DIR_SEARCH}

  # Help the user find it if we cannot.
  DOC "The ${Boost_INCLUDE_PATH_DESCRIPTION}"
)

MARK_AS_ADVANCED(Boost_INCLUDE_DIR)
# Assume we didn't find it.
SET(Boost_FOUND 0)

# Now try to get the include and library path.
IF(Boost_INCLUDE_DIR)

  # Look for the boost library path.
  # Note that the user may not have installed any libraries
  # so it is quite possible the Boost_LIBRARY_PATH may not exist.
  SET(Boost_LIBRARY_DIR ${Boost_INCLUDE_DIR})

  IF("${Boost_LIBRARY_DIR}" MATCHES "boost-[0-9]+")
    GET_FILENAME_COMPONENT(Boost_LIBRARY_DIR ${Boost_LIBRARY_DIR} PATH)
  ENDIF ("${Boost_LIBRARY_DIR}" MATCHES "boost-[0-9]+")

  IF("${Boost_LIBRARY_DIR}" MATCHES "/include$")
    # Strip off the trailing "/include" in the path.
    GET_FILENAME_COMPONENT(Boost_LIBRARY_DIR ${Boost_LIBRARY_DIR} PATH)
  ENDIF("${Boost_LIBRARY_DIR}" MATCHES "/include$")

  IF(EXISTS "${Boost_LIBRARY_DIR}/lib")
    SET (Boost_LIBRARY_DIR ${Boost_LIBRARY_DIR}/lib)
  ELSE(EXISTS "${Boost_LIBRARY_DIR}/lib")
    IF(EXISTS "${Boost_LIBRARY_DIR}/stage/lib")
      SET(Boost_LIBRARY_DIR ${Boost_LIBRARY_DIR}/stage/lib)
    ELSE(EXISTS "${Boost_LIBRARY_DIR}/stage/lib")
      SET(Boost_LIBRARY_DIR "")
    ENDIF(EXISTS "${Boost_LIBRARY_DIR}/stage/lib")
  ENDIF(EXISTS "${Boost_LIBRARY_DIR}/lib")

  IF(EXISTS "${Boost_INCLUDE_DIR}")
    SET(Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIR})
    # We have found boost. It is possible that the user has not
    # compiled any libraries so we set Boost_FOUND to be true here.
    SET(Boost_FOUND 1)
  ENDIF(EXISTS "${Boost_INCLUDE_DIR}")

  IF(Boost_LIBRARY_DIR AND EXISTS "${Boost_LIBRARY_DIR}")
    SET(Boost_LIBRARY_DIRS ${Boost_LIBRARY_DIR})
  ENDIF(Boost_LIBRARY_DIR AND EXISTS "${Boost_LIBRARY_DIR}")
ENDIF(Boost_INCLUDE_DIR)

IF(NOT Boost_FOUND)
  IF(NOT Boost_FIND_QUIETLY)
    MESSAGE(STATUS "Boost was not found. ${Boost_DIR_MESSAGE}")
  ELSE(NOT Boost_FIND_QUIETLY)
    IF(Boost_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Boost was not found. ${Boost_DIR_MESSAGE}")
    ENDIF(Boost_FIND_REQUIRED)
  ENDIF(NOT Boost_FIND_QUIETLY)
ENDIF(NOT Boost_FOUND)

IF ( Boost_FOUND )
  INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} ${INCLUDE_DIRECTORIES})
ENDIF ( Boost_FOUND )

# ------------ Check for Each Boost Library ----------------
#-- Add the library to the Boost_LIBRARIES variable if found and the user asked for it
IF ( Boost_USE_DATE_TIME )
  #-- Set the Release and Debug library variables for DATE_TIME
  SET (Boost_DATE_TIME_NAME "date_time")

  #-- Try to find the release version of the date_time library
  FIND_LIBRARY(Boost_DATE_TIME_LIBRARY "${BOOST}_${Boost_DATE_TIME_NAME}${Boost_COMPILER}${Boost_VERSION}" 
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_DATE_TIME_LIBRARY)

  #-- Try to find the debug version of the date_time library
  FIND_LIBRARY(Boost_DATE_TIME_LIBRARY_DEBUG "${BOOST}_${Boost_DATE_TIME_NAME}${Boost_COMPILER}${Boost_DEBUG}${Boost_VERSION}"
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_DATE_TIME_LIBRARY_DEBUG)

  #-- Check and add the release version of the library
  IF ( Boost_DATE_TIME_LIBRARY )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        optimized "${Boost_DATE_TIME_LIBRARY}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_DATE_TIME_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_DATE_TIME_LIBRARY )
  #-- Check and add the debug version of the library
  IF ( Boost_DATE_TIME_LIBRARY_DEBUG )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        debug "${Boost_DATE_TIME_LIBRARY_DEBUG}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_DATE_TIME_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_DATE_TIME_LIBRARY_DEBUG )	
ENDIF ( Boost_USE_DATE_TIME )

#-- Add the library to the Boost_LIBRARIES variable if found and the user asked for it
IF ( Boost_USE_FILESYSTEM )
  #-- Set the Release and Debug library variables for FILESYSTEM
  SET (Boost_FILESYSTEM_NAME "filesystem")

  #-- Try to find the release version of the filesystem library
  FIND_LIBRARY(Boost_FILESYSTEM_LIBRARY "${BOOST}_${Boost_FILESYSTEM_NAME}${Boost_COMPILER}${Boost_VERSION}" 
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_FILESYSTEM_LIBRARY)

  #-- Try to find the debug version of the filesystem library
  FIND_LIBRARY(Boost_FILESYSTEM_LIBRARY_DEBUG "${BOOST}_${Boost_FILESYSTEM_NAME}${Boost_COMPILER}${Boost_DEBUG}${Boost_VERSION}"
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_FILESYSTEM_LIBRARY_DEBUG)

  #-- Check and add the release version of the library
  IF ( Boost_FILESYSTEM_LIBRARY )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        optimized "${Boost_FILESYSTEM_LIBRARY}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_FILESYSTEM_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_FILESYSTEM_LIBRARY )
  #-- Check and add the debug version of the library
  IF ( Boost_FILESYSTEM_LIBRARY_DEBUG )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        debug "${Boost_FILESYSTEM_LIBRARY_DEBUG}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_FILESYSTEM_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_FILESYSTEM_LIBRARY_DEBUG )	
ENDIF ( Boost_USE_FILESYSTEM )

#-- Add the library to the Boost_LIBRARIES variable if found and the user asked for it
IF ( Boost_USE_IOSTREAMS )
  #-- Set the Release and Debug library variables for IOSTREAMS
  SET (Boost_IOSTREAMS_NAME "iostreams")

  #-- Try to find the release version of the iostreams library
  FIND_LIBRARY(Boost_IOSTREAMS_LIBRARY "${BOOST}_${Boost_IOSTREAMS_NAME}${Boost_COMPILER}${Boost_VERSION}" 
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_IOSTREAMS_LIBRARY)

  #-- Try to find the debug version of the iostreams library
  FIND_LIBRARY(Boost_IOSTREAMS_LIBRARY_DEBUG "${BOOST}_${Boost_IOSTREAMS_NAME}${Boost_COMPILER}${Boost_DEBUG}${Boost_VERSION}"
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_IOSTREAMS_LIBRARY_DEBUG)

  #-- Check and add the release version of the library
  IF ( Boost_IOSTREAMS_LIBRARY )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        optimized "${Boost_IOSTREAMS_LIBRARY}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_IOSTREAMS_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_IOSTREAMS_LIBRARY )
  #-- Check and add the debug version of the library
  IF ( Boost_IOSTREAMS_LIBRARY_DEBUG )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        debug "${Boost_IOSTREAMS_LIBRARY_DEBUG}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_IOSTREAMS_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_IOSTREAMS_LIBRARY_DEBUG )	
ENDIF ( Boost_USE_IOSTREAMS )

#-- Add the library to the Boost_LIBRARIES variable if found and the user asked for it
IF ( Boost_USE_PRG_EXEC_MONITOR )
  #-- Set the Release and Debug library variables for PRG_EXEC_MONITOR
  SET (Boost_PRG_EXEC_MONITOR_NAME "prg_exec_monitor")

  #-- Try to find the release version of the prg_exec_monitor library
  FIND_LIBRARY(Boost_PRG_EXEC_MONITOR_LIBRARY "${BOOST}_${Boost_PRG_EXEC_MONITOR_NAME}${Boost_COMPILER}${Boost_VERSION}" 
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_PRG_EXEC_MONITOR_LIBRARY)

  #-- Try to find the debug version of the prg_exec_monitor library
  FIND_LIBRARY(Boost_PRG_EXEC_MONITOR_LIBRARY_DEBUG "${BOOST}_${Boost_PRG_EXEC_MONITOR_NAME}${Boost_COMPILER}${Boost_DEBUG}${Boost_VERSION}"
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_PRG_EXEC_MONITOR_LIBRARY_DEBUG)

  #-- Check and add the release version of the library
  IF ( Boost_PRG_EXEC_MONITOR_LIBRARY )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        optimized "${Boost_PRG_EXEC_MONITOR_LIBRARY}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_PRG_EXEC_MONITOR_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_PRG_EXEC_MONITOR_LIBRARY )
  #-- Check and add the debug version of the library
  IF ( Boost_PRG_EXEC_MONITOR_LIBRARY_DEBUG )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        debug "${Boost_PRG_EXEC_MONITOR_LIBRARY_DEBUG}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_PRG_EXEC_MONITOR_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_PRG_EXEC_MONITOR_LIBRARY_DEBUG )	
ENDIF ( Boost_USE_PRG_EXEC_MONITOR )

#-- Add the library to the Boost_LIBRARIES variable if found and the user asked for it
IF ( Boost_USE_PROGRAM_OPTIONS )
  #-- Set the Release and Debug library variables for PROGRAM_OPTIONS
  SET (Boost_PROGRAM_OPTIONS_NAME "program_options")

  #-- Try to find the release version of the program_options library
  FIND_LIBRARY(Boost_PROGRAM_OPTIONS_LIBRARY "${BOOST}_${Boost_PROGRAM_OPTIONS_NAME}${Boost_COMPILER}${Boost_VERSION}" 
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_PROGRAM_OPTIONS_LIBRARY)

  #-- Try to find the debug version of the program_options library
  FIND_LIBRARY(Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG "${BOOST}_${Boost_PROGRAM_OPTIONS_NAME}${Boost_COMPILER}${Boost_DEBUG}${Boost_VERSION}"
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG)

  #-- Check and add the release version of the library
  IF ( Boost_PROGRAM_OPTIONS_LIBRARY )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        optimized "${Boost_PROGRAM_OPTIONS_LIBRARY}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_PROGRAM_OPTIONS_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_PROGRAM_OPTIONS_LIBRARY )
  #-- Check and add the debug version of the library
  IF ( Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        debug "${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_PROGRAM_OPTIONS_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG )	
ENDIF ( Boost_USE_PROGRAM_OPTIONS )

#-- Add the library to the Boost_LIBRARIES variable if found and the user asked for it
IF ( Boost_USE_PYTHON )
  #-- Set the Release and Debug library variables for PYTHON
  SET (Boost_PYTHON_NAME "python")

  #-- Try to find the release version of the python library
  FIND_LIBRARY(Boost_PYTHON_LIBRARY "${BOOST}_${Boost_PYTHON_NAME}${Boost_COMPILER}${Boost_VERSION}" 
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_PYTHON_LIBRARY)

  #-- Try to find the debug version of the python library
  FIND_LIBRARY(Boost_PYTHON_LIBRARY_DEBUG "${BOOST}_${Boost_PYTHON_NAME}${Boost_COMPILER}${Boost_DEBUG}${Boost_VERSION}"
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_PYTHON_LIBRARY_DEBUG)

  #-- Check and add the release version of the library
  IF ( Boost_PYTHON_LIBRARY )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        optimized "${Boost_PYTHON_LIBRARY}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_PYTHON_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_PYTHON_LIBRARY )
  #-- Check and add the debug version of the library
  IF ( Boost_PYTHON_LIBRARY_DEBUG )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        debug "${Boost_PYTHON_LIBRARY_DEBUG}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_PYTHON_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_PYTHON_LIBRARY_DEBUG )	
ENDIF ( Boost_USE_PYTHON )

#-- Add the library to the Boost_LIBRARIES variable if found and the user asked for it
IF ( Boost_USE_REGEX )
  #-- Set the Release and Debug library variables for REGEX
  SET (Boost_REGEX_NAME "regex")

  #-- Try to find the release version of the regex library
  FIND_LIBRARY(Boost_REGEX_LIBRARY "${BOOST}_${Boost_REGEX_NAME}${Boost_COMPILER}${Boost_VERSION}" 
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_REGEX_LIBRARY)

  #-- Try to find the debug version of the regex library
  FIND_LIBRARY(Boost_REGEX_LIBRARY_DEBUG "${BOOST}_${Boost_REGEX_NAME}${Boost_COMPILER}${Boost_DEBUG}${Boost_VERSION}"
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_REGEX_LIBRARY_DEBUG)

  #-- Check and add the release version of the library
  IF ( Boost_REGEX_LIBRARY )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        optimized "${Boost_REGEX_LIBRARY}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_REGEX_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_REGEX_LIBRARY )
  #-- Check and add the debug version of the library
  IF ( Boost_REGEX_LIBRARY_DEBUG )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        debug "${Boost_REGEX_LIBRARY_DEBUG}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_REGEX_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_REGEX_LIBRARY_DEBUG )	
ENDIF ( Boost_USE_REGEX )

#-- Add the library to the Boost_LIBRARIES variable if found and the user asked for it
IF ( Boost_USE_SERIALIZATION )
  #-- Set the Release and Debug library variables for SERIALIZATION
  SET (Boost_SERIALIZATION_NAME "serialization")

  #-- Try to find the release version of the serialization library
  FIND_LIBRARY(Boost_SERIALIZATION_LIBRARY "${BOOST}_${Boost_SERIALIZATION_NAME}${Boost_COMPILER}${Boost_VERSION}" 
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_SERIALIZATION_LIBRARY)

  #-- Try to find the debug version of the serialization library
  FIND_LIBRARY(Boost_SERIALIZATION_LIBRARY_DEBUG "${BOOST}_${Boost_SERIALIZATION_NAME}${Boost_COMPILER}${Boost_DEBUG}${Boost_VERSION}"
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_SERIALIZATION_LIBRARY_DEBUG)

  #-- Check and add the release version of the library
  IF ( Boost_SERIALIZATION_LIBRARY )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        optimized "${Boost_SERIALIZATION_LIBRARY}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_SERIALIZATION_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_SERIALIZATION_LIBRARY )
  #-- Check and add the debug version of the library
  IF ( Boost_SERIALIZATION_LIBRARY_DEBUG )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        debug "${Boost_SERIALIZATION_LIBRARY_DEBUG}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_SERIALIZATION_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_SERIALIZATION_LIBRARY_DEBUG )	
ENDIF ( Boost_USE_SERIALIZATION )

#-- Add the library to the Boost_LIBRARIES variable if found and the user asked for it
IF ( Boost_USE_SIGNALS )
  #-- Set the Release and Debug library variables for SIGNALS
  SET (Boost_SIGNALS_NAME "signals")

  #-- Try to find the release version of the signals library
  FIND_LIBRARY(Boost_SIGNALS_LIBRARY "${BOOST}_${Boost_SIGNALS_NAME}${Boost_COMPILER}${Boost_VERSION}" 
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_SIGNALS_LIBRARY)

  #-- Try to find the debug version of the signals library
  FIND_LIBRARY(Boost_SIGNALS_LIBRARY_DEBUG "${BOOST}_${Boost_SIGNALS_NAME}${Boost_COMPILER}${Boost_DEBUG}${Boost_VERSION}"
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_SIGNALS_LIBRARY_DEBUG)

  #-- Check and add the release version of the library
  IF ( Boost_SIGNALS_LIBRARY )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        optimized "${Boost_SIGNALS_LIBRARY}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_SIGNALS_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_SIGNALS_LIBRARY )
  #-- Check and add the debug version of the library
  IF ( Boost_SIGNALS_LIBRARY_DEBUG )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        debug "${Boost_SIGNALS_LIBRARY_DEBUG}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_SIGNALS_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_SIGNALS_LIBRARY_DEBUG )	
ENDIF ( Boost_USE_SIGNALS )

#-- Add the library to the Boost_LIBRARIES variable if found and the user asked for it
IF ( Boost_USE_TEST_EXEC_MONITOR )
  #-- Set the Release and Debug library variables for TEST_EXEC_MONITOR
  SET (Boost_TEST_EXEC_MONITOR_NAME "test_exec_monitor")

  #-- Try to find the release version of the test_exec_monitor library
  FIND_LIBRARY(Boost_TEST_EXEC_MONITOR_LIBRARY "${BOOST}_${Boost_TEST_EXEC_MONITOR_NAME}${Boost_COMPILER}${Boost_VERSION}" 
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_TEST_EXEC_MONITOR_LIBRARY)

  #-- Try to find the debug version of the test_exec_monitor library
  FIND_LIBRARY(Boost_TEST_EXEC_MONITOR_LIBRARY_DEBUG "${BOOST}_${Boost_TEST_EXEC_MONITOR_NAME}${Boost_COMPILER}${Boost_DEBUG}${Boost_VERSION}"
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_TEST_EXEC_MONITOR_LIBRARY_DEBUG)

  #-- Check and add the release version of the library
  IF ( Boost_TEST_EXEC_MONITOR_LIBRARY )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        optimized "${Boost_TEST_EXEC_MONITOR_LIBRARY}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_TEST_EXEC_MONITOR_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_TEST_EXEC_MONITOR_LIBRARY )
  #-- Check and add the debug version of the library
  IF ( Boost_TEST_EXEC_MONITOR_LIBRARY_DEBUG )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        debug "${Boost_TEST_EXEC_MONITOR_LIBRARY_DEBUG}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_TEST_EXEC_MONITOR_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_TEST_EXEC_MONITOR_LIBRARY_DEBUG )	
ENDIF ( Boost_USE_TEST_EXEC_MONITOR )

#-- Add the library to the Boost_LIBRARIES variable if found and the user asked for it
IF ( Boost_USE_THREAD )
  #-- Set the Release and Debug library variables for THREAD
  SET (Boost_THREAD_NAME "thread")

  #-- Try to find the release version of the thread library
  FIND_LIBRARY(Boost_THREAD_LIBRARY "${BOOST}_${Boost_THREAD_NAME}${Boost_COMPILER}${Boost_VERSION}" 
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_THREAD_LIBRARY)

  #-- Try to find the debug version of the thread library
  FIND_LIBRARY(Boost_THREAD_LIBRARY_DEBUG "${BOOST}_${Boost_THREAD_NAME}${Boost_COMPILER}${Boost_DEBUG}${Boost_VERSION}"
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_THREAD_LIBRARY_DEBUG)

  #-- Check and add the release version of the library
  IF ( Boost_THREAD_LIBRARY )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        optimized "${Boost_THREAD_LIBRARY}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_THREAD_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_THREAD_LIBRARY )
  #-- Check and add the debug version of the library
  IF ( Boost_THREAD_LIBRARY_DEBUG )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        debug "${Boost_THREAD_LIBRARY_DEBUG}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_THREAD_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_THREAD_LIBRARY_DEBUG )	
ENDIF ( Boost_USE_THREAD )

#-- Add the library to the Boost_LIBRARIES variable if found and the user asked for it
IF ( Boost_USE_UNIT_TEST_FRAMEWORK )
  #-- Set the Release and Debug library variables for UNIT_TEST_FRAMEWORK
  SET (Boost_UNIT_TEST_FRAMEWORK_NAME "unit_test_framework")

  #-- Try to find the release version of the unit_test_framework library
  FIND_LIBRARY(Boost_UNIT_TEST_FRAMEWORK_LIBRARY "${BOOST}_${Boost_UNIT_TEST_FRAMEWORK_NAME}${Boost_COMPILER}${Boost_VERSION}" 
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_UNIT_TEST_FRAMEWORK_LIBRARY)

  #-- Try to find the debug version of the unit_test_framework library
  FIND_LIBRARY(Boost_UNIT_TEST_FRAMEWORK_LIBRARY_DEBUG "${BOOST}_${Boost_UNIT_TEST_FRAMEWORK_NAME}${Boost_COMPILER}${Boost_DEBUG}${Boost_VERSION}"
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_UNIT_TEST_FRAMEWORK_LIBRARY_DEBUG)

  #-- Check and add the release version of the library
  IF ( Boost_UNIT_TEST_FRAMEWORK_LIBRARY )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        optimized "${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_UNIT_TEST_FRAMEWORK_LIBRARY )
  #-- Check and add the debug version of the library
  IF ( Boost_UNIT_TEST_FRAMEWORK_LIBRARY_DEBUG )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        debug "${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_DEBUG}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_UNIT_TEST_FRAMEWORK_LIBRARY_DEBUG )	
ENDIF ( Boost_USE_UNIT_TEST_FRAMEWORK )

#-- Add the library to the Boost_LIBRARIES variable if found and the user asked for it
IF ( Boost_USE_WAVE )
  #-- Set the Release and Debug library variables for WAVE
  SET (Boost_WAVE_NAME "wave")

  #-- Try to find the release version of the wave library
  FIND_LIBRARY(Boost_WAVE_LIBRARY "${BOOST}_${Boost_WAVE_NAME}${Boost_COMPILER}${Boost_VERSION}" 
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_WAVE_LIBRARY)

  #-- Try to find the debug version of the wave library
  FIND_LIBRARY(Boost_WAVE_LIBRARY_DEBUG "${BOOST}_${Boost_WAVE_NAME}${Boost_COMPILER}${Boost_DEBUG}${Boost_VERSION}"
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_WAVE_LIBRARY_DEBUG)

  #-- Check and add the release version of the library
  IF ( Boost_WAVE_LIBRARY )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        optimized "${Boost_WAVE_LIBRARY}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_WAVE_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_WAVE_LIBRARY )
  #-- Check and add the debug version of the library
  IF ( Boost_WAVE_LIBRARY_DEBUG )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        debug "${Boost_WAVE_LIBRARY_DEBUG}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_WAVE_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_WAVE_LIBRARY_DEBUG )	
ENDIF ( Boost_USE_WAVE )

#-- Add the library to the Boost_LIBRARIES variable if found and the user asked for it
IF ( Boost_USE_WSERIALIZATION )
  #-- Set the Release and Debug library variables for WSERIALIZATION
  SET (Boost_WSERIALIZATION_NAME "wserialization")

  #-- Try to find the release version of the wserialization library
  FIND_LIBRARY(Boost_WSERIALIZATION_LIBRARY "${BOOST}_${Boost_WSERIALIZATION_NAME}${Boost_COMPILER}${Boost_VERSION}" 
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_WSERIALIZATION_LIBRARY)

  #-- Try to find the debug version of the wserialization library
  FIND_LIBRARY(Boost_WSERIALIZATION_LIBRARY_DEBUG "${BOOST}_${Boost_WSERIALIZATION_NAME}${Boost_COMPILER}${Boost_DEBUG}${Boost_VERSION}"
      ${Boost_LIBRARY_DIR}
  )
  MARK_AS_ADVANCED(Boost_WSERIALIZATION_LIBRARY_DEBUG)

  #-- Check and add the release version of the library
  IF ( Boost_WSERIALIZATION_LIBRARY )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        optimized "${Boost_WSERIALIZATION_LIBRARY}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_WSERIALIZATION_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_WSERIALIZATION_LIBRARY )
  #-- Check and add the debug version of the library
  IF ( Boost_WSERIALIZATION_LIBRARY_DEBUG )
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES}
        debug "${Boost_WSERIALIZATION_LIBRARY_DEBUG}"
        )
    ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(Boost_LIBRARIES ${Boost_LIBRARIES} "${Boost_WSERIALIZATION_LIBRARY}")
    ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
  ENDIF ( Boost_WSERIALIZATION_LIBRARY_DEBUG )	
ENDIF ( Boost_USE_WSERIALIZATION )

