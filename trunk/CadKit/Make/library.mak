# The C++ compiler's flags.
CPP_FLAGS = $(GLOBAL_CPP_FLAGS) $(LOCAL_CPP_FLAGS)

# The include path.
INCLUDE = -I./ -I$(CADKIT_ROOT_DIR) $(LOCAL_INCLUDE)

# The libraries to link to.
LIBS = -L$(CADKIT_ROOT_DIR)/bin $(LOCAL_LIBS)

# Tell it to make the .cpp files.
OBJECTS = $(CPP_FILES:.cpp=.o)

# By default we compile a library.
default: lib

# Compile the source files.
.cpp.o:
	$(CPP) -c $(INCLUDE) $(CPP_FLAGS) $< -o $@

# Clean the generated files.
clean:
	rm -f $(OBJECTS)
	echo ----- Target $(TARGET) cleaned! -----

# Clobber the generated files (libraries too).
clobber:
	rm -f $(OBJECTS) *~ core Makedepend Makedepend.bak so_locations
	rm -fr ii_files
	rm -f $(CADKIT_ROOT_DIR)/bin/lib$(TARGET)_s.so
	rm -f $(CADKIT_ROOT_DIR)/bin/lib$(TARGET)_a.a
	echo ----- Target $(TARGET) clobbered! -----

# Automatically generated dependencies.
depend:
	rm -f Makedepend;
	touch Makedepend;
	makedepend -f Makedepend -- $(INCLUDE) $(CPP_FLAGS) -- $(CPP_FILES);
	echo ----- Target $(TARGET) dependencies created! ----

# Link the object files into the library.
lib: $(OBJECTS)
	rm -rf lib$(TARGET)_a.a
	ar -cr lib$(TARGET)_a.a $(OBJECTS)
	$(CPP) -shared -Wl,-soname,lib$(TARGET)_s.so -o lib$(TARGET)_s.so $(OBJECTS) $(LIBS)
	mkdir -p $(CADKIT_ROOT_DIR)/bin
	mv lib$(TARGET)_s.so $(CADKIT_ROOT_DIR)/bin/.
	mv lib$(TARGET)_a.a $(CADKIT_ROOT_DIR)/bin/.
	echo ----- Target $(TARGET) successfully built! -----
