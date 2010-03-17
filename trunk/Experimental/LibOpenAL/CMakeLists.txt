
PROJECT(LibOpenAL)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${OSG_INC_DIR} 
                     ${CADKIT_INC_DIR} 
                     "${CADKIT_INC_DIR}/Experimental"
                     "${PROJECT_SOURCE_DIR}/../../../" 
		     ${Boost_INCLUDE_DIR}
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
./LibOpenAL.cpp

)

# Create a Shared Library
ADD_LIBRARY( LibOpenAL SHARED ${SOURCES} )
CADKIT_ADD_LIBRARY ( LibOpenAL )

# Link the Library	
ADD_DEPENDENCIES( LibOpenAL Usul CadKitOsgTools )

TARGET_LINK_LIBRARIES( LibOpenAL
   ${OSG_LIB}
   ${OSG_DB_LIB}
   ${OSG_TEXT_LIB}
   ${OPENTHREADS_LIB}
)
