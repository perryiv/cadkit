# C++ source files.
CPPFILES = Pathname.cpp

# The include path.
INCLUDE = \
	-I./ \
	-I$(CADKIT_ROOT_DIR) \
	-I/usr/include/g++-3

# The libraries to link to.
LIBS = 

# Any flags for the compiler.
CPPFLAGS = -D_LINUX -D_DEBUG

# The name of the thing we are making.
TARGET = pathname

# By default we compile the executable.
default: $(TARGET)

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/MakeExecutable.in
