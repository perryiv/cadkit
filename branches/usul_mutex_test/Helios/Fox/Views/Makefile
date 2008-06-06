# OsgFox/Views MAKEFILE #
SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = Views
COMPILE_GUARD = OSG_FOX_VIEWS

# Include all variables, flags, etc...
include ../../Usul/Make/Makedefs

LIBS = \
	-losg -losgGA -losgDB -losgUtil -losgText -lOsgTools -lOsgNodes \
	-lUsul -lFoxTools -lOpenThreads -lOpenThreadsEx \
	-lFOX-1.4

# Platform dependent flags. Do not indent.
ifeq ($(findstring Darwin,$(OS)),Darwin)
OPENGL_LIB_DIR = /System/Library/Frameworks/OpenGL.framework/Libraries
LIBS += -lgl
endif

INCLUDES = \
	-I../ \
	-I../../ \
	-I$(OSG_INC_DIR) \
	-I$(USUL_INC_DIR) \
	-I$(FOX_INC_DIR) \
	-I$(OPENTHREADS_INC_DIR)

LIB_DIRS = \
	-L${OSG_LIB_DIR} \
	-L${USUL_LIB_DIR} \
	-L${FOX_LIB_DIR} \
	-L${OPENTHREADS_LIB_DIR} \
	-L$(OPENGL_LIB_DIR)


OBJS = $(patsubst %.cpp,%.o,$(subst ./,$(OS)/,$(shell find . -type f -name "*.cpp" -maxdepth 1)))
  
# Include the rules for make...
include ../../Usul/Make/Makerules
