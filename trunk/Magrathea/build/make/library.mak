# Include the common definitions.
include common.mak

# By default we compile a library.
default: lib

# Include the common targets.
include targets.mak

# Clobber the generated files (libraries too).
clobber:
	rm -f $(OBJECTS) *~ core Makedepend Makedepend.bak so_locations
	rm -fr ii_files
	rm -f lib$(TARGET)_s.so lib$(TARGET)_a.a
	rm -f $(USUL_INC_DIR)/bin/lib$(TARGET)_s.so
	rm -f $(USUL_INC_DIR)/bin/lib$(TARGET)_a.a
	echo ----- Target $(TARGET) clobbered! -----

# Link the object files into the library.
lib: $(OBJECTS)
	rm -rf lib$(TARGET)_a.a
	ar -cr lib$(TARGET)_a.a $(OBJECTS)
	$(CPP) $(LINK_FLAGS) -fPIC -shared -Wl,-soname,lib$(TARGET)_s.so -o lib$(TARGET)_s.so $(OBJECTS) $(LIBS)
	mkdir -p $(USUL_INC_DIR)/bin
	mv lib$(TARGET)_s.so $(USUL_INC_DIR)/bin/.
	mv lib$(TARGET)_a.a $(USUL_INC_DIR)/bin/.
	echo ----- Target $(TARGET) successfully built! -----
