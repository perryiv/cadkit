# Include the header to set up the environment.
include $(CADKIT_ROOT_DIR)/Make/header.mak

# C++ source files.
CPP_FILES = \
	TrJt2Pf.cpp \
	TrJt2PfDll.cpp \
	TrJt2PfPrecompiled.cpp

# The include path.
LOCAL_INCLUDE = -I/usr/include/Performer

# The libraries to link to.
LOCAL_LIBS = \
	-lCkDbJt_s \
	-lCkDbXml_s \
	-lCkSl_s \
	-lpfdu \
	-lpfui \
	-lpfutil \
	-lpf \
	-L$(DMDTK_DEV_PATH)/lib/$(os) -lDMDataTk40

# Any flags for the compiler. 
LOCAL_CPP_FLAGS = 

# The name of the thing we are making.
TARGET = CkTrJt2Xml

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/library.mak
