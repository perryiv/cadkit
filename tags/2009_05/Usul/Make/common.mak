# The C++ compiler's flags.
CPP_FLAGS = $(GLOBAL_CPP_FLAGS) $(PLATFORM_CPP_FLAGS) $(LOCAL_CPP_FLAGS)

# The linker's flags.
LINK_FLAGS = $(GLOBAL_LINK_FLAGS) $(PLATFORM_LINK_FLAGS) $(LOCAL_LINK_FLAGS)

# The include path.
INCLUDE = $(LOCAL_INCLUDE) $(PLATFORM_INCLUDE_FLAGS) -I./

# The libraries to link to.
LIBS = -L$(CADKIT_LIB_DIR) $(LOCAL_LIBS)

# Tell it to make the .cpp files.
OBJECTS = $(CPP_FILES:.cpp=.o)

# Compile the source files.
.cpp.o:
	$(CPP) -c $(INCLUDE) $(CPP_FLAGS) $< -o $@
