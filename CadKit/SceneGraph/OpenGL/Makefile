# Include the header to set up the environment.
include $(CADKIT_ROOT_DIR)/Make/header.mak

# C++ source files.
include ./source.mak

# The include path.
LOCAL_INCLUDE =

# The libraries to link to.
LOCAL_LIBS = -lCkSg_s -lCkSl_s

# Any flags for the compiler.
LOCAL_CPPFLAGS =

# The name of the thing we are making.
TARGET = CkSgGl

# By default we compile a library.
default: lib

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/library.mak
