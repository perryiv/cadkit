SOURCE = \
	Mutex.cpp

OS := $(shell uname)
OS_COMP_FLAGS =
OS_LINK_FLAGS = -shared
COMPILER = c++

COMMON_FLAGS   = -g -D_DEBUG -D_COMPILING_OPEN_THREADS_EXTENSIONS

# Platform dependent flags. Do not indent.
ifeq ($(findstring Darwin,$(OS)),Darwin)
OS_LINK_FLAGS = -dynamiclib
endif
ifeq ($(findstring IRIX,$(OS)),IRIX)

COMPILER = g++ #CC
OS_COMP_FLAGS = -mabi=n32 -mips4  #-n32 -LANG:std
OS_LINK_FLAGS = -L/usr/freeware/lib32 #-n32 -LANG:std -v
COMMON_FLAGS  = -g -D_DEBUG -D_COMPILING_OPEN_THREADS_EXTENSIONS
endif


COMPILER_FLAGS = $(COMMON_FLAGS) $(OS_COMP_FLAGS)
LINKER_FLAGS   = $(COMMON_FLAGS) $(OS_LINK_FLAGS)

COMPILE_COMMAND = $(COMPILER) $(COMPILER_FLAGS)
LINK_COMMAND		= $(COMPILER) $(LINKER_FLAGS)

INCLUDES = \
	-I../../ \
	-I$(OPENTHREADS_INC_DIR) \
	-I$(USUL_INC_DIR) 


LIBS = \
	-L$(OPENTHREADS_LIB_DIR) -lOpenThreads \
	-L$(USUL_LIB_DIR) -lUsul_s

TARGET      = OpenThreadsEx
ARCHIVE_LIB = lib$(TARGET)_a.a
SHARED_LIB  = lib$(TARGET)_s.so
BIN_DIR     = $(OSG_FOX_BIN_DIR)
CLEAN       = rm -rf ii_files/ ; rm -f *.o ; cd ../

OBJECTS = $(SOURCE:.cpp=.o)

all: default

all : default 

default : $(TARGET)

$(TARGET) : $(OBJECTS)
	rm -rf $(ARCHIVE_LIB)
	ar -cr $(ARCHIVE_LIB) $(OBJECTS)
	$(LINK_COMMAND) $(OBJECTS) -o $(SHARED_LIB) $(LIBS)
	mkdir -p $(BIN_DIR)
	mv $(ARCHIVE_LIB) $(BIN_DIR)
	mv $(SHARED_LIB) $(BIN_DIR)

.cpp.o :
	$(COMPILE_COMMAND) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -rf ii_files/

clobber:
	rm -f $(OBJECTS) $(ARCHIVE_LIB) $(SHARED_LIB)
	rm -rf ii_files/
