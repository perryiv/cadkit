# Include the common definitions.
include $(USUL_INC_DIR)/Usul/Make/common.mak

# By default we compile a library.
default: lib

# Include the common targets.
include $(USUL_INC_DIR)/Usul/Make/targets.mak

# Clobber the generated files (libraries too).
clobber:
	rm -f $(OBJECTS) *~ core Makedepend Makedepend.bak so_locations
	rm -fr ii_files
	rm -f lib$(TARGET)_s.so lib$(TARGET).a
	rm -f $(USUL_INC_DIR)/bin/lib$(TARGET).so
	rm -f $(USUL_INC_DIR)/bin/lib$(TARGET).a
	echo ----- Target $(TARGET) clobbered! -----

# Link the object files into the library.
lib: $(OBJECTS)
	rm -rf lib$(TARGET).a
	ar -cr lib$(TARGET).a $(OBJECTS)
	$(CPP) $(LINK_FLAGS) -fPIC -shared -Wl,-soname,lib$(TARGET).so -o lib$(TARGET).so $(OBJECTS) $(LIBS)
	mkdir -p $(USUL_INC_DIR)/bin
	mv lib$(TARGET).so $(USUL_INC_DIR)/bin/.
	mv lib$(TARGET).a $(USUL_INC_DIR)/bin/.
	echo ----- Target $(TARGET) successfully built! -----
