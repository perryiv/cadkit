# C++ source files.
CPP_FILES = \
	DbJtDll.cpp \
	DbJtPrecompiled.cpp \
	DbJtTraverser.cpp

# The include path.
LOCAL_INCLUDE = -I$(DMDTK_DEV_PATH)/include/

# The libraries to link to.
LOCAL_LIBS = -lCkSl_s -L$(DMDTK_DEV_PATH)/lib/$(os) -lDMDataTk40

# Any flags for the compiler. 
# EAI's header files create these warnings.
LOCAL_CPP_FLAGS = -woff 1681,3322

# The name of the thing we are making.
TARGET = CkDbJt

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/library.mak
