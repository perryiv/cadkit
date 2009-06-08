SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = ComputeNormals
COMPILE_GUARD = COMPUTE_NORMALS

# Include all variables, flags, etc...
include ${USUL_INC_DIR}/Usul/Make/Makedefs.plug

INCLUDES = \
	-I./ \
	-I../../ \
	-I${USUL_INC_DIR} \

LIB_DIRS = \
	-L${USUL_LIB_DIR} 

LIBS = \
	-lUsul

# Include the rules for make...
include ${USUL_INC_DIR}/Usul/Make/Makerules.plug
