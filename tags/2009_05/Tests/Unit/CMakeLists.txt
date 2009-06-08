
ADD_SUBDIRECTORY ( Usul )

INCLUDE ( ${CMakeModules}/FindGoogleTest.cmake )
INCLUDE ( ${CMakeModules}/FindOSG.cmake )

IF ( GOOGLE_TEST_FOUND )

	INCLUDE_DIRECTORIES( ${CADKIT_INC_DIR} ${GOOGLE_TEST_INCLUDE_DIR} ${OSG_INC_DIR} ${Boost_INCLUDE_DIR} )

	SET ( SOURCES
		./Main.cpp
		Minerva/Core/TileEngine/TileTest.cpp
		Minerva/Ellipsoid/EllipsoidTest.cpp
		Minerva/Extents/ExtentsTest.cpp
		Usul/Math/BarycentricTest.cpp
		./Usul/System/Process/ProcessTest.cpp
	)

	SET ( TARGET_NAME CadKitUnitTest )

	ADD_EXECUTABLE( ${TARGET_NAME} ${SOURCES} )

	# Add the target label.
	SET_TARGET_PROPERTIES( ${TARGET_NAME} PROPERTIES PROJECT_LABEL "Test: ${TARGET_NAME}" )

	# Add the debug postfix.
	SET_TARGET_PROPERTIES( ${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}" )

	# Link the Library	
	LINK_CADKIT( ${TARGET_NAME} Usul Minerva )
	
	TARGET_LINK_LIBRARIES( ${TARGET_NAME} ${GOOGLE_TEST_LIBRARY} )

	
ENDIF ( GOOGLE_TEST_FOUND )
