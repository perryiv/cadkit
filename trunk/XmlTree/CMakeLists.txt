
#########################################################
#
#  CMakeLists for XmlTree.
#
#########################################################

PROJECT(XmlTree)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

ADD_DEFINITIONS("-D_COMPILING_XML_TREE")

INCLUDE ( ${CMakeModules}/FindXerces.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )


#########################################################
#
#  Add include folders.
#
#########################################################

INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} 
                     ${XERCESC_INCLUDE_DIR} 
					 ${Boost_INCLUDE_DIR}
					 "${PROJECT_SOURCE_DIR}/../" )


#########################################################
#
#  Add source directory.
#
#########################################################

SOURCE_GROUP ( Source REGULAR_EXPRESSION ./* )
SOURCE_GROUP ( Detail REGULAR_EXPRESSION ./Detail/* )


#########################################################
#
#  List all the headers.
#
#########################################################

SET ( HEADERS
	#./Detail/RootImpl.h
	./Document.h
	./Export.h
	./Functions.h
	./Loader.h
	./Node.h
	./Predicates.h
	./RegistryBuilder.h
	./RegistryIO.h
	./RegistryVisitor.h
	./ReplaceIllegalCharacters.h
	./Writer.h
	./XercesErrorHandler.h
	./XercesLife.h
	./XercesString.h
)


#########################################################
#
#  List all the sources.
#
#########################################################

SET (SOURCES
    Document.cpp
    Node.cpp
    Loader.cpp
    XercesLife.cpp
    Writer.cpp
    #Detail/RootImpl.cpp
    Functions.cpp
    RegistryBuilder.cpp
    RegistryIO.cpp
    RegistryVisitor.cpp
    ReplaceIllegalCharacters.cpp
)

SET ( TARGET_NAME XmlTree )

# Create a Shared Library
ADD_LIBRARY( ${TARGET_NAME} SHARED ${HEADERS} ${SOURCES} )

# Add a library
CADKIT_ADD_LIBRARY ( ${TARGET_NAME} )

# Link the Library
LINK_CADKIT( ${TARGET_NAME} Usul )

TARGET_LINK_LIBRARIES( ${TARGET_NAME}
	${XERCESC_LIBRARY}
)

#INSTALL( FILES ${XERCESC_LIBRARY}
#  DESTINATION bin
#)
