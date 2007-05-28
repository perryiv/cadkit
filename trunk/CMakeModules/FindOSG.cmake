
# Pick up if the user has set an environment variable to point to the 
# OpenThreads  Library Directory. If these are not set
# then they will simply be ignored.
 set(OT_LIB_DIR "$ENV{OPENTHREADS_LIB_DIR}")
 set(OT_INC_DIR "$ENV{OPENTHREADS_INC_DIR}")
 
 # Pick up if the user has set an environment variable to point to the 
#  OpenSceneGraph Library Directory. If these are not set
#  then they will simply be ignored.
 set(OSG_LIB_DIR "$ENV{OSG_LIB_DIR}")
 set(OSG_INC_DIR "$ENV{OSG_INC_DIR}")
 

# ------------ Find OpenThreads Library ----------------------
FIND_LIBRARY(OPENTHREADS_LIB OpenThreads
 ${OT_LIB_DIR}
 ${OSG_LIB_DIR}
)

# ------------ Find osg Library ----------------------
FIND_LIBRARY(OSG_LIB osg
 ${OSG_LIB_DIR}
)

# ------------ Find osgDB Library ----------------------
FIND_LIBRARY(OSG_DB_LIB osgDB
 ${OSG_LIB_DIR}
)

# ------------ Find osgGA Library ----------------------
FIND_LIBRARY(OSG_GA_LIB osgGA
 ${OSG_LIB_DIR}
)

# ------------ Find osgText Library ----------------------
FIND_LIBRARY(OSG_TEXT_LIB osgText
 ${OSG_LIB_DIR}
)

# ------------ Find osgUtil Library ----------------------
FIND_LIBRARY(OSG_UTIL_LIB osgUtil
 ${OSG_LIB_DIR}
)

# ------------ Find osgFX Library ----------------------
FIND_LIBRARY(OSG_FX_LIB osgFX
 ${OSG_LIB_DIR}
)

# ------------ Find osgProducer Library ----------------------
FIND_LIBRARY(OSG_PRODUCER_LIB osgProducer ${OSG_LIB_DIR} )