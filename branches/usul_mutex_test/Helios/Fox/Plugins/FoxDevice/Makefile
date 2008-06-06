SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = FoxDevice
COMPILE_GUARD = FOX_DEVICE

# Include all variables, flags, etc...
include ../Makedefs

INCLUDES = \
	-I./ \
	-I../../ \
	-I$(FOX_INC_DIR) \
	-I$(USUL_INC_DIR)

LIBS = -lFOX-1.4 -lUsul

LIB_DIRS = -L$(USUL_LIB_DIR) -L$(FOX_LIB_DIR) 

# Include the rules for make...
include ../Makerules
