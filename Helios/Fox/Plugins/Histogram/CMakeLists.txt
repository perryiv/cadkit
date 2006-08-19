project (FoxHistogram)

ADD_DEFINITIONS ("-D_COMPILING_HISTOGRAM")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" "${FOX_INCLUDE}" "${GENERIC_NURBS_INCLUDE_DIR}" )

set (SOURCES
HistogramFactory.cpp
HistogramComponent.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(FoxHistogram SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxHistogram
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (FoxHistogram
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(FoxHistogram 
  Usul 
  FoxTools
	FoxViews
  ${FOXTOOLKIT_LIB}
)

