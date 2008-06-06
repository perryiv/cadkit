SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = Convolve
COMPILE_GUARD = CONVOLVE

# Include all variables, flags, etc...
include ../Makedefs

INCLUDES = \
	-I./ \
	-I../../  \
	-I../../../ \
	-I${FOX_INC_DIR}  \
	-I${OSG_INC_DIR}

LIB_DIRS = \
	-L${USUL_LIB_DIR} \
	-L${FOX_LIB_DIR} \
	-L${OSG_LIB_DIR}

LIBS = \
	-lUsul -lFOX-1.4 -lFoxTools -lOpenThreads -losg -lOsgTools

# Include the rules for make...
include ../Makerules
