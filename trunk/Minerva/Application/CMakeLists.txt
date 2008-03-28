
PROJECT(MinervaQt)

SET(CMakeModules "$ENV{CADKIT_INC_DIR}/CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

INCLUDE ( ${CMakeModules}/FindOSG.cmake)

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )


# ------------ Set Include Folders ----------------------
INCLUDE_DIRECTORIES(
        ${CADKIT_INC_DIR}
       "${CADKIT_INC_DIR}/Experimental"
        )

#List the Sources
SET (SOURCES
    main.cpp
)

SET ( TARGET MinervaQt )

# Create an executable
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
SET ( TARGET ${TARGET}d )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

ADD_EXECUTABLE( ${TARGET} ${SOURCES} )

# Link the Library	
LINK_CADKIT( ${TARGET} Usul OpenThreadsEx HeliosQtCore )

TARGET_LINK_LIBRARIES( ${TARGET}
   ${QT_LIBRARIES}
   ${OPENTHREADS_LIB}
)


SET( IconDir ${EXECUTABLE_OUTPUT_PATH}/icons )
FILE(GLOB PngFiles "${CMAKE_CURRENT_SOURCE_DIR}/../Images/*.png" )
FILE(GLOB JpgFiles "${CMAKE_CURRENT_SOURCE_DIR}/../Images/*.jpg" )

SET ( IconFiles ${PngFiles} ${JpgFiles} )

MAKE_DIRECTORY( ${IconDir} )
FOREACH(FILE ${IconFiles})
ADD_CUSTOM_COMMAND(
        SOURCE MinervaQt
        COMMAND ${CCOMMAND_COMMAND}
        ARGS cp ${FILE}  ${IconDir}
        TARGET MinervaQt )
ENDFOREACH(FILE ${IconFiles})
