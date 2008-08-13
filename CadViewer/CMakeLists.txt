
cmake_minimum_required(VERSION "2.4")

PROJECT(CadViewer)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")

SET ( CADKIT_INC_DIR ${PROJECT_SOURCE_DIR}/../ )

# Path to look for .cmake files.
SET(CMAKE_MODULE_PATH "${CADKIT_INC_DIR}/CMakeModules" )

INCLUDE ( ${CMakeModules}/Cadkit.cmake )

# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)

INCLUDE( FindOSG )
INCLUDE( FindBoost )
INCLUDE( FindOSSIM )

SUBDIRS(
${CADKIT_INC_DIR}/Usul 
${CADKIT_INC_DIR}/Threads/OpenThreads
${CADKIT_INC_DIR}/OsgTools
${CADKIT_INC_DIR}/XmlTree
${CADKIT_INC_DIR}/Serialize/XML
${CADKIT_INC_DIR}/MenuKit
${CADKIT_INC_DIR}/MenuKit/OSG
${CADKIT_INC_DIR}/VRV
${CADKIT_INC_DIR}/CadViewer/Plugins/scene_operations
${CADKIT_INC_DIR}/CadViewer/Plugins/display_scene_stats

${CADKIT_INC_DIR}/Helios/Plugins/PathAnimation
${CADKIT_INC_DIR}/Helios/Plugins/CenterGeometry
${CADKIT_INC_DIR}/Helios/Plugins/MeasureTool
${CADKIT_INC_DIR}/Minerva/Plugins/EsriShapeFile

# ---- Document Plug-ins -------
${CADKIT_INC_DIR}/Helios/Plugins/SceneModel
${CADKIT_INC_DIR}/Helios/Plugins/TriangleModel
${CADKIT_INC_DIR}/Experimental/WRF/WrfModel

${CADKIT_INC_DIR}/Helios/Plugins/ModelPresentation/ModelPresentation
${CADKIT_INC_DIR}/Helios/Plugins/ModelPresentation/ModelPresentationLib

# ---- Minerva -------
${CADKIT_INC_DIR}/Minerva/Core
${CADKIT_INC_DIR}/Minerva/DataSources/PG
${CADKIT_INC_DIR}/Minerva/Document

${CADKIT_INC_DIR}/Minerva/Layers/PostGIS
${CADKIT_INC_DIR}/Minerva/Layers/GDAL
${CADKIT_INC_DIR}/Minerva/Layers/Kml

# ---- Minerva Plug-ins -------
${CADKIT_INC_DIR}/Minerva/Plugins/Document
${CADKIT_INC_DIR}/Minerva/Plugins/GDALReadImage
)

#---- ModFlow ---
IF ("$ENV{MODFLOW_INC_DIR}")
   SUBDIRS(  $ENV{MODFLOW_INC_DIR}/ModflowModel )
ENDIF ( "$ENV{MODFLOW_INC_DIR}")

# Find VR Juggler
INCLUDE( FindVRJuggler )

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
		     )

#List the Sources
SET (SOURCES
./main.cpp
./Application.cpp
)


SET ( TARGET cv )

# Create an executable
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
SET ( TARGET ${TARGET}d )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

ADD_EXECUTABLE( ${TARGET} ${SOURCES} )

# Link the Library	
LINK_CADKIT( ${TARGET} Usul OpenThreadsEx OsgTools VRV MenuKit MenuKit_OSG XmlTree SerializeXML )
TARGET_LINK_LIBRARIES( ${TARGET} 
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
