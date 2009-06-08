# Include the header to set up the environment.
include $(CADKIT_ROOT_DIR)/Make/header.mak

# C++ source files.
CPP_FILES = Nurbs.cpp

# The include path.
LOCAL_INCLUDE =

# The libraries to link to.
LOCAL_LIBS = -lm

# Any flags for the compiler. 
# Note: not sure how to do an "else if".
LOCAL_CPP_FLAGS =
ifeq ($(OS), IRIX)
	LOCAL_CPP_FLAGS = -woff 1183
endif

# Any flags for the linker.
LOCAL_LINK_FLAGS =

# The name of the thing we are making.
TARGET = nurbs

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/executable.mak
