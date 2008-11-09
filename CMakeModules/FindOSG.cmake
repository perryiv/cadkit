
INCLUDE ( Cadkit )

# Pick up if the user has set an environment variable to point to the 
# OpenThreads  Library Directory. If these are not set
# then they will simply be ignored.

FIND_PATH(OT_INC_DIR  OpenThreads/Mutex
  "$ENV{OPENTHREADS_INC_DIR}"
  "$ENV{OSG_INC_DIR}"
  /usr/include
  /usr/local/include
)

 set(OT_LIB_DIR "$ENV{OPENTHREADS_LIB_DIR}")
  
 # Pick up if the user has set an environment variable to point to the 
#  OpenSceneGraph Library Directory. If these are not set
#  then they will simply be ignored.
 set(OSG_LIB_DIR "$ENV{OSG_LIB_DIR}")
 set(OSG_INC_DIR "$ENV{OSG_INC_DIR}")
 
#########################################################
#
#  Find OpenThread libraries.
#
#########################################################

FIND_LIBRARY(OPENTHREADS_LIBRARY_RELEASE OpenThreads
 ${OT_LIB_DIR}
 ${OSG_LIB_DIR}
)

FIND_LIBRARY(OPENTHREADS_LIBRARY_DEBUG OpenThreadsd
 ${OT_LIB_DIR}
 ${OSG_LIB_DIR}
)

SET ( OPENTHREADS_LIB OPENTHREADS_LIBRARY_RELEASE )
CADKIT_SET_DEBUG_RELEASE_LIBRARY( OPENTHREADS_LIB OPENTHREADS_LIBRARY_DEBUG OPENTHREADS_LIBRARY_RELEASE)

#########################################################
#
#  Find OpenSceneGraph libraries.
#
#########################################################

FIND_LIBRARY ( OSG_LIB         osg            ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_DB_LIB      osgDB          ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_GA_LIB      osgGA          ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_TEXT_LIB    osgText        ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_UTIL_LIB    osgUtil        ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_FX_LIB      osgFX          ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_MANIPULATOR osgManipulator ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_VIEWER      osgViewer      ${OSG_LIB_DIR} )


#########################################################
#
#  Mark these are advanced.
#
#########################################################

MARK_AS_ADVANCED(
  OT_INC_DIR
  OPENTHREADS_LIB
)
