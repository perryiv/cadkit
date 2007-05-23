default : shared_lib
PKG_NAME = osgVRJ
SHELL = /bin/sh
OS := $(shell uname)

LIB_OS = lib
CLEAN = $(RM) -r $(BUILD)

# Platform dependent flags. Do not indent.
ifeq ($(findstring IRIX,$(OS)),IRIX)
OS := IRIX32
CXX = CC -n32 -LANG:std -woff 1682 -mips3
CXXFLAGS = -I$(VJ_BASE_DIR)/include/boost/compatibility/cpp_c_headers
LIB_OS = lib32
CLEAN = $(RM) -r $(BUILD) so_locations
endif

libs = -L$(VJ_BASE_DIR)/lib64 -lvrj -lvrj_ogl -lgadget
includes = -I$(OSG_INC_DIR) -I$(VJ_BASE_DIR)/include -I$(CADKIT_ROOT_DIR) -I$(OPENTHREADS_INC_DIR) -I$(VJ_DEPS_DIR)/include
flags = -D_COMPILING_OSG_VRJ

BUILD=$(OS)
EXT = so
name = $(PKG_NAME)
libname = lib$(name).$(EXT)

OBJS=   $(BUILD)/osgVRJ.o \
	$(BUILD)/Application.o

shared_lib: $(OBJS)
	$(CXX) -shared -g -fPIC $(OBJS) -o $(BUILD)/$(libname) $(libs)
	mkdir -p ../bin
	cp $(BUILD)/$(libname) ../bin/

$(OBJS): $(BUILD)/%.o: %.cpp
	mkdir -p $(BUILD)
	@echo "Compiling $< into $@"
	$(CXX) $(flags) $(CXXFLAGS) -g -fPIC $(includes) -c $< -o $@

clean:
	$(CLEAN)

mvlibs:
	mkdir -p $(MOVE_LOCATION)
	mv ./$(OS)/*.so $(MOVE_LOCATION)
