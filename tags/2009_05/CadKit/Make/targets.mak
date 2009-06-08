# Clean the generated files.
clean:
	rm -f $(OBJECTS)
	echo ----- Target $(TARGET) cleaned! -----

# Automatically generated dependencies.
depend:
	rm -f Makedepend;
	touch Makedepend;
	makedepend -f Makedepend -- $(INCLUDE) $(CPP_FLAGS) -- $(CPP_FILES);
	echo ----- Target $(TARGET) dependencies created! ----
