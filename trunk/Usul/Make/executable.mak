# Include the common definitions.
include $(CADKIT_MAKE_DIR)/common.mak

# By default we compile the target.
default: release

# Include the common targets.
include $(CADKIT_MAKE_DIR)/targets.mak

# Clobber the generated files (executable too).
clobber:
	rm -f $(OBJECTS) *~ core Makedepend Makedepend.bak
	rm -fr ii_files
	rm -f $(TARGET_DIR)/$(TARGET)$(EXECUTABLE_EXTENSION)
	rm -f $(TARGET)$(EXECUTABLE_EXTENSION)
	echo ----- Target $(TARGET) clobbered! -----

# Link the object files into the executable.
release: $(OBJECTS)
	$(CPP) $(OBJECTS) $(LINK_FLAGS) $(LIBS) -o $(TARGET)
	mkdir -p $(TARGET_DIR)
	mv $(TARGET)$(EXECUTABLE_EXTENSION) $(TARGET_DIR)/.
	$(POST_BUILD_COMMAND)
	echo ----- Target $(TARGET) successfully built! -----

# Link the object files into the executable.
debug: $(OBJECTS)
	$(CPP) $(OBJECTS) $(LINK_FLAGS) $(DEBUG_CPP_FLAGS) $(LIBS) -o $(TARGET)d
	mkdir -p $(TARGET_DIR)
	mv $(TARGET)d$(EXECUTABLE_EXTENSION) $(TARGET_DIR)/.
	$(POST_BUILD_COMMAND)
	echo ----- Target $(TARGET) successfully built! -----
