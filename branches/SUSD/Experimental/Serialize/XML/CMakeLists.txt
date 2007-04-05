
PROJECT(SerializeXML)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

ADD_DEFINITIONS("-D_COMPILING_SERIALIZE_XML")

INCLUDE ( ${CMakeModules}/FindXerces.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} ${XERCESC_INC_DIR} "${PROJECT_SOURCE_DIR}/../../" )


#List the Sources
SET (SOURCES
    Factory.cpp
    DataMemberMap.cpp
    BaseCreator.cpp
    MemberBase.cpp
)

# Create a Shared Library
ADD_LIBRARY(SerializeXML SHARED ${SOURCES} )

# Link the Library
LINK_CADKIT(SerializeXML Usul XmlTree )

TARGET_LINK_LIBRARIES(SerializeXML
	${XERCESC_LIBRARY}
)
