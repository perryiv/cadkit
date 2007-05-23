PKG_NAME = vrjGA
SHELL = /bin/sh
LIB_OS = lib64
OS := $(shell uname)

default : shared_lib

# Platform dependent flags. Do not indent.
ifeq ($(findstring IRIX,$(OS)),IRIX)
OS := IRIX32
CXX = CC -n32 -LANG:std -woff 1682 -mips3
CXXFLAGS = -I$(VJ_BASE_DIR)/include/boost/compatibility/cpp_c_headers
LIB_OS = lib32
endif

libs = -L$(VJ_BASE_DIR)/$(LIB_OS) -lgadget
includes = -I../ -I$(BOOST_ROOT_DIR) -I$(VJ_BASE_DIR)/include -I$(VJ_DEPS_DIR)/include
flags = -D_COMPILING_VRJ_GA

BUILD=$(OS)
EXT = so
libname = lib$(PKG_NAME).$(EXT)

OBJS=\
	$(BUILD)/vrjGA.o \
	$(BUILD)/Referenced.o \
	$(BUILD)/ButtonDevice.o \
	$(BUILD)/ButtonGroup.o \
	$(BUILD)/JoystickDevice.o \
	$(BUILD)/TrackerDevice.o \
	$(BUILD)/Callback.o

shared_lib: $(OBJS)
	$(CXX) $(OS_LINK_FLAGS) -shared -g -fPIC $(OBJS) -o $(BUILD)/$(libname) $(libs)
	mkdir -p ../bin
	cp $(BUILD)/$(libname) ../bin/

$(OBJS): $(BUILD)/%.o: %.cpp
	mkdir -p $(BUILD)
	@echo "Compiling $< into $@"
	$(CXX) $(OS_COMP_FLAGS) $(flags) $(CXXFLAGS) -g -fPIC $(includes) -c $< -o $@

clean:
	$(RM) -r $(BUILD) so_locations

mvlibs:
	mkdir -p $(MOVE_LOCATION)
	mv ./$(OS)/*.so $(MOVE_LOCATION)
