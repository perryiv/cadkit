
SET(VR_JUGGLER_VERSION 2.0 CACHE STRING "Enter VR Juggler Version (2.0 or 2.2).")

IF(VR_JUGGLER_VERSION MATCHES 2.0)
  INCLUDE( FindVRJuggler20 )
ENDIF(VR_JUGGLER_VERSION MATCHES 2.0)

IF(VR_JUGGLER_VERSION MATCHES 2.2)
  INCLUDE( FindVRJuggler22 )
ENDIF(VR_JUGGLER_VERSION MATCHES 2.2)

################################################################
#
#  Find cppdom.
#
################################################################

FIND_LIBRARY( CPPDOM_LIB cppdom 
  "$ENV{VJ_DEPS_DIR}/lib64"
  "$ENV{VJ_DEPS_DIR}/lib"
  "$ENV{CPPDOM_ROOT}/lib64"
  "$ENV{CPPDOM_ROOT}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib64
  /usr/lib
)

# Look for a root installation
FIND_PATH(CPPDOM_INC_DIR cppdom/cppdom.h
	/usr/include/
	/usr/local/include/
	/usr/include/cppdom-0.7.10/
	"$ENV{VJ_DEPS_DIR}/include/"
	"$ENV{CPPDOM_ROOT}/include/"
	DOC "The root of an installed cppdom installation"
)

IF(CPPDOM_INC_DIR AND CPPDOM_LIB)
	MESSAGE (STATUS "Found Cppdom: ${CPPDOM_LIB}")
	SET(VJ_DEPS_LIBS ${VJ_DEPS_LIBS} ${CPPDOM_LIB})
ENDIF(CPPDOM_INC_DIR AND CPPDOM_LIB)


# Look for gmtl installation
FIND_PATH( GMTL_INC_DIR gmtl/gmtl.h
	/usr/include/
	/usr/local/include/
	/usr/include/gmtl-0.5.4/
	"$ENV{VJ_DEPS_DIR}/include/"
	"$ENV{GMTL_ROOT}/include/"
	DOC "The root of an installed cppdom installation"
)
