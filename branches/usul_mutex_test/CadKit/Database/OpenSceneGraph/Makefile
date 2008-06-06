# Include the header to set up the environment.
include $(CADKIT_ROOT_DIR)/Make/header.mak

# C++ source files.
CPP_FILES = \
	DbOsgArraySetters.cpp \
	DbOsgDatabase.cpp \
	DbOsgDll.cpp \
	DbOsgFunctions.cpp \
	DbOsgPrecompiled.cpp

# The include path.
LOCAL_INCLUDE = -I$(OSGHOME)/include

# OSG gets "installed" in a different directory, depending on the platform
ifeq ($(OS), IRIX)
	LOCAL_OSG_LIB_DIR = $(OSGHOME)/lib32
else
	LOCAL_OSG_LIB_DIR = $(OSGHOME)/lib
endif

# The libraries to link to.
LOCAL_LIBS = \
	-lCkSl_s \
	-lCkDbBase_s \
	-L$(LOCAL_OSG_LIB_DIR) \
	-losgDB \
	-losg 

# Any flags for the compiler. 
# Note: not sure how to do an "else if".
ifeq ($(OS), IRIX)
	LOCAL_CPP_FLAGS = -woff 3303
else
	LOCAL_CPP_FLAGS =
endif

# The name of the thing we are making.
TARGET = CkDbOsg

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/library.mak
