SOURCE = \
	Factory.cpp \
	FileIO.cpp \
	Group.cpp \
	Node.cpp \
	Transform.cpp \
	Grid.cpp \
	Sphere.cpp

OS := $(shell uname)
OS_COMP_FLAGS =
OS_LINK_FLAGS =
COMPILER = c++

# Change some settings when on irix. Do not indent.
ifeq ($(findstring IRIX,$(OS)),IRIX)
COMPILER = CC
OS_COMP_FLAGS = -LANG:std -I$(BOOST_ROOT_DIR)/boost/compatibility/cpp_c_headers -woff 1682,1183
OS_LINK_FLAGS = -LANG:std -v
endif

COMMON_FLAGS   = -g -D_DEBUG -D_COMPILING_OSG_SAL
COMPILER_FLAGS = $(COMMON_FLAGS) $(OS_COMP_FLAGS)
LINKER_FLAGS   = $(COMMON_FLAGS) $(OS_LINK_FLAGS)

COMPILE_COMMAND = $(COMPILER) $(COMPILER_FLAGS)
LINK_COMMAND		= $(COMPILER) $(LINKER_FLAGS)

BIN_DIR     = ../../bin

INCLUDES = -I./ -I../../ -I$(OSG_INC_DIR)
LIBS = \
	-L$(BIN_DIR) -lOsgTools_s \
	-L$(OSG_LIB_DIR) -losg -losgDB

TARGET      = osg_sal
PLUGIN_LIB  = $(TARGET).vrplug

OBJECTS = $(SOURCE:.cpp=.o)

default : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(LINK_COMMAND) -shared $(OBJECTS) -o $(PLUGIN_LIB) $(LIBS)
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
