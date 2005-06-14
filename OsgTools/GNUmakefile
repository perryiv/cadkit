PKG_NAME = OsgTools
SHELL = /bin/sh
LIB_OS = lib
OS := $(shell uname)

all: default

default : make_dirs shared_lib archive_lib

OS_LINK_FLAGS = -shared

# Platform dependent flags. Do not indent.
ifeq ($(findstring Darwin,$(OS)),Darwin)
OS_LINK_FLAGS = -dynamiclib
endif
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
	-L$(OSG_LIB_DIR) -losg -losgUtil -losgDB -losgText \
	-L$(USUL_LIB_DIR) -lUsul_s \
	-L$(OPENTHREADS_LIB_DIR) -lOpenThreads

includes = \
	-I$(OSG_INC_DIR) \
	-I$(USUL_INC_DIR) \
	-I$(OPENTHREADS_INC_DIR)

flags = -D_COMPILING_OSG_TOOLS

BUILD=$(OS)
IMAGES=Images
TRIANGLES=Triangles

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
	$(BUILD)/$(IMAGES)/EdgeDetect.o \
	$(BUILD)/$(IMAGES)/Grayscale.o \
	$(BUILD)/$(IMAGES)/Histogram.o \
	$(BUILD)/$(IMAGES)/Image3d.o \
	$(BUILD)/$(IMAGES)/Invert.o \
	$(BUILD)/$(IMAGES)/Morphology.o \
	$(BUILD)/$(IMAGES)/Scale.o \
	$(BUILD)/$(IMAGES)/Threshold.o \
	$(BUILD)/$(TRIANGLES)/Loop.o \
	$(BUILD)/$(TRIANGLES)/SharedVertex.o \
	$(BUILD)/$(TRIANGLES)/Triangle.o \
	$(BUILD)/$(TRIANGLES)/TriangleSet.o 

make_dirs:
	mkdir -p $(BUILD)
	mkdir -p $(BUILD)/$(IMAGES)
	mkdir -p $(BUILD)/$(TRIANGLES)

shared_lib: $(OBJS)
	$(CXX) $(OS_LINK_FLAGS) -g $(OBJS) -o $(BUILD)/$(shared_libname) $(libs)
	mkdir -p ../bin
	cp $(BUILD)/$(shared_libname) ../bin/

archive_lib: $(OBJS)
	$(RM) $(BUILD)/$(archive_libname)
	ar -cr $(BUILD)/$(archive_libname) $(OBJS)
	cp $(BUILD)/$(archive_libname) ../bin/

$(OBJS): $(BUILD)/%.o: %.cpp
	@echo "Compiling $< into $@"
	$(CXX) $(OS_COMP_FLAGS) $(flags) $(CXXFLAGS) -g $(includes) -c $< -o $@

clean:
	$(RM) -r $(BUILD) so_locations

mvlibs:
	mkdir -p $(MOVE_LOCATION)
	mv ./$(OS)/*.so ./$(OS)/*.a $(MOVE_LOCATION)
 
