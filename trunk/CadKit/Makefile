# List these in the order that they need to be compiled.
SUBDIRS = \
	Standard \
	SceneGraph/Core \
	SceneGraph/OpenGL \
	Examples/BoundingBox

# This says to make in all the subdirs.
all:
	make $(SUBDIRS)

# This tells it to cd to the subdir and then make.
$(SUBDIRS): FORCE
	if [ -d $@ ]; then cd $@ ; make; fi

# This tells it to go in each subdir and execute a "make depend".
depend:
	for i in $(SUBDIRS); \
		do ( if [ -d $$i ]; then cd $$i ; make $@; fi; ) ; \
	done

# This tells it to go in each subdir and execute a "make clean",
clean:
	for i in $(SUBDIRS); \
		do ( if [ -d $$i ]; then cd $$i ; make $@; fi; ) ; \
	done

# This tells it to go in each subdir and execute a "make clobber",
clobber:
	for i in $(SUBDIRS); \
		do ( if [ -d $$i ]; then cd $$i ; make $@; fi; ) ; \
	done

# Without this it then it doesn't make in the subdirs, it just cd's there.
FORCE:
