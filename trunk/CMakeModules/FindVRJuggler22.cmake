

################################################################
#
#  Find VR Juggler includes.
#
################################################################


# Look for a root installation
FIND_PATH(VRJ_BASE_DIR include/vrjuggler-2.2/vrj/vrjConfig.h
	/usr
	/usr/local
	"$ENV{VJ_BASE_DIR}"
	DOC "The root of an installed vr-juggler installation"
)

IF(VRJ_BASE_DIR)

  SET(VRJ_INC_DIR ${VRJ_BASE_DIR}/include/vrjuggler-2.2)
  SET(VPR_INC_DIR ${VRJ_BASE_DIR}/include/vpr-2.0)
  SET(GADGET_INC_DIR ${VRJ_BASE_DIR}/include/gadgeteer-1.2)
  SET(TWEEK_INC_DIR ${VRJ_BASE_DIR}/include/tweek-1.2)
  SET(SONIX_INC_DIR ${VRJ_BASE_DIR}/include/sonix-1.2)
  SET(JCCL_INC_DIR ${VRJ_BASE_DIR}/include/jccl-1.2)
  
  SET(VR_JUGGLER_INCLUDES ${VRJ_INC_DIR} ${VPR_INC_DIR} ${GADGET_INC_DIR} ${TWEEK_INC_DIR} ${SONIX_INC_DIR} ${JCCL_INC_DIR} )

ENDIF(VRJ_BASE_DIR)

################################################################
#
#  Find VR Juggler libraries.
#
################################################################

FIND_LIBRARY(VRJ_LIB vrj-2_2 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib64
  /usr/lib
)

if(VRJ_LIB)
  SET(VR_JUGGLER_LIBS ${VRJ_LIB})
endif(VRJ_LIB)

FIND_LIBRARY(VRJ_OGL_LIB vrj_ogl-2_2 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib64
  /usr/lib
)

if(VRJ_OGL_LIB)
  SET(VR_JUGGLER_LIBS ${VR_JUGGLER_LIBS} ${VRJ_OGL_LIB})
endif(VRJ_OGL_LIB)

FIND_LIBRARY(SONIX_LIB sonix-1_2 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib64
  /usr/lib
)

if(SONIX_LIB)
  SET(VR_JUGGLER_LIBS ${VR_JUGGLER_LIBS} ${SONIX_LIB})
endif(SONIX_LIB)

FIND_LIBRARY(VPR_LIB vpr-2_0 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib64
  /usr/lib
)

if(VPR_LIB)
  SET(VR_JUGGLER_LIBS ${VR_JUGGLER_LIBS} ${VPR_LIB})
endif(VPR_LIB)

FIND_LIBRARY(GADGET_LIB gadget-1_2 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib64
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
  /usr/lib64
  /usr/lib
)

if(JCCL_LIB)
  SET(VR_JUGGLER_LIBS ${VR_JUGGLER_LIBS} ${JCCL_LIB})
endif(JCCL_LIB)


FIND_LIBRARY(TWEEK_LIB tweek-1_2 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib64
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



