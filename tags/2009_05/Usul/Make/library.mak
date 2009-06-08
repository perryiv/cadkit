# Include the common definitions.
include $(CADKIT_MAKE_DIR)/common.mak

# By default we compile a library.
default: lib

# Include the common targets.
include $(CADKIT_MAKE_DIR)/targets.mak

# Clobber the generated files (libraries too).
clobber:
	rm -f $(OBJECTS) *~ core Makedepend Makedepend.bak so_locations
	rm -fr ii_files
	rm -f lib$(TARGET)_s.so lib$(TARGET).a
	rm -f $(TARGET_DIR)/lib$(TARGET).so
	rm -f $(TARGET_DIR)/lib$(TARGET).a
	echo ----- Target $(TARGET) clobbered! -----

# Link the object files into the library.
lib: $(OBJECTS)
	rm -rf lib$(TARGET).a
	ar -cr lib$(TARGET).a $(OBJECTS)
	$(CPP) $(LINK_FLAGS) -fPIC -shared -Wl,-soname,lib$(TARGET).so -o lib$(TARGET).so $(OBJECTS) $(LIBS)
	mkdir -p $(TARGET_DIR)
	mv lib$(TARGET).so $(TARGET_DIR)/.
	mv lib$(TARGET).a $(TARGET_DIR)/.
	echo ----- Target $(TARGET) successfully built! -----
