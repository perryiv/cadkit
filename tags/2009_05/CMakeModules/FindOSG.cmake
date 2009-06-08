
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
 
 FIND_PATH ( OSG_INC_DIR osg/Node
 	"$ENV{OSG_INC_DIR}")
 
 
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

FIND_LIBRARY ( OSG_LIBRARY_RELEASE         osg            ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_DB_LIBRARY_RELEASE      osgDB          ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_GA_LIBRARY_RELEASE      osgGA          ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_TEXT_LIBRARY_RELEASE    osgText        ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_UTIL_LIBRARY_RELEASE    osgUtil        ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_FX_LIBRARY_RELEASE      osgFX          ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_MANIPULATOR_LIBRARY_RELEASE osgManipulator ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_VIEWER_LIBRARY_RELEASE      osgViewer      ${OSG_LIB_DIR} )

FIND_LIBRARY ( OSG_LIBRARY_DEBUG         osgd            ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_DB_LIBRARY_DEBUG      osgDBd          ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_GA_LIBRARY_DEBUG      osgGAd          ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_TEXT_LIBRARY_DEBUG    osgTextd        ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_UTIL_LIBRARY_DEBUG    osgUtild        ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_FX_LIBRARY_DEBUG      osgFXd          ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_MANIPULATOR_LIBRARY_DEBUG osgManipulatord ${OSG_LIB_DIR} )
FIND_LIBRARY ( OSG_VIEWER_LIBRARY_DEBUG      osgViewerd      ${OSG_LIB_DIR} )

CADKIT_SET_DEBUG_RELEASE_LIBRARY( OSG_LIB             OSG_LIBRARY_DEBUG             OSG_LIBRARY_RELEASE )
CADKIT_SET_DEBUG_RELEASE_LIBRARY( OSG_DB_LIB          OSG_DB_LIBRARY_DEBUG          OSG_DB_LIBRARY_RELEASE )
CADKIT_SET_DEBUG_RELEASE_LIBRARY( OSG_GA_LIB          OSG_GA_LIBRARY_DEBUG          OSG_GA_LIBRARY_RELEASE )
CADKIT_SET_DEBUG_RELEASE_LIBRARY( OSG_TEXT_LIB        OSG_TEXT_LIBRARY_DEBUG        OSG_TEXT_LIBRARY_RELEASE )
CADKIT_SET_DEBUG_RELEASE_LIBRARY( OSG_UTIL_LIB        OSG_UTIL_LIBRARY_DEBUG        OSG_UTIL_LIBRARY_RELEASE )
CADKIT_SET_DEBUG_RELEASE_LIBRARY( OSG_FX_LIB          OSG_FX_LIBRARY_DEBUG          OSG_FX_LIBRARY_RELEASE )
CADKIT_SET_DEBUG_RELEASE_LIBRARY( OSG_MANIPULATOR_LIB OSG_MANIPULATOR_LIBRARY_DEBUG OSG_MANIPULATOR_LIBRARY_RELEASE )
CADKIT_SET_DEBUG_RELEASE_LIBRARY( OSG_VIEWER_LIB      OSG_VIEWER_LIBRARY_DEBUG      OSG_VIEWER_LIBRARY_RELEASE )

#########################################################
#
#  Mark these are advanced.
#
#########################################################

MARK_AS_ADVANCED(
  OT_INC_DIR
  OPENTHREADS_LIB
)
