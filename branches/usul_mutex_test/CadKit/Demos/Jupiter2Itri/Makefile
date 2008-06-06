# Include the header to set up the environment.
include $(CADKIT_ROOT_DIR)/Make/header.mak

# C++ source files.
CPP_FILES = main.cpp

# The include path.
LOCAL_INCLUDE = 

# The libraries to link to.
LOCAL_LIBS = \
	-lCkCt_s \
	-lCkDbJt_s \
	-lCkDbItri_s \
	-lCkSl_s

# Any flags for the compiler. 
LOCAL_CPP_FLAGS = 

# Any flags for the linker.
LOCAL_LINK_FLAGS =

# The name of the thing we are making.
TARGET = jt2itri

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/executable.mak
