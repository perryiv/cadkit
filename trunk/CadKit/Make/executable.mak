# Include the common definitions.
include $(CADKIT_ROOT_DIR)/Make/common.mak

# By default we compile the target.
default: $(TARGET)

# Include the common targets.
include $(CADKIT_ROOT_DIR)/Make/targets.mak

# Clobber the generated files (executable too).
clobber:
	rm -f $(OBJECTS) *~ core Makedepend Makedepend.bak
	rm -fr ii_files
	rm -f $(CADKIT_ROOT_DIR)/bin/$(TARGET)
	rm -f $(TARGET)
	echo ----- Target $(TARGET) clobbered! -----

# Link the object files into the executable.
$(TARGET): $(OBJECTS)
	$(CPP) $(OBJECTS) $(LINK_FLAGS) $(LIBS) -o $@
	mkdir -p $(CADKIT_ROOT_DIR)/bin
	cp $(TARGET) $(CADKIT_ROOT_DIR)/bin/.
	echo ----- Target $(TARGET) successfully built! -----
