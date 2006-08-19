
PROJECT(Helios)


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../../../"  "${FOXTOOLKIT_INCLUDE}" ${OSG_INC_DIR} ${OT_INC_DIR})

#List the Sources
SET (SOURCES
    main.cpp
)

# Create a Shared Library
ADD_EXECUTABLE(Helios ${SOURCES} )

# Link the Library	
TARGET_LINK_LIBRARIES(Helios 
  Usul
  OpenThreadsEx
  FoxTools
  OsgTools
  FoxViews
  FoxCore
  ${FOXTOOLKIT_LIB}
  ${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSG_GA_LIB}
	${OSG_TEXT_LIB}
	${OSG_UTIL_LIB}
	${OPENGL_LIBRARIES}
)

