# Source files go in here.
include MakeSource.in

# The include path.
INCLUDE = \
	-I./ \
	-I$(CADKIT_ROOT_DIR)

# The libraries to link to.
LIBS = \
	-L$(CADKIT_ROOT_DIR)/bin \
	-lCkSg_s \
	-lCkSl_s

# Any flags for the compiler.
CPPFLAGS = -D_LINUX -D_DEBUG -w

# The name of the thing we are making.
TARGET = CkSgGl

# By default we compile a library.
default: lib

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/MakeLibrary.in
