# Include the header to set up the environment.
include $(CADKIT_ROOT_DIR)/Make/header.mak

# C++ source files.
CPP_FILES = \
	TrJt2Pf.cpp \
	TrJt2PfDll.cpp \
	TrJt2PfPrecompiled.cpp

# The include path.
LOCAL_INCLUDE =

# The libraries to link to.
LOCAL_LIBS = \
	-lCkDbJt_s \
	-lCkSl_s \
	-lpfutil \
	-lpf \
	-L$(DMDTK_DEV_PATH)/lib/$(os) \
	-lDMDataTk$(DMDTK_VERSION)

# Any flags for the compiler. 
#LOCAL_CPP_FLAGS = -D_CADKIT_SL_PRINT_DEBUG
LOCAL_CPP_FLAGS =

# The name of the thing we are making.
TARGET = CkTrJt2Pf

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/library.mak
