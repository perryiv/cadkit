# The compiler.
CPP = g++

# Tell it to make the .cpp files.
OBJECTS = $(CPPFILES:.cpp=.o)

# Compile the source files.
.cpp.o:
	$(CPP) -c $(INCLUDE) $(CPPFLAGS) $< -o $@

# Clean the generated files.
clean:
	rm -f $(OBJECTS)

# Clobber the generated files (executable too).
clobber:
	rm -f $(OBJECTS) *~ core Makedepend
	rm -f $(CADKIT_ROOT_DIR)/bin/$(TARGET)

# Automatically generated dependencies.
depend:
	rm -f Makedepend;
	touch Makedepend;
	makedepend -f Makedepend -- $(INCLUDE) $(CPPFLAGS) -- $(CPPFILES);

# Link the object files into the executable.
$(TARGET): $(OBJECTS)
	$(CPP) $(OBJECTS) $(LIBS) -o $@
	mkdir -p $(CADKIT_ROOT_DIR)/bin
	mv $(TARGET) $(CADKIT_ROOT_DIR)/bin/.
