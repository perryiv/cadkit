# Include the header to set up the environment.
include $(CADKIT_ROOT_DIR)/Make/header.mak

# C++ source files.
CPP_FILES = \
	DbBaseDll.cpp \
	DbBaseObject.cpp \
	DbBasePrecompiled.cpp \
	DbBaseSource.cpp \
	DbBaseTarget.cpp

# The include path.
LOCAL_INCLUDE =

# The libraries to link to.
LOCAL_LIBS = -lCkSl_s

# Any flags for the compiler. 
LOCAL_CPP_FLAGS =

# The name of the thing we are making.
TARGET = CkDbBase

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/library.mak
