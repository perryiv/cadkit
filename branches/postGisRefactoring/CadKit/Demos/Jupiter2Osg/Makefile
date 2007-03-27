# Include the header to set up the environment.
include $(CADKIT_ROOT_DIR)/Make/header.mak

# C++ source files.
CPP_FILES = main.cpp

# The include path.
LOCAL_INCLUDE = 

# OSG gets "installed" in a different directory, depending on the platform
ifeq ($(OS), IRIX)
	LOCAL_OSG_LIB_DIR = $(OSGHOME)/lib32
else
	LOCAL_OSG_LIB_DIR = $(OSGHOME)/lib
endif

# The libraries to link to.
LOCAL_LIBS = \
	-lCkCt_s \
	-lCkDbJt_s \
	-lCkDbOsg_s \
	-lCkSl_s \
	-L$(LOCAL_OSG_LIB_DIR) \
	-losgDB \
	-losg \
	-L$(DMDTK_DEV_PATH)/lib/$(os) \
	-lDMDataTk$(DMDTK_VERSION)

# Any flags for the compiler. 
LOCAL_CPP_FLAGS = 

# Any flags for the linker.
LOCAL_LINK_FLAGS =

# The name of the thing we are making.
TARGET = jt2osg

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/executable.mak
