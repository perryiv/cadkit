

set(VRJ_INC_DIR "$ENV{VJ_BASE_DIR}/include")

################################################################
#
#  Find VR Juggler libraries.
#
################################################################

FIND_LIBRARY(VRJ_LIB vrj 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib
)

if(VRJ_LIB)
  SET(VR_JUGGLER_LIBS ${VRJ_LIB})
endif(VRJ_LIB)

FIND_LIBRARY(VRJ_OGL_LIB vrj_ogl 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib
)

if(VRJ_OGL_LIB)
  SET(VR_JUGGLER_LIBS ${VR_JUGGLER_LIBS} ${VRJ_OGL_LIB})
endif(VRJ_OGL_LIB)

FIND_LIBRARY(SONIX_LIB sonix 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib
)

if(SONIX_LIB)
  SET(VR_JUGGLER_LIBS ${VR_JUGGLER_LIBS} ${SONIX_LIB})
endif(SONIX_LIB)

FIND_LIBRARY(VPR_LIB vpr 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib
)

if(VPR_LIB)
  SET(VR_JUGGLER_LIBS ${VR_JUGGLER_LIBS} ${VPR_LIB})
endif(VPR_LIB)

FIND_LIBRARY(GADGET_LIB gadget 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib
)

if(GADGET_LIB)
  SET(VR_JUGGLER_LIBS ${VR_JUGGLER_LIBS} ${GADGET_LIB})
endif(GADGET_LIB)

FIND_LIBRARY(JCCL_LIB jccl 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib
)

if(JCCL_LIB)
  SET(VR_JUGGLER_LIBS ${VR_JUGGLER_LIBS} ${JCCL_LIB})
endif(JCCL_LIB)


FIND_LIBRARY(TWEEK_LIB tweek 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib
)

if(TWEEK_LIB)
  SET(VR_JUGGLER_LIBS ${VR_JUGGLER_LIBS} ${TWEEK_LIB})
endif(TWEEK_LIB)


################################################################
#
#  Find Omni libraries.
#
################################################################

FIND_LIBRARY(OMNI_CODE_SETS_LIB omniCodeSets4 
  "$ENV{VJ_DEPS_DIR}/lib64"
  "$ENV{VJ_DEPS_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib
)

if(OMNI_CODE_SETS_LIB)
  SET(OMNI_LIBS ${OMNI_CODE_SETS_LIB})
endif(OMNI_CODE_SETS_LIB)


FIND_LIBRARY(OMNI_CONNECTION_MGMT_LIB omniConnectionMgmt4 
  "$ENV{VJ_DEPS_DIR}/lib64"
  "$ENV{VJ_DEPS_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib
)

if(OMNI_CONNECTION_MGMT_LIB)
  SET(OMNI_LIBS ${OMNI_LIBS} ${OMNI_CONNECTION_MGMT_LIB})
endif(OMNI_CONNECTION_MGMT_LIB)



FIND_LIBRARY(OMNI_DYNAMIC_LIB omniDynamic4 
  "$ENV{VJ_DEPS_DIR}/lib64"
  "$ENV{VJ_DEPS_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib
)

if(OMNI_DYNAMIC_LIB)
  SET(OMNI_LIBS ${OMNI_LIBS} ${OMNI_DYNAMIC_LIB})
endif(OMNI_DYNAMIC_LIB)


FIND_LIBRARY(OMNI_ORB_LIB omniORB4
  "$ENV{VJ_DEPS_DIR}/lib64"
  "$ENV{VJ_DEPS_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib
)

if(OMNI_ORB_LIB)
  SET(OMNI_LIBS ${OMNI_LIBS} ${OMNI_ORB_LIB})
endif(OMNI_ORB_LIB)


FIND_LIBRARY(OMNI_THREAD_LIB omnithread 
  "$ENV{VJ_DEPS_DIR}/lib64"
  "$ENV{VJ_DEPS_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib
)

if(OMNI_THREAD_LIB)
  SET(OMNI_LIBS ${OMNI_LIBS} ${OMNI_THREAD_LIB})
endif(OMNI_THREAD_LIB)


################################################################
#
#  Find cppdom.
#
################################################################

FIND_LIBRARY( CPPDOM_LIB cppdom 
  "$ENV{VJ_DEPS_DIR}/lib64"
  "$ENV{VJ_DEPS_DIR}/lib"
  "$ENV{CPPDOM_ROOT}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib
)

# Look for a root installation
FIND_PATH(CPPDOM_ROOT_DIR include/cppdom/cppdom.h
	/usr
	/usr/local
	"$ENV{VJ_DEPS_DIR}/include"
	"$ENV{CPPDOM_ROOT}/include"
	DOC "The root of an installed cppdom installation"
)

IF(CPPDOM_ROOT_DIR AND CPPDOM_LIB)
	MESSAGE (STATUS "Found Cppdom: ${CPPDOM_LIB}")
	SET(VJ_DEPS_LIBS ${VJ_DEPS_LIBS} ${CPPDOM_LIB})
	SET(CPPDOM_INC_DIR "{CPPDOM_ROOT_DIR}/include")
ENDIF(CPPDOM_ROOT_DIR AND CPPDOM_LIB)

