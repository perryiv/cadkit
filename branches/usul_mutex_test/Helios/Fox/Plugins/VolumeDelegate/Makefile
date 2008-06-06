SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = VolumeDelegate
COMPILE_GUARD = VOLUME_DELEGATE

# Include all variables, flags, etc...
include ../Makedefs

INCLUDES = \
	-I./ \
	-I../../ \
	-I$(OSG_INC_DIR) \
	-I$(USUL_INC_DIR) \
	-I$(OPENTHREADS_INC_DIR) \
	-I$(FOX_INC_DIR)

LIBS = \
	 -losg -losgDB -lFOX-1.4 -lUsul

LIB_DIRS = \
		-L$(USUL_LIB_DIR) -L$(OSG_LIB_DIR) -L$(FOX_LIB_DIR)

# Include the rules for make...
include ../Makerules
