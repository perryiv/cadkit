
# Include all variables, flags, etc...
include $(CADKIT_MAKE_DIR)/header.mak

# C++ source files.
CPP_FILES = \
	./WebGen.cpp \
	./TranslateTags.cpp \
	./Functions.cpp \
	./Main.cpp

LOCAL_INCLUDE = \
	-I$(CADKIT_INC_DIR) \
	-I../

LOCAL_LIBS = \
	$(CADKIT_LIB_DIR)/libXmlTree$(TARGET_SUFFIX).a \
	$(CADKIT_LIB_DIR)/libUsul$(TARGET_SUFFIX).a \
	-lxerces-c \
	-ldl \
	-luuid


# The name of the thing we are making.
TARGET = make_page

# Override the target directory.
TARGET_DIR = $(HOME)/www/cgi-bin

# Do this after the target builds.
#POST_BUILD_COMMAND = chcon -t httpd_sys_content_t $(TARGET_DIR)/$(TARGET)

# Include the rules for make...
include $(CADKIT_MAKE_DIR)/executable.mak
