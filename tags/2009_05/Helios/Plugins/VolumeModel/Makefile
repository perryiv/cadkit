SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = VolumeModel
COMPILE_GUARD = VOLUME_MODEL

# Include all variables, flags, etc...
include ../Makedefs	

INCLUDES = \
	-I./ \
  	-I../../ \
	-I$(OSG_INC_DIR) \
	-I$(USUL_INC_DIR) 

LIBS = \
	-losg -losgDB -lUsul 

LIB_DIRS = \
		-L$(USUL_LIB_DIR) -L$(OSG_LIB_DIR)

# Include the rules for make...
include ../Makerules
