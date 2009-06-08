SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = SceneModel
COMPILE_GUARD = SCENE_MODEL

# Include all variables, flags, etc...
include ../Makedefs

INCLUDES = \
	-I./ \
	-I../../ \
	-I$(OSG_INC_DIR) \
	-I$(USUL_INC_DIR) \
	-I$(OPENTHREADS_INC_DIR) \

LIB_DIRS = \
	-L$(OSG_LIB_DIR) \
	-L$(OPENTHREADS_LIB_DIR) \
	-L$(USUL_LIB_DIR) \

LIBS = \
	 -losg -losgDB -losgUtil -losgGA -losgText -lOpenThreads \
	 -lUsul -lOsgTools  -lOsgNodes  -lOpenThreadsEx 

# Include the rules for make...
include ../Makerules
