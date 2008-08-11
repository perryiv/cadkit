SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = PlayMovie
COMPILE_GUARD = PLAY_MOVIE

# Include all variables, flags, etc...
include $(USUL_INC_DIR)/Usul/Make/Makedefs.plug

INCLUDES = \
	-I./ 				\
	-I./../../../Cinema/src 	\
	-I./../../../Experimental 	\
	-I../../ 			\
	-I${OSG_INC_DIR} 		\
	-I${PQXX_INC_DIR}		\
	-I${USUL_INC_DIR} 		\
	-I${OPENTHREADS_INC_DIR}	\
	-I${HOME}/sdk/

LIB_DIRS = \
	-L${USUL_LIB_DIR} \
	-L${OSG_LIB_DIR} \
	-L${OPENTHREADS_LIB_DIR} \

LIBS = \
	-lUsul -lOpenThreads -losg -lCinema

# Include the rules for make...
include $(USUL_INC_DIR)/Usul/Make/Makerules.plug
