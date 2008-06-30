SHELL = /bin/sh

# This shouldn't be compiled on non-windows systam
# Must be defined before Makedefs is included.
PKG_NAME = DirectXJoystick
COMPILE_GUARD = DIRECTX_JOYSTICK

# Include all variables, flags, etc...
include ../Makedefs

INCLUDES = \
	-I./ \
	-I../../ \
	-I$(OSG_INC_DIR) \
	-I$(USUL_INC_DIR)

LIBS = -losg -losgDB -lUsul -lOsgTools -losgText -losgUtil -lOpenThreads

LIB_DIRS = -L$(USUL_LIB_DIR) -L$(OSG_LIB_DIR) 

# Include the rules for make...
#include ../Makerules

#nothing to do.  Windows only plugin.
default:

clean:
