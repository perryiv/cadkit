SOURCE = \
	Library.cpp \
	Factory.cpp

OS := $(shell uname)
OS_COMP_FLAGS =
OS_LINK_FLAGS = -shared
COMPILER = c++

# Change some settings when on Darwin. Do not indent.
ifeq ($(findstring Darwin,$(OS)),Darwin)
OS_LINK_FLAGS = -dynamiclib
endif

# Change some settings when on irix. Do not indent.
ifeq ($(findstring IRIX,$(OS)),IRIX)
COMPILER = CC
OS_COMP_FLAGS = -LANG:std -I$(BOOST_ROOT_DIR)/boost/compatibility/cpp_c_headers
OS_LINK_FLAGS = -LANG:std -v
endif

COMMON_FLAGS   = -g -D_DEBUG -D_COMPILING_SELECTION_STL
COMPILER_FLAGS = $(COMMON_FLAGS) $(OS_COMP_FLAGS)
LINKER_FLAGS   = $(COMMON_FLAGS) $(OS_LINK_FLAGS)

COMPILE_COMMAND = $(COMPILER) $(COMPILER_FLAGS)
LINK_COMMAND		= $(COMPILER) $(LINKER_FLAGS)

INCLUDES = \
	-I./ \
	-I../../ \
	-I${OSG_INC_DIR} \
	-I${FOX_INC_DIR} \
	-I${USUL_INC_DIR}

LIB_DIRS = \
	-L${USUL_LIB_DIR} \
	-L${FOX_LIB_DIR} \
	-L${OSG_LIB_DIR}

LIBS = \
	-lUsul \
	-lFOX-1.4 -lFoxTools -lOpenThreads -losg -losgDB -losgText -losgUtil \
	-lOSgTools

TARGET      = SelectionSTL
PLUGIN_LIB  = $(TARGET).plug
BIN_DIR     = ${OSG_FOX_BIN_DIR}

OBJECTS = $(SOURCE:.cpp=.o)

all : 
	@echo "THIS IS UNMAINTAINED. DO NOT COMPILE" 

default : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(LINK_COMMAND) $(OBJECTS) -o $(PLUGIN_LIB) $(LIB_DIRS) $(LIBS)
	mkdir -p $(BIN_DIR)
	mv $(PLUGIN_LIB) $(BIN_DIR)

.cpp.o :
	$(COMPILE_COMMAND) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -rf ii_files/

clobber:
	rm -f $(OBJECTS) $(PLUGIN_LIB)
	rm -rf ii_files/
