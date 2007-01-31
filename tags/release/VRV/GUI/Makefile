SOURCE = \
	Parser.cpp \
	Factory.cpp \
	Menu.cpp

OS := $(shell uname)
OS_COMP_FLAGS =
OS_LINK_FLAGS =
COMPILER = c++

# Change some settings when on irix. Do not indent.
ifeq ($(findstring IRIX,$(OS)),IRIX)
COMPILER = CC
OS_COMP_FLAGS = -LANG:std -I$(BOOST_ROOT_DIR)/boost/compatibility/cpp_c_headers -woff 1183
OS_LINK_FLAGS = -LANG:std -v
endif

COMMON_FLAGS   = -g -D_DEBUG -D_COMPILING_VRV_GUI
COMPILER_FLAGS = $(COMMON_FLAGS) $(OS_COMP_FLAGS)
LINKER_FLAGS   = $(COMMON_FLAGS) $(OS_LINK_FLAGS)

COMPILE_COMMAND = $(COMPILER) $(COMPILER_FLAGS)
LINK_COMMAND		= $(COMPILER) $(LINKER_FLAGS)

INCLUDES = -I./ -I../../ -I$(BOOST_ROOT_DIR)
LIBS = -L../../bin -lMenuKit

TARGET      = vrv_gui
PLUGIN_LIB  = $(TARGET).vrplug
BIN_DIR     = ../../bin

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
