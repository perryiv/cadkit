
PROJECT(Helios)

SET (CMAKE_BUILD_TYPE Debug)
SET (CMAKE_SKIP_RPATH OFF)


#setup output Directories
SET (LIBRARY_OUTPUT_PATH
${PROJECT_SOURCE_DIR}/../../Bin/${CMAKE_BUILD_TYPE}
CACHE PATH
"Single Directory for all Libraries"
)

#setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
${PROJECT_SOURCE_DIR}/../../Bin/${CMAKE_BUILD_TYPE}
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
	${PROJECT_SOURCE_DIR}/../../Bin/${CMAKE_BUILD_TYPE}
	/usr/local/lib
)

# ------------ Find FoxTools Library ----------------------
FIND_LIBRARY (FOXTOOLS_LIBRARY FoxTools
	${PROJECT_SOURCE_DIR}/../../Bin/${CMAKE_BUILD_TYPE}
	/usr/local/lib
)

# ------------ Find OsgTools Library ----------------------
FIND_LIBRARY (OSGTOOLS_LIBRARY OsgTools
	${PROJECT_SOURCE_DIR}/../../Bin/${CMAKE_BUILD_TYPE}
	/usr/local/lib
)

# ------------ Find FoxCore Library ----------------------
FIND_LIBRARY (FOXCORE_LIBRARY FoxCore
	${PROJECT_SOURCE_DIR}/../../Bin/${CMAKE_BUILD_TYPE}
	/usr/local/lib
)

# ------------ Find FoxView Library ----------------------
FIND_LIBRARY (FOXVIEW_LIBRARY FoxView
	${PROJECT_SOURCE_DIR}/../../Bin/${CMAKE_BUILD_TYPE}
	/usr/local/lib
)

# ------------ Find Threads Library ----------------------
FIND_LIBRARY (THREADS_LIBRARY Threads
	${PROJECT_SOURCE_DIR}/../../Bin/${CMAKE_BUILD_TYPE}
	/usr/local/lib
)

# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)

# ------------ Find OpenThreads Library ----------------------
FIND_LIBRARY(OPENTHREADS_LIB OpenThreads
	/usr/local/lib
	/usr/lib
)
# ------------ Find osg Library ----------------------
FIND_LIBRARY(OSG_LIB osg
	/usr/local/lib
	/usr/lib
)
# ------------ Find osgDB Library ----------------------
FIND_LIBRARY(OSG_DB_LIB osgDB
	/usr/local/lib
	/usr/lib
)
# ------------ Find osgGA Library ----------------------
FIND_LIBRARY(OSG_GA_LIB osgGA
	/usr/local/lib
	/usr/lib
)
# ------------ Find osgText Library ----------------------
FIND_LIBRARY(OSG_TEXT_LIB osgText
	/usr/local/lib
	/usr/lib
)
# ------------ Find osgUtil Library ----------------------
FIND_LIBRARY(OSG_UTIL_LIB osgUtil
	/usr/local/lib
	/usr/lib
)



# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../../" )
INCLUDE_DIRECTORIES( "${FOX_INCLUDE}" )

#List the Sources
SET (SOURCES
    main.cpp
)

# Create a Shared Library
ADD_EXECUTABLE(Helios ${SOURCES} )

# Link the Library	
TARGET_LINK_LIBRARIES(Helios 
  ${USUL_LIBRARY}
  ${THREADS_LIBRARY}
  ${FOXTOOLS_LIBRARY}
  ${OSGTOOLS_LIBRARY}
  ${FOXVIEW_LIBRARY}
  ${FOXCORE_LIBRARY}
  ${FOXTOOLKIT_LIB}
  ${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSG_GA_LIB}
	${OSG_TEXT_LIB}
	${OSG_UTIL_LIB}
	${OPENGL_LIBRARIES}
)

