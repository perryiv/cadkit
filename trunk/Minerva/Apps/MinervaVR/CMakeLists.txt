
PROJECT(MinervaVR)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)


# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)

INCLUDE(${CMakeModules}/FindOSG.cmake)
INCLUDE(${CMakeModules}/FindProducer.cmake)
INCLUDE(${CMakeModules}/FindOSSIM.cmake)
INCLUDE(${CMakeModules}/FindXerces.cmake)

# ------------ Find boost ----------------------
SET ( Boost_DATE_TIME_LIBRARY TRUE )
INCLUDE( ${CMakeModules}/FindBoost.cmake)

FIND_LIBRARY(VJCINEMA_LIB VrjCinema
 "$ENV{VRJCINEMA_ROOT_DIR}/lib"
)

INCLUDE(${CMakeModules}/FindVRJuggler.cmake)

SUBDIRS(
${CADKIT_ROOT_DIR}/Usul 
${CADKIT_ROOT_DIR}/Threads/OpenThreads
${CADKIT_ROOT_DIR}/OsgTools
${CADKIT_ROOT_DIR}/Experimental/XmlTree
${CADKIT_ROOT_DIR}/Experimental/Serialize/XML
${CADKIT_ROOT_DIR}/Magrathea
${CADKIT_ROOT_DIR}/Minerva/Core
# ---- PlugIns -------
${CADKIT_ROOT_DIR}/Minerva/Plugins/ComputeNormals
${CADKIT_ROOT_DIR}/Minerva/Plugins/TriangulateCGAL
)

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
		     "${CADKIT_INC_DIR}/Experimental"
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     ${WMS_INC_DIR}
		     ${OSSIM_INC_DIR}
		     ${OSG_EPHEMERIS_INC_DIR}
		     ${OSSIM_PLANET_INC_DIR}
		     $ENV{PQXX_INC_DIR}
		     "$ENV{VRJCINEMA_ROOT_DIR}/src"
		     ${VRJ_INC_DIR}
		     "${VRJ_INC_DIR}/tweek/CORBA"
		     "${VRJ_INC_DIR}/tweek/idl"
		     ${PRODUCER_INC_DIR}
		     ${XERCESC_INCLUDE_DIR}
		     )

#List the Sources
SET (SOURCES
    MinervaVR.cpp
    SharedUpdateData.cpp
    main.cpp
)

# Create a Shared Library
ADD_EXECUTABLE(MinervaVR ${SOURCES} )

# Link the Library	
LINK_CADKIT( MinervaVR Usul OpenThreadsEx OsgTools Magrathea Minerva )
TARGET_LINK_LIBRARIES(MinervaVR 
  ${VJCINEMA_LIB}
  ${VR_JUGGLER_LIBS}
  ${OPENTHREADS_LIB}
  ${OSG_LIB}
  ${OSG_DB_LIB}
  ${OSG_GA_LIB}
  ${OSG_TEXT_LIB}
  ${OSG_UTIL_LIB}
  ${OSG_FX_LIB}
  ${OPENGL_LIBRARIES}
  ${OSSIM_LIB}
  ${OSSIM_PLANET_LIB}
  ${Boost_LIBRARIES}
  ${PQXX_LIB}
  ${XERCESC_LIBRARY}
  ${OMNI_LIBS}
)
