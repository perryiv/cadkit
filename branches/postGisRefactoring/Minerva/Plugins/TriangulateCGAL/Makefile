SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = Triangulate
COMPILE_GUARD = TRIANGULATE

# Include all variables, flags, etc...
include ../Makedefs

INCLUDES = \
	-I./ \
	-I../../ \
	-I${OSG_INC_DIR} \
	-I${FOX_INC_DIR} \
	-I${USUL_INC_DIR} \
	-I${HOME}/sdk/

LIB_DIRS = \
	-L${USUL_LIB_DIR} \
	-L${FOX_LIB_DIR} \
	-L${OSG_LIB_DIR} \
	-L${HOME}/sdk/Quake

LIBS = \
	-lUsul -lOpenThreads -losg -ltriangle

# Include the rules for make...
include ../Makerules
