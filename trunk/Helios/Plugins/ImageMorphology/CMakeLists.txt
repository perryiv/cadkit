project (ImageMorphology)

set(COMPILE_GUARD "_COMPILING_IMAGE_MORPHOLOGY")
ADD_DEFINITIONS ("-D${COMPILE_GUARD}")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${HELIOS_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/../" )

set (SOURCES
	SmoothComponent.cpp
	ImageMorphologyFactory.cpp
	ErodeComponent.cpp
	DilateComponent.cpp
)

# ---------- Create a Shared Library
ADD_LIBRARY(ImageMorphology SHARED ${SOURCES} )

# ---- Change the Suffix on the Generated Dylib --------------------------------
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (ImageMorphology
	PROPERTIES SUFFIX ".plugd"
 )
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
 SET_TARGET_PROPERTIES (ImageMorphology
	PROPERTIES SUFFIX ".plug"
 )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Link the Library	
TARGET_LINK_LIBRARIES(ImageMorphology 
  Usul
	OsgTools
)

