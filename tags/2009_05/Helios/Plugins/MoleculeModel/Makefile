SHELL = /bin/sh
LIB_OS = lib
PKG_NAME = MoleculeModel
OS := $(shell uname)
TARGET  = $(PKG_NAME)
COMPILE_GUARD = MOLECULE_MODEL

all: 
	@echo "THIS DEPENDS ON OSG PLUGINS THAT HAVE BEEN DEPRECATED."
	@echo "THIS PLUGIN WILL NOT BE COMPILED"

default : make_dirs shared_lib archive_lib

# ##### default values for includes and libs ###### #

# Fox Includes and Libs
ifeq ("$(FOX_INC_DIR)","")
FOX_INC_DIR = /usr/local/include/fox-1.4
endif

ifeq ("$(FOX_LIB_DIR)","")
FOX_LIB_DIR = /usr/local/lib
endif

# OpenThreads Includes and Libs
ifeq ("$(OPENTHREADS_INC_DIR)","")
OPENTHREADS_INC_DIR = /usr/local/include
endif

ifeq ("$(OPENTHREADS_LIB_DIR)","")
OPENTHREADS_LIB_DIR = /usr/local/lib
endif

# OpenProducer Includes and Libs
ifeq ("$(PRODUCER_INC_DIR)","")
PRODUCER_INC_DIR = /usr/local/include
endif

ifeq ("$(PRODUCER_LIB_DIR)","")
PRODUCER_LIB_DIR = /usr/local/lib
endif

# Osg Includes and Libs
ifeq ("$(OSG_BIN_DIR)","")
OSG_BIN_DIR = /usr/local/bin
endif

ifeq ("$(OSG_INC_DIR)","")
OSG_INC_DIR = /usr/local/include
endif

ifeq ("$(OSG_LIB_DIR)","")
OSG_LIB_DIR = /usr/local/lib
endif

# Our Build Libraries all go in ../libs
ifeq ("$(USUL_INC_DIR)","")
USUL_INC_DIR = ../../../
endif

ifeq ("$(USUL_LIB_DIR)","")
USUL_LIB_DIR = ../../../lib
endif

ifeq ("$(BOOST_INC_DIR)","")
BOOST_INC_DIR = /usr/local/include/boost
endif 

ifeq ("$(BOOST_LIB_DIR)","")
BOOST_LIB_DIR = /usr/local/lib
endif 
# ################################################## #

OS_LINK_FLAGS = -shared
TARGET_PREFIX = 
TARGET_DYNAMIC_SUFFIX = .plugd
TARGET_STATIC_SUFFIX = .a
INSTALL = ../../../bin

# Platform dependent flags. Do not indent.
ifeq ($(findstring Darwin,$(OS)),Darwin)
OS_LINK_FLAGS := -dynamiclib
endif

ifeq ($(findstring IRIX,$(OS)),IRIX)
OS := IRIX32
CXX = g++ #CC -n32 -LANG:std -woff 1682 -mips3
CXXFLAGS = -mabi=n32 -mips4  
LIB_OS = -L/usr/freeware/lib32 #lib32
endif

INCLUDES = \
	-I./ \
	-I../ \
	-I../../../GenericNurbs/ \
	-I$(USUL_INC_DIR) \
	-I$(OSG_INC_DIR) \
	-I$(FOX_INC_DIR)

LIBS = \
	-lUsul  -losg -losgDB -losgUtil -losgText\
 	-lOpenThreads -lOsgTools

LIB_DIRS = \
		-L$(USUL_LIB_DIR) -L$(OSG_LIB_DIR) \
		-L$(OSG_LIB_DIR)/osgPlugins \
		-L$(FOX_LIB_DIR)

flags = -g -D_DEBUG -D_COMPILING_$(COMPILE_GUARD)

BUILD = $(OS)
ARCHIVE_LIB = $(TARGET_PREFIX)$(TARGET)$(TARGET_STATIC_SUFFIX)
SHARED_LIB  = $(TARGET_PREFIX)$(TARGET)$(TARGET_DYNAMIC_SUFFIX)

OBJS = $(patsubst %.cpp,%.o,$(subst ./,$(OS)/,$(shell find . -type file -name "*.cpp")))
  
BUILD_DIRS:= \
  $(dir $(subst ./,$(OS)/,$(shell find . -type file -name "*.cpp")))
  
make_dirs:
	@echo '@@@@@ Building $(PKG_NAME) @@@@@@'
	mkdir -p $(BUILD_DIRS)

shared_lib: $(OBJS)
	@echo '@@@@@ Linking Dynamic Library... @@@@@'
	$(CXX) $(OS_LINK_FLAGS) -g $(OBJS) -o $(BUILD)/$(SHARED_LIB) $(LIB_DIRS) $(LIBS)
	mkdir -p $(INSTALL)
	cp $(BUILD)/$(SHARED_LIB) $(INSTALL)

archive_lib: $(OBJS) 
	@echo '@@@@@ Linking Static Library @@@@@'
	$(RM) $(BUILD)/$(ARCHIVE_LIB)
	ar -cr $(BUILD)/$(ARCHIVE_LIB) $(OBJS)
#	cp $(BUILD)/$(ARCHIVE_LIB) $(INSTALL)

$(OBJS): $(BUILD)/%.o: %.cpp
	@echo "[$(PKG_NAME)]:Compiling $< into $@"
	$(CXX) $(OS_COMP_FLAGS) $(flags) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) -rf $(BUILD) so_locations

real-clean: clean
	$(RM) $(INSTALL)/$(SHARED_LIB)
	$(RM) $(INSTALL)/$(ARCHIVE_LIB)
	
mvlibs:
	mkdir -p $(MOVE_LOCATION)
	mv ./$(OS)/*.so ./$(OS)/*.a $(MOVE_LOCATION)