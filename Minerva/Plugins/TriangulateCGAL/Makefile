SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = TriangulateCGAL
COMPILE_GUARD = TRIANGULATE_CGAL

# Include all variables, flags, etc...
include ${USUL_INC_DIR}/Usul/Make/Makedefs.plug

INCLUDES = \
	-I./ \
	-I../../ \
	-I${OSG_INC_DIR} \
	-I${CGAL_INC_DIR} \
	-I$(CGAL_INC_DIR)/CGAL/config/x86-64_Linux-2.6_g++-3.4.4 \
	-I${USUL_INC_DIR} 

LIB_DIRS = \
	-L${USUL_LIB_DIR} \
	-L${OSG_LIB_DIR} 

LIBS = \
	-lUsul

# Include the rules for make...
include ${USUL_INC_DIR}/Usul/Make/Makerules.plug
