# C++ source files.
CPPFILES = Matrix4.cpp

# The include path.
INCLUDE = \
	-I./ \
	-I$(CADKIT_ROOT_DIR)

# The libraries to link to.
LIBS = 

# Any flags for the compiler.
CPPFLAGS = -D_LINUX -D_DEBUG

# The name of the thing we are making.
TARGET = matrix4

# By default we compile the executable.
default: $(TARGET)

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/MakeExecutable.in
