SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = ImageModel
COMPILE_GUARD = IMAGE_MODEL

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
	-lUsul \
	-lFOX-1.4 -lOpenThreads -losg -losgDB -losgGA -losgText -losgUtil \
	-lOsgTools -lFoxTools \
	-lOpenThreadsEx -lOsgNodes -lViews

# Include the rules for make...
include ../Makerules
