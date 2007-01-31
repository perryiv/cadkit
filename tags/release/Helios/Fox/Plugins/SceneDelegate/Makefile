SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = SceneDelegate
COMPILE_GUARD = SCENE_DELEGATE

# Include all variables, flags, etc...
include ../Makedefs

INCLUDES = \
	-I./ \
	-I../../ \
	-I$(OSG_INC_DIR) \
	-I$(USUL_INC_DIR) \
	-I$(OPENTHREADS_INC_DIR) \
	-I$(FOX_INC_DIR)

LIBS = \
	 -losg -losgDB -lOpenThreads -losgGA -losgUtil -losgText \
	 -lFOX-1.4 -lFoxTools -lUsul -lOsgTools -lOsgNodes -lOpenThreadsEx -lViews

LIB_DIRS = \
		-L$(USUL_LIB_DIR) -L$(OSG_LIB_DIR) -L$(FOX_LIB_DIR)

# Include the rules for make...
include ../Makerules
	