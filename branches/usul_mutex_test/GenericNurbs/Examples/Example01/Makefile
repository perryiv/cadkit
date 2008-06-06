SOURCE = main.cpp

OS := $(shell uname)
COMPILER        = c++
COMPILER_FLAGS  =
LINKER_FLAGS    =
DEBUG_FLAGS     = -g -D_DEBUG

# Change some settings when on irix.
ifeq ($(findstring IRIX,$(OS)),IRIX)
COMPILER        = CC
COMPILER_FLAGS  = -LANG:std -I$(BOOST_ROOT_DIR)/boost/compatibility/cpp_c_headers -woff 1183,1178
LINKER_FLAGS    = -LANG:std -v
endif

# Change some settings when on Cygwin.
ifeq ($(findstring CYGWIN,$(OS)),CYGWIN)
COMPILER  = g++
EXTENSION = .exe
endif

COMPILE_COMMAND = $(COMPILER) $(DEBUG_FLAGS) $(COMPILER_FLAGS)
LINK_COMMAND		= $(COMPILER) $(DEBUG_FLAGS) $(LINKER_FLAGS)

INCLUDES = -I../../ -I${USUL_INC_DIR}
LIBS =

BIN_DIR = ../../../bin
TARGET  = gn_example_01
OBJECTS = $(SOURCE:.cpp=.o)

all : default 

default : $(TARGET)

$(TARGET) : ${OBJECTS}
	${LINK_COMMAND} ${OBJECTS} -o $(TARGET) ${LIBS}
	mkdir -p ${BIN_DIR}
	mv ${TARGET}${EXTENSION} ${BIN_DIR}

.cpp.o :
	$(COMPILE_COMMAND) $(INCLUDES) -c $< -o $@

clean:
	rm -f ${TARGET} ${OBJECTS} *.o
	rm -rf ii_files/
