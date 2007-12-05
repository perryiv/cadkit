
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
SET ( Boost_USE_DATE_TIME TRUE )
INCLUDE( ${CMakeModules}/FindBoost.cmake)

FIND_LIBRARY ( VJCINEMA_LIB VrjCinema "$ENV{VRJCINEMA_ROOT_DIR}/lib" )

INCLUDE(${CMakeModules}/FindVRJuggler.cmake)

SUBDIRS(
${CADKIT_INC_DIR}/Usul 
${CADKIT_INC_DIR}/Threads/OpenThreads
${CADKIT_INC_DIR}/OsgTools
${CADKIT_INC_DIR}/XmlTree
${CADKIT_INC_DIR}/Serialize/XML
${CADKIT_INC_DIR}/Magrathea
${CADKIT_INC_DIR}/Minerva/Core
${CADKIT_INC_DIR}/Minerva/Document
${CADKIT_INC_DIR}/VRV
# ---- Plug-ins -------
${CADKIT_INC_DIR}/Minerva/Plugins/ProjectionManager
${CADKIT_INC_DIR}/Minerva/Plugins/ComputeNormals
${CADKIT_INC_DIR}/Minerva/Plugins/TriangulateCGAL
)

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     ${WMS_INC_DIR}
		     ${OSSIM_INC_DIR}
		     ${OSG_EPHEMERIS_INC_DIR}
		     ${OSSIM_PLANET_INC_DIR}
		     $ENV{PQXX_INC_DIR}
		     ${VRJ_INC_DIR}
		     "${VRJ_INC_DIR}/tweek/CORBA"
		     "${VRJ_INC_DIR}/tweek/idl"
		     ${PRODUCER_INC_DIR}
		     ${XERCESC_INCLUDE_DIR}
		     "$ENV{VJ_DEPS_DIR}/include"
         "$ENV{VJCINEMA}/src"
		     )

#List the Sources
SET (SOURCES
    MinervaVR.cpp
    SharedUpdateData.cpp
    main.cpp
)


SET ( TARGET MinervaVR )

# Create an executable
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
SET ( TARGET ${TARGET}d )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

ADD_EXECUTABLE( ${TARGET} ${SOURCES} )

# Link the Library	
LINK_CADKIT( ${TARGET} Usul OpenThreadsEx OsgTools Magrathea Minerva VRV MinervaDocument )
TARGET_LINK_LIBRARIES( ${TARGET} 
  ${VJCINEMA_LIB}
  ${VR_JUGGLER_LIBS}
  ${OPENTHREADS_LIB}
  ${OSG_LIB}
  ${OSG_DB_LIB}
  ${OSG_GA_LIB}
  ${OSG_TEXT_LIB}
  ${OSG_UTIL_LIB}
  ${OSG_FX_LIB}
  ${PRODUCER_LIB}
  ${OPENGL_LIBRARIES}
  ${OSSIM_LIB}
  ${OSSIM_PLANET_LIB}
  ${Boost_LIBRARIES}
  ${PQXX_LIB}
  ${XERCESC_LIBRARY}
  ${OMNI_LIBS}
)
