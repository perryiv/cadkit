# Include the header to set up the environment.
include $(CADKIT_ROOT_DIR)/Make/header.mak

# C++ source files.
CPP_FILES = \
	DbJtDatabase.cpp \
	DbJtDll.cpp \
	DbJtFunctions.cpp \
	DbJtPrecompiled.cpp \
	DbJtTargets.cpp \
	DbJtTraversalState.cpp \
	DbJtTraverser.cpp \
	DbJtVisApiVersion.cpp \
	DbJtVisApi.cpp

# The include path.
LOCAL_INCLUDE = -I$(DMDTK_DEV_PATH)/include/

# The libraries to link to.
LOCAL_LIBS = -lCkDbBase_s -lCkSl_s -L$(DMDTK_DEV_PATH)/lib/$(os) -lDMDataTk$(DMDTK_VERSION)

# Any flags for the compiler. 
# EAI's header files create these warnings.
# Note: not sure how to do an "else if".
ifeq ($(CPP), g++)
	LOCAL_CPP_FLAGS = -fpermissive
else
	ifeq ($(OS), IRIX)
		LOCAL_CPP_FLAGS = -woff 1681,3322
	else
		LOCAL_CPP_FLAGS =
	endif
endif

# The name of the thing we are making.
TARGET = CkDbJt

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/library.mak
