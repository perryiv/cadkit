SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = TriangleModel
COMPILE_GUARD = TRIANGLE_MODEL

# Include all variables, flags, etc...
include ../Makedefs

INCLUDES = \
	-I./ \
  	-I../../ \
	-I$(OSG_INC_DIR) \
	-I$(USUL_INC_DIR)

LIBS = \
	-losg -losgDB -lUsul -lOsgTools -losgText -losgUtil -lOpenThreads

LIB_DIRS = \
		-L$(USUL_LIB_DIR) -L$(OSG_LIB_DIR) 

# Include the rules for make...
include ../Makerules
