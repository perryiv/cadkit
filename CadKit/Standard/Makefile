# Source files go in here.
include MakeSource.in

# The include path.
INCLUDE = -I./

# The libraries to link to.
LIBS = 

# Any flags for the compiler.
CPPFLAGS = -D_LINUX -D_DEBUG

# The name of the thing we are making.
TARGET = CkSl

# By default we compile a library.
default: lib

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/MakeLibrary.in
