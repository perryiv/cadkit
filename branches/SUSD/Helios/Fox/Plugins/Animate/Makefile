SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = Animate
COMPILE_GUARD = ANIMATE

# Include all variables, flags, etc...
include ../Makedefs

INCLUDES = \
	-I./ \
	-I../ \
	-I../../../GenericNurbs/ \
	-I$(USUL_INC_DIR) \
	-I$(OSG_INC_DIR) \
	-I$(FOX_INC_DIR)

LIBS = -lUsul -lFoxTools -losg -lFOX-1.4 -lOpenThreads

LIB_DIRS = -L$(USUL_LIB_DIR) -L$(OSG_LIB_DIR) -L$(FOX_LIB_DIR)

# Include the rules for make...
include ../Makerules
