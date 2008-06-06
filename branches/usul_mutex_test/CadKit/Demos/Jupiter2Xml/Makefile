# Include the header to set up the environment.
include $(CADKIT_ROOT_DIR)/Make/header.mak

# C++ source files.
CPP_FILES = main.cpp

# The include path.
LOCAL_INCLUDE = 

# The libraries to link to.
LOCAL_LIBS = \
	-lCkTrJt2Xml_s \
	-lCkDbJt_s \
	-lCkDbXml_s \
	-lCkSl_s \
	-L$(DMDTK_DEV_PATH)/lib/$(os) -lDMDataTk$(DMDTK_VERSION)

# Any flags for the compiler. 
LOCAL_CPP_FLAGS = 

# Any flags for the linker.
LOCAL_LINK_FLAGS =

# The name of the thing we are making.
TARGET = jt2xml

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/executable.mak
