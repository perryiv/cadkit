
PROJECT(FoxCore)

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_COMPILING_OSG_FOX_CORE")



# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../../../" "${FOX_INCLUDE}" )

#List the Sources
SET (SOURCES
    MainWindow.cpp
    MdiMainWindow.cpp
    Preferences.cpp
)

# Create a Shared Library
ADD_LIBRARY(FoxCore SHARED ${SOURCES} )

# Link the Library	
TARGET_LINK_LIBRARIES(FoxCore 
  Usul 
  FoxTools
  ${FOXTOOLKIT_LIB}
)

    
   
