# This file should be at CadKit/Helios/CMakeLists.txt

cmake_minimum_required(VERSION "2.6")

PROJECT(HELIOS)

INCLUDE(CPack)
SET ( CPACK_PACKAGE_NAME Helios )

#########################################################
#
#  Options to configure which libraries get built.
#
#########################################################

OPTION ( BUILD_MINERVA "Build Minerva libraries."  ON )
OPTION ( BUILD_HELIOS_VR "Build VR components of Helios framework." OFF )
OPTION ( BUILD_HELIOS_QT "Build Qt components of Helios framework." ON )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

# Set variables needed below...
SET ( CADKIT_INC_DIR ${PROJECT_SOURCE_DIR}/../ )
SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")

# Path to look for .cmake files.
SET( CMAKE_MODULE_PATH ${CMakeModules} )


#########################################################
#
#  Bring in our helper macros and variables.
#
#########################################################

INCLUDE ( ${CMakeModules}/Cadkit.cmake )


#########################################################
#
#  Find Qt.
#
#########################################################

SET ( QT_MAX_VERSION "4.9.9" )
SET ( QT_USE_OPENGL TRUE )

FIND_PACKAGE(Qt4)

ADD_DEFINITIONS(${QT_DEFINITIONS})


#########################################################
#
#  Find OpenGL.
#
#########################################################

FIND_PACKAGE(OpenGL)


#########################################################
#
#  Find OpenSceneGraph.
#
#########################################################

INCLUDE( FindOSG )


#########################################################
#
#  Find boost.
#
#########################################################

SET ( BOOST_INCLUDEDIR $ENV{BOOST_INC_DIR} )
SET ( BOOST_LIBRARYDIR $ENV{BOOST_LIB_DIR} )
FIND_PACKAGE( Boost 1.36 COMPONENTS date_time filesystem regex threads system )


#########################################################
#
#  Versions of boost prior to 1.35 had the system 
#  library rolled into the file-system library.
#
#########################################################

IF ( ${Boost_VERSION} LESS "103500" )
SET ( Boost_SYSTEM_LIBRARY ${Boost_FILESYSTEM_LIBRARY} )
ENDIF ( ${Boost_VERSION} LESS "103500" )


#########################################################
#
#  Find GDAL.
#
#########################################################

INCLUDE( FindGDAL )


#########################################################
#
#  Find Ossim.
#
#########################################################

INCLUDE( FindOSSIM )


#########################################################
#
#  Find HDF 5.
#
#########################################################

INCLUDE( FindHDF )


#########################################################
#
#  Find pqxx.
#
#########################################################

INCLUDE( FindPQXX )
INCLUDE( FindPostgreSQL )


#########################################################
#
#  Are these variables needed still?
#
#########################################################

SET (HELIOS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../ )
SET (GENERIC_NURBS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../GenericNurbs)


#########################################################
#
#  Set some Apple specific properties.
#
#########################################################

IF (APPLE)
	
	option(OSX_DWARF_DEBUG "Use Dwarf Debug Style" ON)
	IF (OSX_DWARF_DEBUG )
		SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-2")
	ENDIF(OSX_DWARF_DEBUG )

	# CMake doesn't define _DEBUG when using Xcode.
	IF(${CMAKE_GENERATOR} MATCHES "Xcode")
		SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG" )
	ENDIF(${CMAKE_GENERATOR} MATCHES "Xcode")
	
ENDIF (APPLE)


#########################################################
#
#  Start adding the sub projects.
#
#########################################################

SUBDIRS(

# include base libraries.
${PROJECT_SOURCE_DIR}/../Usul 
${PROJECT_SOURCE_DIR}/../Threads/OpenThreads
${PROJECT_SOURCE_DIR}/../OsgTools 
${PROJECT_SOURCE_DIR}/../XmlTree 
${PROJECT_SOURCE_DIR}/../Serialize/XML
${PROJECT_SOURCE_DIR}/../MenuKit

# include plugins.
${PROJECT_SOURCE_DIR}/Plugins/SceneModel
${PROJECT_SOURCE_DIR}/Plugins/TriangleModel
${PROJECT_SOURCE_DIR}/Plugins/VolumeModel
${PROJECT_SOURCE_DIR}/Plugins/MatrixAnimation
${PROJECT_SOURCE_DIR}/Plugins/PathAnimation
${PROJECT_SOURCE_DIR}/Plugins/ModelPresentation/ModelPresentationLib
${PROJECT_SOURCE_DIR}/Plugins/ModelPresentation/ModelPresentation
${PROJECT_SOURCE_DIR}/../Experimental/WRF/WrfModel
${CADKIT_INC_DIR}/Helios/Plugins/MeasureTool
${CADKIT_INC_DIR}/Helios/Plugins/CenterGeometry
)

# Build these projects if we found HDF
IF ( HDF_FOUND )
	SUBDIRS ( ${PROJECT_SOURCE_DIR}/../Experimental/Flash/FlashModel )
ENDIF ( HDF_FOUND )

# Build these projects if we found GDAL.
IF(GDAL_FOUND)
	SUBDIRS(
	${CADKIT_INC_DIR}/Minerva/Plugins/EsriShapeFile
	${CADKIT_INC_DIR}/Nimue/Plugins/ModflowModel
	)
ENDIF(GDAL_FOUND)


# Build Minerva libraries if we should.
IF(BUILD_MINERVA)
	SUBDIRS ( ${CADKIT_INC_DIR}/Minerva )
ENDIF(BUILD_MINERVA)


# Include the QT Cmake files.
IF(QT_FOUND AND BUILD_HELIOS_QT)
	SUBDIRS (
	  ${PROJECT_SOURCE_DIR}/Qt 
	  ${CADKIT_INC_DIR}/Helios/Qt/Plugins/PasswordPrompt
	)
ENDIF(QT_FOUND AND BUILD_HELIOS_QT)

# Should we build the VR components?
IF(BUILD_HELIOS_VR)

	# Find VR Juggler
	INCLUDE( FindVRJuggler )

	SUBDIRS(
		${CADKIT_INC_DIR}/VRV
		${CADKIT_INC_DIR}/MenuKit/OSG
		${CADKIT_INC_DIR}/CadViewer/Plugins/scene_operations
		${CADKIT_INC_DIR}/CadViewer/Plugins/display_scene_stats
		${CADKIT_INC_DIR}/CadViewer
	)
ENDIF(BUILD_HELIOS_VR)


# Install 3rd party dependencies.
#INSTALL ( FILES ${QT_QTCORE_LIBRARY_RELEASE}
#                ${QT_QTGUI_LIBRARY_RELEASE}
#                ${QT_QTOPENGL_LIBRARY_RELEASE}
#	     		 ${OSG_LIB} 
#                ${OSG_DB_LIB} 
#                ${OSG_GA_LIB} 
#                ${OSG_TEXT_LIB}
#                ${OSG_UTIL_LIB}
#                ${OSG_MANIPULATOR}
#                ${OPENTHREADS_LIB}
#  DESTINATION bin
#)
