
PROJECT(SizeInfo)

#List the Sources
SET (SOURCES
    main.cpp
)

SET ( TARGET SizeInfo )


# Create an executable
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
SET ( TARGET ${TARGET}d )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

ADD_EXECUTABLE( ${TARGET} ${SOURCES} )


