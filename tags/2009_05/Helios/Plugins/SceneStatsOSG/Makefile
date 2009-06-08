SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = SceneStatsOsg
COMPILE_GUARD = OSG_SCENE_STATISTICS

# Include all variables, flags, etc...
include ../Makedefs

INCLUDES = \
	-I./ \
	-I../../ \
	-I$(OSG_INC_DIR) \
	-I$(USUL_INC_DIR) \
	-I$(OPENTHREADS_INC_DIR) 

LIB_DIRS = \
	-L$(OSG_LIB_DIR) \
	-L${OPENTHREADS_LIB_DIR} \
	-L${USUL_LIB_DIR} 

LIBS = \
	 -losg -lOpenThreads \
	 -lUsul 

# Include the rules for make...
include ../Makerules
