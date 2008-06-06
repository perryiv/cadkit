SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = SubdivideTriangles
COMPILE_GUARD = SUBDIVIDE_TRIANGLES

# Include all variables, flags, etc...
include ../Makedefs

INCLUDES = \
	-I./ \
	-I../ \
	-I$(USUL_INC_DIR) 

LIBS = -lUsul

LIB_DIRS = -L$(USUL_LIB_DIR)

# Include the rules for make...
include ../Makerules
