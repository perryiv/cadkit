PROJECT (HeliosAnimate)

set(COMPILE_GUARD "_COMPILING_Animated")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" "${HELIOS_INCLUDE_DIR}/GenericNurbs")

SET (SOURCES
	Movie.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(HeliosAnimate SHARED ${SOURCES} )


# Link the Library	
TARGET_LINK_LIBRARIES(HeliosAnimate
	Usul
	OsgTools
  ${OPENTHREADS_LIB}
  ${OSG_LIB}
)
