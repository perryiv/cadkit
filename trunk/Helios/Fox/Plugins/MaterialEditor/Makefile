SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = MaterialEditor
COMPILE_GUARD = MATERIAL_EDITOR

# Include all variables, flags, etc...
include ../Makedefs

INCLUDES = \
	-I./ \
	-I../../ \
	-I${OSG_INC_DIR} \
	-I${FOX_INC_DIR} \
	-I${USUL_INC_DIR}

LIB_DIRS = \
	-L${USUL_LIB_DIR} \
	-L${FOX_LIB_DIR} \
	-L${OSG_LIB_DIR}

LIBS = \
	-lUsul -lOpenThreads -losg -lOsgTools

# Include the rules for make...
include ../Makerules
