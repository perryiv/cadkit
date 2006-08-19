project (FoxVolumeDelegate)

set(COMPILE_GUARD "_COMPILING_VOLUME_DELEGATE")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" "${FOX_INCLUDE}" "${GENERIC_NURBS_INCLUDE_DIR}" )

set (SOURCES
	VolumeDelegateFactory.cpp
	VolumeDelegateComponent.cpp
	SettingsDialog.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(FoxVolumeDelegate SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxVolumeDelegate
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxVolumeDelegate
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(FoxVolumeDelegate 
  Usul
  FoxViews
  FoxTools
  ${FOXTOOLKIT_LIB}
)

