
PROJECT(FoxCore)

SET (CMAKE_BUILD_TYPE Debug)
SET (CMAKE_SKIP_RPATH OFF)
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_COMPILING_OSG_FOX_CORE")

#setup output Directories
SET (LIBRARY_OUTPUT_PATH
${PROJECT_SOURCE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}
CACHE PATH
"Single Directory for all Libraries"
)

#setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
${PROJECT_SOURCE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}
CACHE PATH
"SINGLE Directory for all Executables."
)


# ------------ Find FoxToolkit Library and include Dir ----------------------
FIND_LIBRARY(FOXTOOLKIT_LIB FOX-1.4
	/usr/local/lib
	/usr/lib
)

FIND_PATH(FOX_INCLUDE fx.h
    /usr/local/include/fox-1.4
    /usr/include/fox-1.4
)

# ------------ Find Usul Library ----------------------
FIND_LIBRARY (USUL_LIBRARY Usul
	${PROJECT_SOURCE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}
	/usr/local/lib
)

# ------------ Find FoxTools Library ----------------------
FIND_LIBRARY (FOXTOOLS_LIBRARY FoxTools
	${PROJECT_SOURCE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}
	/usr/local/lib
)

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../../../" )
INCLUDE_DIRECTORIES( "${FOX_INCLUDE}" )

#List the Sources
SET (SOURCES
    MainWindow.cpp
    MdiMainWindow.cpp
    Preferences.cpp
)

# Create a Shared Library
ADD_LIBRARY(FoxCore SHARED ${SOURCES} )

# Link the Library	
TARGET_LINK_LIBRARIES(FoxCore ${USUL_LIBRARY} ${FOXTOOLS_LIBRARY} ${FOXTOOLKIT_LIB})

    
   
