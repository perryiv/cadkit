
PROJECT(CadViewer)

INCLUDE ( ${CMakeModules}/Cadkit.cmake )

# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)

INCLUDE( FindOSG )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( 
		     ${CADKIT_INC_DIR}
		     "${CADKIT_INC_DIR}/CadViewer"
			 "${CADKIT_INC_DIR}/GenericNurbs"
		     ${OSG_INC_DIR} 
		     ${OT_INC_DIR} 
		     ${VR_JUGGLER_INCLUDES}
		     "${VRJ_INC_DIR}/tweek/CORBA"
		     "${VRJ_INC_DIR}/tweek/idl"
		     "$ENV{VJ_DEPS_DIR}/include"
		     ${CPPDOM_INC_DIR}
		     ${GMTL_INC_DIR}
		     ${Boost_INCLUDE_DIR}
		     )

#List the Sources
SET (SOURCES
./main.cpp
./Application.cpp
)

IF(WIN32)
SET ( TARGET_NAME CadViewer )
ELSE(WIN32)
SET ( TARGET_NAME cv )
ENDIF(WIN32)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
SET ( TARGET_NAME ${TARGET_NAME}d )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Create an executable
ADD_EXECUTABLE( ${TARGET_NAME} ${SOURCES} )

# Link the Library	
LINK_CADKIT( ${TARGET_NAME} Usul OpenThreadsEx OsgTools VRV MenuKit MenuKit_OSG XmlTree SerializeXML )
TARGET_LINK_LIBRARIES( ${TARGET_NAME} 
  ${VR_JUGGLER_LIBS}
  ${VJ_DEPS_LIBS}
  ${OPENTHREADS_LIB}
  ${OSG_LIB}
  ${OSG_DB_LIB}
  ${OSG_GA_LIB}
  ${OSG_TEXT_LIB}
  ${OSG_UTIL_LIB}
  ${OSG_FX_LIB}
  ${OPENGL_LIBRARIES}
)


INSTALL(TARGETS ${TARGET_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin
  ARCHIVE DESTINATION bin
)
