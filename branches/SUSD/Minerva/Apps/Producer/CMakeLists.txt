
PROJECT(MinervaProducer)

# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)

INCLUDE( $ENV{USUL_INC_DIR}/Usul/Make/FindOSG.cmake )
INCLUDE( $ENV{USUL_INC_DIR}/Usul/Make/FindProducer.cmake )
INCLUDE( $ENV{USUL_INC_DIR}/Usul/Make/FindOSSIM.cmake )

# ------------ Find Boost ----------------------
SET (Boost_USE_DATE_TIME TRUE)
SET ( Boost_USE_SERIALIZATION TRUE )
INCLUDE($ENV{USUL_INC_DIR}/Usul/Make/FindBoost.cmake)

# ------------ Set Include Folders ----------------------
# Include the Top Level Directory
INCLUDE_DIRECTORIES( 
		     "$ENV{USUL_INC_DIR}" 
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     ${PRODUCER_INC_DIR} 
		     $ENV{PQXX_INC_DIR}
		     ${OSSIM_INC_DIR}
		     ${OSSIM_PLANET_INC_DIR}
		     )


LINK_DIRECTORIES( $ENV{USUL_BIN_DIR} )

# setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
	$ENV{USUL_BIN_DIR}
	CACHE PATH
	"SINGLE Directory for all Executables."
)


#List the Sources
SET (SOURCES
    DrawCallbackImpl.cpp
    main.cpp
)

# Create a Shared Library
ADD_EXECUTABLE(MinervaProducer ${SOURCES} )

# Link the Library	
TARGET_LINK_LIBRARIES(MinervaProducer 
  Usul
  OpenThreadsEx
  OsgTools
  Minerva
  Magrathea
  ${OPENTHREADS_LIB}
  ${Boost_LIBRARIES}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSG_GA_LIB}
	${OSG_TEXT_LIB}
	${OSG_UTIL_LIB}
	${OPENGL_LIBRARIES}
	${OSG_PRODUCER_LIB}
	${PRODUCER_LIB}
	${OSSIM_LIB}
	${OSSIM_PLANET_LIB}
)

