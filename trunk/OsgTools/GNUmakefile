PKG_NAME = OsgTools
SHELL = /bin/sh
LIB_OS = lib
OS := $(shell uname)

default : shared_lib archive_lib

# Platform dependent flags. Do not indent.
ifeq ($(findstring IRIX,$(OS)),IRIX)
OS := IRIX32
CXX = CC -n32 -LANG:std -woff 1682 -mips3
CXXFLAGS = 
LIB_OS = lib32
endif

#-- default values for includes and libs --#
ifeq ("$(USUL_INC_DIR)","")
USUL_INC_DIR = ../
endif

ifeq ("$(USUL_LIB_DIR)","")
USUL_LIB_DIR = ../bin
endif
#-- ------------------------------------ --#

libs = \
	-L$(OSG_LIB_DIR) -losg \
	-L$(USUL_LIB_DIR) -lUsul_s

includes = \
	-I$(OSG_INC_DIR) \
	-I$(USUL_INC_DIR)

flags = -D_COMPILING_OSG_TOOLS

BUILD=$(OS)
shared_libname = lib$(PKG_NAME)_s.so
archive_libname = lib$(PKG_NAME)_a.a

OBJS=\
	$(BUILD)/Axes.o \
	$(BUILD)/Circle.o \
	$(BUILD)/ColorSetter.o \
	$(BUILD)/Font.o \
	$(BUILD)/Grid.o \
	$(BUILD)/Group.o \
	$(BUILD)/MaterialSetter.o \
	$(BUILD)/Mesh.o \
	$(BUILD)/Ray.o \
	$(BUILD)/State.o \
	$(BUILD)/Text.o \
	$(BUILD)/Torus.o

shared_lib: $(OBJS)
	$(CXX) $(OS_LINK_FLAGS) -shared -g $(OBJS) -o $(BUILD)/$(shared_libname) $(libs)
	mkdir -p ../bin
	cp $(BUILD)/$(shared_libname) ../bin/

archive_lib: $(OBJS)
	$(RM) $(BUILD)/$(archive_libname)
	ar -cr $(BUILD)/$(archive_libname) $(OBJS)
	cp $(BUILD)/$(archive_libname) ../bin/

$(OBJS): $(BUILD)/%.o: %.cpp
	mkdir -p $(BUILD)
	@echo "Compiling $< into $@"
	$(CXX) $(OS_COMP_FLAGS) $(flags) $(CXXFLAGS) -g $(includes) -c $< -o $@

clean:
	$(RM) -r $(BUILD) so_locations

mvlibs:
	mkdir -p $(MOVE_LOCATION)
	mv ./$(OS)/*.so ./$(OS)/*.a $(MOVE_LOCATION)
