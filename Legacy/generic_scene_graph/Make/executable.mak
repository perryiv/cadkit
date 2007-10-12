# Include the common definitions.
include $(GSG_ROOT_DIR)/Make/common.mak

# By default we compile the target.
default: release

# Include the common targets.
include $(GSG_ROOT_DIR)/Make/targets.mak

# Clobber the generated files (executable too).
clobber:
	rm -f $(OBJECTS) *~ core Makedepend Makedepend.bak
	rm -fr ii_files
	rm -f $(GSG_ROOT_DIR)/bin/$(TARGET)$(EXECUTABLE_EXTENSION)
	rm -f $(TARGET)$(EXECUTABLE_EXTENSION)
	echo ----- Target $(TARGET) clobbered! -----

# Link the object files into the executable.
release: $(OBJECTS)
	$(CPP) $(OBJECTS) $(LINK_FLAGS) $(LIBS) -o $(TARGET)
	mkdir -p $(GSG_ROOT_DIR)/bin
	mv $(TARGET)$(EXECUTABLE_EXTENSION) $(GSG_ROOT_DIR)/bin/.
	echo ----- Target $(TARGET) successfully built! -----

# Link the object files into the executable.
debug: $(OBJECTS)
	$(CPP) $(OBJECTS) $(LINK_FLAGS) $(DEBUG_CPP_FLAGS) $(LIBS) -o $(TARGET)d
	mkdir -p $(GSG_ROOT_DIR)/bin
	mv $(TARGET)d$(EXECUTABLE_EXTENSION) $(GSG_ROOT_DIR)/bin/.
	echo ----- Target $(TARGET) successfully built! -----
