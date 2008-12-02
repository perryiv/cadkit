

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

FIND_LIBRARY(VRJ_LIBRARY_RELEASE vrj-2_2 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib64
  /usr/lib
)

FIND_LIBRARY(VRJ_LIBRARY_DEBUG vrj_d-2_2 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib64
  /usr/lib
)

CADKIT_SET_DEBUG_RELEASE_LIBRARY( VRJ_LIBRARY VRJ_LIBRARY_DEBUG VRJ_LIBRARY_RELEASE )

if(VRJ_LIBRARY)
  SET(VR_JUGGLER_LIBS ${VRJ_LIBRARY})
endif(VRJ_LIBRARY)

FIND_LIBRARY(VRJ_OGL_LIBRARY_RELEASE vrj_ogl-2_2 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib64
  /usr/lib
)

FIND_LIBRARY(VRJ_OGL_LIBRARY_DEBUG vrj_ogl_d-2_2 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib64
  /usr/lib
)

CADKIT_SET_DEBUG_RELEASE_LIBRARY( VRJ_OGL_LIBRARY VRJ_OGL_LIBRARY_DEBUG VRJ_OGL_LIBRARY_RELEASE )

if(VRJ_OGL_LIBRARY)
  SET(VR_JUGGLER_LIBS ${VR_JUGGLER_LIBS} ${VRJ_OGL_LIBRARY})
endif(VRJ_OGL_LIBRARY)

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

FIND_LIBRARY(VPR_LIBRARY_RELEASE vpr-2_0 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib64
  /usr/lib
)

FIND_LIBRARY(VPR_LIBRARY_DEBUG vpr_d-2_0 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib64
  /usr/lib
)

CADKIT_SET_DEBUG_RELEASE_LIBRARY( VPR_LIBRARY VPR_LIBRARY_DEBUG VPR_LIBRARY_RELEASE )

if(VPR_LIBRARY)
  SET(VR_JUGGLER_LIBS ${VR_JUGGLER_LIBS} ${VPR_LIBRARY})
endif(VPR_LIBRARY)

FIND_LIBRARY(GADGET_LIBRARY_RELEASE gadget-1_2 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib64
  /usr/lib
)

FIND_LIBRARY(GADGET_LIBRARY_DEBUG gadget_d-1_2 
  "$ENV{VJ_BASE_DIR}/lib64"
  "$ENV{VJ_BASE_DIR}/lib"
  /usr/local/lib64
  /usr/local/lib
  /usr/lib64
  /usr/lib
)

CADKIT_SET_DEBUG_RELEASE_LIBRARY( GADGET_LIBRARY GADGET_LIBRARY_DEBUG GADGET_LIBRARY_RELEASE )

if(GADGET_LIBRARY)
  SET(VR_JUGGLER_LIBS ${VR_JUGGLER_LIBS} ${GADGET_LIBRARY})
endif(GADGET_LIBRARY)

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


