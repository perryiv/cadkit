SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = OsgTools
COMPILE_GUARD = OSG_TOOLS

# Include all variables, flags, etc...
include ../Usul/Make/Makedefs

INCLUDES = \
	-I../ \
	-I$(OSG_INC_DIR) \
	-I$(USUL_INC_DIR) \
	-I$(OPENTHREADS_INC_DIR)

LIBS = \
	 -losg -losgUtil -losgDB -losgText -lUsul -lOpenThreads

LIB_DIRS = \
	-L$(OSG_LIB_DIR) -L$(USUL_LIB_DIR) -L$(OPENTHREADS_LIB_DIR)
	
# Include the rules for make...
include ../Usul/Make/Makerules
