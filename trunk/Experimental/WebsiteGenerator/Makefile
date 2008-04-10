
# Include all variables, flags, etc...
include $(CADKIT_MAKE_DIR)/header.mak

# C++ source files.
CPP_FILES = \
	./WebGen.cpp \
	./Main.cpp

LOCAL_INCLUDE = \
	-I$(CADKIT_INC_DIR) \
	-I../

LOCAL_LIBS = \
	-L$(CADKIT_LIB_DIR) \
	-lUsul$(TARGET_SUFFIX) \
	-lXmlTree$(TARGET_SUFFIX) \
	-lxerces-c


# The name of the thing we are making.
TARGET = make_page

# Override the target directory.
TARGET_DIR = $(HOME)/www/cgi-bin/helios

# Do this after the target builds.
POST_BUILD_COMMAND = chcon -t httpd_sys_content_t $(TARGET_DIR)/$(TARGET)

# Include the rules for make...
include $(CADKIT_MAKE_DIR)/executable.mak
