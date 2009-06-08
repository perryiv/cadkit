
# Include all variables, flags, etc...
include $(CADKIT_MAKE_DIR)/header.mak

# C++ source files.
CPP_FILES = \
	./Minerva/Extents/ExtentsTest.cpp \
	./Minerva/Ellipsoid/EllipsoidTest.cpp \

LOCAL_INCLUDE = \
	-I$(CADKIT_INC_DIR) \
	-I../ \
	-F${HOME}/Library/Frameworks \

LOCAL_LIBS = \
	/usr/local/lib/libgtest.a \
	/usr/local/lib/libgtest_main.a \
	-L${CADKIT_LIB_DIR} \
	-lUsul \
	-lXmlTree \
	-lSerializeXML \
	-lMinerva \
	-F${HOME}/Library/Frameworks \
	-framework curl -framework osgUtil -framework osgDB -framework ossim -framework Boost -framework osgText -framework osg


# The name of the thing we are making.
TARGET = cadkit_unit_tests

# Include the rules for make...
include $(CADKIT_MAKE_DIR)/executable.mak
