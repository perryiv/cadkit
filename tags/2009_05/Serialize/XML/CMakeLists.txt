
PROJECT(SerializeXML)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

ADD_DEFINITIONS("-D_COMPILING_SERIALIZE_XML")

INCLUDE ( ${CMakeModules}/FindXerces.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} ${XERCESC_INCLUDE_DIR} "${PROJECT_SOURCE_DIR}/../../" )

SET ( HEADERS
./BuiltInType.h
./DataMemberMap.h
./Deserialize.h
./Export.h
./Macros.h
./MemberBase.h
./PointerMapMember.h
./QueryPointerMember.h
./SequenceMember.h
./Serialize.h
./SimpleDataMember.h
./SmartPointer.h
./SmartPointerMember.h
./StandardString.h
./TypeWrapper.h
./ValueMapMember.h
)

#List the Sources
SET (SOURCES
    DataMemberMap.cpp
    MemberBase.cpp
)

# Create a Shared Library
ADD_LIBRARY(SerializeXML SHARED ${SOURCES} ${HEADERS} )
CADKIT_ADD_LIBRARY(SerializeXML)

# Link the Library
LINK_CADKIT(SerializeXML Usul XmlTree )

TARGET_LINK_LIBRARIES(SerializeXML
	${XERCESC_LIBRARY}
)
