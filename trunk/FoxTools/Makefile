SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = FoxTools
COMPILE_GUARD = FOX_TOOLS

# Include all variables, flags, etc...
include ../Usul/Make/Makedefs

INCLUDES = \
	-I../ \
	-I$(FOX_INC_DIR) \
	-I$(BOOST_INC_DIR)


LIBS = \
	 -lFOX-1.4 \
	 -lUsul

LIB_DIRS = \
	-L$(FOX_LIB_DIR) -L$(USUL_LIB_DIR)

# Include the rules for make...
include ../Usul/Make/Makerules
