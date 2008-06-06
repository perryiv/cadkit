# List these in the order that they need to be compiled.
SUBDIRS = \
	Standard \
	Controllers \
	Database/Base \
	Database/Jupiter \
	Database/OpenSceneGraph \
	Database/Performer \
	Database/STL \
	Database/ITRI \
	Demos/Jupiter2Osg \
	Demos/Jupiter2Performer \
	Demos/Jupiter2Stl \
	Demos/Jupiter2Itri

# This says to make in all the subdirs. We use $(MAKE) instead of
# "make" because it could be "gmake".
all:
	$(MAKE) $(SUBDIRS)

# This tells it to cd to the subdir and then make.
$(SUBDIRS): FORCE
	if [ -d $@ ]; then cd $@ ; $(MAKE); fi

# This tells it to go in each subdir and execute a "make depend".
depend:
	for i in $(SUBDIRS); \
		do ( if [ -d $$i ]; then cd $$i ; $(MAKE) $@; fi; ) ; \
	done

# This tells it to go in each subdir and execute a "make clean",
clean:
	for i in $(SUBDIRS); \
		do ( if [ -d $$i ]; then cd $$i ; $(MAKE) $@; fi; ) ; \
	done

# This tells it to go in each subdir and execute a "make clobber",
clobber:
	for i in $(SUBDIRS); \
		do ( if [ -d $$i ]; then cd $$i ; $(MAKE) $@; fi; ) ; \
	done

# Without this it then it doesn't make in the subdirs, it just cd's there.
FORCE:
