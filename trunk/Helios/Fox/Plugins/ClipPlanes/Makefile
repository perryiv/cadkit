SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = ClipPlanes
COMPILE_GUARD = CLIP_PLANES

# Include all variables, flags, etc...
include ../Makedefs

INCLUDES = \
	-I./ \
  	-I../../  \
  	-I../../../ \
	-I$(FOX_INC_DIR)  \
  	-I$(OSG_INC_DIR) \
	-I$(USUL_LIB_DIR)

LIB_DIRS = \
	-L$(USUL_LIB_DIR) \
	-L$(FOX_LIB_DIR) \
	-L$(OSG_LIB_DIR)

LIBS = \
	-lFOX-1.4 -lUsul -lFoxTools

# Include the rules for make...
include ../Makerules
	