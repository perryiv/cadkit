SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = Core
COMPILE_GUARD = OSG_FOX_CORE

# Include all variables, flags, etc...
include ../../Usul/Make/Makedefs

INCLUDES = \
	-I../ \
	-I../../ \
	-I$(USUL_INC_DIR) \
	-I$(FOX_INC_DIR) \
	-I$(OPENTHREADS_INC_DIR)

LIB_DIRS = \
	-L${USUL_LIB_DIR} \
	-L${FOX_LIB_DIR} \
	-L${OPENTHREADS_LIB_DIR}

LIBS = \
	-lOpenThreads -lFOX-1.4 -lOpenThreadsEx -lUsul -lFoxTools 

# Include the rules for make...
include ../../Usul/Make/Makerules
