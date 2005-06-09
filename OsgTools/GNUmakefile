PKG_NAME = OsgTools
SHELL = /bin/sh
LIB_OS = lib
OS := $(shell uname)

default : shared_lib archive_lib

# Platform dependent flags. Do not indent.
ifeq ($(findstring IRIX,$(OS)),IRIX)
OS := IRIX32
CXX = g++ #CC -n32 -LANG:std -woff 1682 -mips3
CXXFLAGS = -mabi=n32 -mips4  
LIB_OS = -L/usr/freeware/lib32 #lib32
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
	-L$(USUL_LIB_DIR) -lUsul_s \
	-L$(OPENTHREADS_LIB_DIR) -lOpenThreads

includes = \
	-I$(OSG_INC_DIR) \
	-I$(USUL_INC_DIR) \
	-I$(OPENTHREADS_INC_DIR)

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
	$(BUILD)/Jitter.o \
	$(BUILD)/Lod.o \
	$(BUILD)/MaterialFactory.o \
	$(BUILD)/MaterialSetter.o \
	$(BUILD)/Mesh.o \
	$(BUILD)/Ray.o \
	$(BUILD)/ReadModel.o\
	$(BUILD)/Sample.o \
	$(BUILD)/ShapeFactory.o \
	$(BUILD)/State.o \
	$(BUILD)/Statistics.o \
	$(BUILD)/Text.o \
	$(BUILD)/Torus.o \
	$(BUILD)/Images/EdgeDetect.o \
	$(BUILD)/Images/Grayscale.o \
	$(BUILD)/Images/Histogram.o \
	$(BUILD)/Images/Image3d.o \
	$(BUILD)/Images/Invert.o \
	$(BUILD)/Images/Morphology.o \
	$(BUILD)/Images/Scale.o \
	$(BUILD)/Images/Threshold.o \
	$(BUILD)/Triangles/Loop.o \
	$(BUILD)/Triangles/SharedVertex.o \
	$(BUILD)/Triangles/Triangle.o \
	$(BUILD)/Triangles/TriangleSet.o 

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
 
