# Include the header to set up the environment.
include $(CADKIT_ROOT_DIR)/Make/header.mak

# C++ source files.
CPP_FILES = \
	DbStlDatabase.cpp \
	DbStlDll.cpp

# The include path.
LOCAL_INCLUDE = -I$(OSGHOME)/include

# The libraries to link to.
LOCAL_LIBS = \
	-lCkSl_s \
	-lCkDbBase_s

# Any flags for the compiler. 
LOCAL_CPP_FLAGS =

# The name of the thing we are making.
TARGET = CkDbStl

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/library.mak
