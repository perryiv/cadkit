
PROJECT(FoxViews)

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_COMPILING_OSG_FOX_VIEWS")

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../../../" "${FOX_INCLUDE}" ${OSG_INC_DIR} ${OT_INC_DIR} )

#List the Sources
SET (SOURCES
    Canvas.cpp
    ChartView.cpp
    FoxCanvas.cpp
    FoxContext.cpp
    ImageView.cpp
    LightEditor.cpp
    MaterialEditor.cpp
    Registry.cpp
    TFView.cpp
)

# Create a Shared Library
ADD_LIBRARY(FoxViews SHARED ${SOURCES} )

# Link the Library	
TARGET_LINK_LIBRARIES(FoxViews 
  Usul 
  FoxTools
  OsgTools
  ${FOXTOOLKIT_LIB}
  ${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSG_GA_LIB}
	${OSG_TEXT_LIB}
	${OSG_UTIL_LIB}
	${OPENGL_LIBRARIES}
)

    
   
