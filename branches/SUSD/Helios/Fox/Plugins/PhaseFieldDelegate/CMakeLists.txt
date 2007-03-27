project (FoxPhaseFieldDelegate)

ADD_DEFINITIONS ("-D_COMPILING_PHASEFIELDDELEGATE")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" "${FOXTOOLKIT_INCLUDE}" "${GENERIC_NURBS_INCLUDE_DIR}" )

set (SOURCES
	PhaseFieldDelegateFactory.cpp
	PhaseFieldDelegateComponent.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(FoxPhaseFieldDelegate SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxPhaseFieldDelegate
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxPhaseFieldDelegate
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(FoxPhaseFieldDelegate 
  Usul 
  FoxTools
  OsgTools
  ${OPENTHREADS_LIB}
	${OSG_LIB}
  FoxViews
  ${FOXTOOLKIT_LIB}
)

