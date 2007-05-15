
PROJECT(XmlTree)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

ADD_DEFINITIONS("-D_COMPILING_XML_TREE")

INCLUDE ( ${CMakeModules}/FindXerces.cmake)
INCLUDE ( ${CMakeModules}/FindBoost.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} ${XERCESC_INCLUDE_DIR} ${Boost_INCLUDE_DIR} "${PROJECT_SOURCE_DIR}/../" )


#List the Sources
SET (SOURCES
    Document.cpp
    Node.cpp
    Loader.cpp
    XercesLife.cpp
    Writer.cpp
    Detail/RootImpl.cpp
    Functions.cpp
    ReplaceIllegalCharacters.cpp
)

# Create a Shared Library
ADD_LIBRARY(XmlTree SHARED ${SOURCES} )

# Link the Library
LINK_CADKIT(XmlTree Usul)

TARGET_LINK_LIBRARIES(XmlTree
	${XERCESC_LIBRARY}
)
