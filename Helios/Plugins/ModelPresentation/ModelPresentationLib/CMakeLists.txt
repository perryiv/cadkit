
PROJECT(ModelPresentationLib)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${OSG_INC_DIR} 
                     ${CADKIT_INC_DIR} 
                     "${CADKIT_INC_DIR}/Experimental"
                     "${PROJECT_SOURCE_DIR}/../../../" 
                      )


# Add definitions
ADD_DEFINITIONS ("-D_COMPILING_MODEL_PRESENTATION_LIB")

# List the Sources.
SET (SOURCES
./DrtSim/Agent.cpp
./DrtSim/Stock.cpp
./DrtSim/DRTSimReader.cpp
./DrtSim/Area.cpp
./DrtSim/Spore.cpp
./DrtSim/Transhipment.cpp
./MpdWriter.cpp
./ModelPresentationLib.cpp

)

# Create a Shared Library
ADD_LIBRARY( ModelPresentationLib SHARED ${SOURCES} )
CADKIT_ADD_LIBRARY ( ModelPresentationLib )

# Link the Library	
LINK_CADKIT( ModelPresentationLib Usul OsgTools )

TARGET_LINK_LIBRARIES( ModelPresentationLib
   ${OSG_LIB}
   ${OSG_DB_LIB}
   ${OSG_TEXT_LIB}
   ${OPENTHREADS_LIB}
)
