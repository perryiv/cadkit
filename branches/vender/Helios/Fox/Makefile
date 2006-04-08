SOURCE = \
	main.cpp 

OS := $(shell uname)


# ##### default values for INCLUDES and LIBS ###### #

# Fox Includes and Libs
ifeq ("$(FOX_INC_DIR)","")
FOX_INC_DIR = /usr/local/include/fox-1.4
endif

ifeq ("$(FOX_LIB_DIR)","")
FOX_LIB_DIR = /usr/local/lib
endif

# OpenThreads Includes and Libs
ifeq ("$(OPENTHREADS_INC_DIR)","")
OPENTHREADS_INC_DIR = /usr/local/include
endif

ifeq ("$(OPENTHREADS_LIB_DIR)","")
OPENTHREADS_LIB_DIR = /usr/local/lib
endif

# OpenProducer Includes and Libs
ifeq ("$(PRODUCER_INC_DIR)","")
PRODUCER_INC_DIR = /usr/local/include
endif

ifeq ("$(PRODUCER_LIB_DIR)","")
PRODUCER_LIB_DIR = /usr/local/lib
endif

# Osg Includes and Libs
ifeq ("$(OSG_BIN_DIR)","")
OSG_BIN_DIR = /usr/local/bin
endif

ifeq ("$(OSG_INC_DIR)","")
OSG_INC_DIR = /usr/local/include
endif

ifeq ("$(OSG_LIB_DIR)","")
OSG_LIB_DIR = /usr/local/lib
endif

# Our Build Libraries all go in ../LIBS
ifeq ("$(USUL_INC_DIR)","")
USUL_INC_DIR = ../../
endif

ifeq ("$(USUL_LIB_DIR)","")
USUL_LIB_DIR = ../../lib
endif
# ################################################## #



COMPILER        = g++
DEBUG_FLAGS     = -g -D_DEBUG -D_DEBUG_PRINT

# Change some settings when on irix.
ifeq ($(findstring IRIX,$(OS)),IRIX)
COMPILER        = g++ #CC
COMPILER_FLAGS  = -mabi=n32 -mips4 #-n32 -LANG:std -woff 1681,1682,1183
LINKER_FLAGS    = -L/usr/freeware/lib32 #-n32 -LANG:std -v
endif

# Change some settings when on Cygwin.
ifeq ($(findstring Cygwin,$(OS)),Cygwin)
COMPILER_FLAGS = -I$(NSPR_ROOT)/include
COMPILER = g++
endif

COMPILE_COMMAND = $(COMPILER) $(DEBUG_FLAGS) $(COMPILER_FLAGS)
LINK_COMMAND		= $(COMPILER) $(DEBUG_FLAGS) $(LINKER_FLAGS)

BIN_DIR = ../../bin

INCLUDES = \
	-I./ \
	-I../ \
	-I../../ \
	-I${OSG_INC_DIR} \
	-I${USUL_INC_DIR} \
	-I${FOX_INC_DIR} \
	-I${OPENTHREADS_INC_DIR}

LOCAL_LIBS = \
	-L$(USUL_LIB_DIR) \
	-lUsul \
	-lOpenThreadsEx \
	-lFoxTools \
	-lCore

OSG_LIBS = \
	-L${OSG_LIB_DIR} \
	-losgText \
	-losgUtil \
	-losgGA \
	-losgDB -losg

OPEN_THREADS_LIBS = \
	-L${OPENTHREADS_LIB_DIR} \
	-lOpenThreads

FOX_LIBS = \
	-L${FOX_LIB_DIR} \
	-lFOX-1.4

LIBS = $(LOCAL_LIBS) $(OSG_LIBS) $(OPEN_THREADS_LIBS) $(FOX_LIBS)

TARGET  = Osg_Fox_Example
OBJECTS = $(SOURCE:.cpp=.o)
CLEAN   = rm -rf ii_files/ ; rm -f *.o ; cd ../

all : default 

default : $(TARGET)

$(TARGET) : ${OBJECTS}
	${LINK_COMMAND} ${OBJECTS} -o $(TARGET) ${LIBS}
	mkdir -p $(BIN_DIR)
	mv $(TARGET) $(BIN_DIR)

.cpp.o :
	$(COMPILE_COMMAND) $(INCLUDES) -c $< -o $@

clean:
	rm -f ${TARGET} ${OBJECTS} *.o
	rm -rf ii_files/

clobber:
	rm -f ${TARGET} ${OBJECTS} *.o
	rm -rf ii_files/
